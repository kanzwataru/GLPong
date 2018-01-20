//
//  gameplay.c
//  GLPong
//
//  Created by 神崎航 on 2017/12/31.
//  Copyright © 2017年 神崎航. All rights reserved.
//

#include "gameplay.h"

#include <stdlib.h>
#include <math.h>

#include "util_funcs.h"
#include "globals.h"

static World wo;
static RenderInfo rinfo;

static int ball_collides_topbtm(const Ball *ball) {
    if(ball->sprite.rect.y + (ball->sprite.rect.h / 2) > 1.0f)  return -1;   /* top */
    if(ball->sprite.rect.y - (ball->sprite.rect.h / 2) < -1.0f) return 1;  /* btm */
    else /* -----------------------------------------------> */ return 0;   /* no collison */
}

static int ball_collides_leftright(const Ball *ball) {
    if(ball->sprite.rect.x - (ball->sprite.rect.w / 2) > rinfo.x_max)  return -1;   /* right */
    if(ball->sprite.rect.x + (ball->sprite.rect.w / 2) < rinfo.x_min)  return 1;  /* left */
    else /* ------------------------------------------------------> */ return 0;   /* no collison */
}

static void ball_bounce(Ball *ball, int top_btm, int left_right) {
    if(top_btm != 0)
        ball->vel_y = top_btm * fabsf(ball->vel_y);
    if(left_right != 0)
        ball->vel_x = left_right * fabsf(ball->vel_x);

    /* prevent getting stuck at top/btm edges
     * or stopping completely */
    if(ball->sprite.rect.y > 0.98f
      || ball->sprite.rect.y < -0.98f)
    {
        ball->vel_y = ball->vel_y == 0 ? 1 : ball->vel_y;
    }
}

static void ball_smash(Ball *ball, float speed, int move_dir, float percent) {
    ball->vel_y = percent + (speed * -move_dir * SMACK_FORCE_MULT);

    printf("ball->vel_y: %f speed: %f move_dir: %d percent: %f\n", ball->vel_y,speed,move_dir,percent);
}

static void update_ball(Ball *ball) {
    if(ball->parent) {
        ball->vel_x = 1;
        ball->vel_y = 1;

        int side = ball->parent->x > SCREEN_HALF ? -1 : 1;
        ball->sprite.rect.x = (ball->parent->x + (side * (ball->parent->w / 2))
                                         + (side * (ball->sprite.rect.w / 2)));
        ball->sprite.rect.y = ball->parent->y;
    }
    else {
        ball->sprite.rect.x += (BALL_SPEED * ball->vel_x);
        ball->sprite.rect.y += (BALL_SPEED * ball->vel_y);
    }
}

static float ai_predict(const Rect *ball_r) {
    static int predict_counter;
    static float last_prediction;

    if(predict_counter > AI_PREDICT_STEP) {
        predict_counter = 0;

        last_prediction = ball_r->y;
    }
    else {
        ++predict_counter;
    }

    /*
    printf("predict_counter: %d last_prediction: %f\n",
            predict_counter,    last_prediction); */

    return last_prediction;
}

static void update_ai(Paddle *ai, const Rect *ball_r) {
    Rect *pad_r = &ai->sprite.rect; /* いちいち書くのめんどい */
    float prediction = ai_predict(ball_r);

    /* up or down */
    if(ball_r->x > AI_MIN_REACT && ball_r->x < AI_MAX_REACT) {
        ai->direction = (prediction < pad_r->y) ? -1 : ai->direction;
        ai->direction = (prediction > pad_r->y) ?  1 : ai->direction;
    }

    /* Should be able to stop any time */
    if(prediction > (pad_r->y + (AI_STOP_MIN * pad_r->h))
    && prediction < (pad_r->y + (AI_STOP_MAX * pad_r->h)))
    {
        ai->direction = 0;
    }

    //printf("ai_dir: %d ai_prediction: %f\n", ai->direction, prediction);
}

static float calc_speed(float current_speed, int direction)
{
    return f_clamp((current_speed + (PADDLE_ACCEL * direction))
                                  * (1.0f - (PADDLE_DECEL * !abs(direction))), -PADDLE_SPEED, PADDLE_SPEED);
}

static void update_paddle(Paddle *paddle) {
    paddle->speed = calc_speed(paddle->speed, paddle->direction);
    paddle->sprite.rect.y = f_clamp(paddle->sprite.rect.y + paddle-> speed,
                                    BTM, TOP);
}

static int ball_collides_paddle(Ball *ball, const Paddle *paddle) {
    static const int LEFT = 1;
    static const int RIGHT = -1;

    const Rect *rect = &paddle->sprite.rect;
    const Rect *ball_r = &ball->sprite.rect;

    float percent = remap(ball_r->y, rect->y - (rect->h / 2),
                                     rect->y + (rect->h / 2),
                                     -1.0f, 1.0f);

    int side = rect->x > SCREEN_HALF ? RIGHT : LEFT;
    float ball_lf = ball_r->x - (ball_r->w * 0.5f);
    float ball_rt = ball_r->x + (ball_r->w * 0.5f);
    float rect_lf = rect->x - (rect->w * 0.5f);
    float rect_rt = rect->x + (rect->w * 0.5f);

    if(percent < 1.0f && percent > -1.0f) {
        if((side == LEFT  && ball_lf < rect_rt && ball_lf > rect_lf)
        || (side == RIGHT && ball_rt > rect_lf && ball_lf < rect_rt))
        {
            ball_smash(ball, paddle->speed, paddle->direction, percent);
            return side;
        }
    }

    return 0;
}


static void init_paddle(Paddle *pad) {
    pad->sprite.rect.w = PADDLE_W;
    pad->sprite.rect.h = PADDLE_H;
    pad->sprite.rect.x = 0.0f;
    pad->sprite.rect.y = 0.0f;
    pad->sprite.rotation = 0.0f;
    pad->sprite.depth = 0;
    pad->sprite.color = RND_WHITE;
}

void GMP_init(const RenderInfo renderinfo) {
    rinfo = renderinfo;

    init_paddle(&wo.player);
    init_paddle(&wo.ai);
    wo.player.sprite.rect.x = SCREEN_HALF - PADDLE_GAP;
    wo.ai.sprite.rect.x = SCREEN_HALF + PADDLE_GAP;

    wo.ball.sprite.rect.h = BALL_SIZE;
    wo.ball.sprite.rect.w = BALL_SIZE;
    wo.ball.sprite.rotation = 0.0f;
    wo.ball.sprite.depth = 0;
    wo.ball.sprite.color = RND_WHITE;
    wo.ball.parent = &wo.player.sprite.rect;
}

World GMP_update(void) {
    if(wo.paused)
        return wo;

    int ball_top_btm = ball_collides_topbtm(&wo.ball);
    int ball_left_right = ball_collides_leftright(&wo.ball);
    if(1 == ball_left_right) {
        wo.ai.score += 1;
        wo.ball.parent = &wo.player.sprite.rect;
    }
    else if(-1 == ball_left_right) {
        wo.player.score += 1;
        wo.ball.parent = &wo.player.sprite.rect;
    }
    else { /* If we didn't score, check if we hit any paddles */
        if(0 == ball_left_right)
            ball_left_right = ball_collides_paddle(&wo.ball, &wo.ai);
        if(0 == ball_left_right)
            ball_left_right = ball_collides_paddle(&wo.ball, &wo.player);
    }

    ball_bounce(&wo.ball, ball_top_btm, ball_left_right);

    update_ai(&wo.ai, &wo.ball.sprite.rect);
    update_ball(&wo.ball);
    update_paddle(&wo.player);
    update_paddle(&wo.ai);

    return wo;
}

void GMP_up_event(void) {
    wo.player.direction = 1;
}

void GMP_stop_event(void) {
    wo.player.direction = 0;
}

void GMP_down_event(void) {
    wo.player.direction = -1;
}

void GMP_serve_event(void) {
    wo.ball.parent = NULL;
}

void GMP_pause_event(void) {
    wo.paused = !wo.paused;
}
