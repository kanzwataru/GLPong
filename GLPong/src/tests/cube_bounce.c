//
//  cube_bounce.c
//  GLPong
//
//  Created by 神崎航 on 2018/01/05.
//  Copyright © 2018年 神崎航. All rights reserved.
//

#include "cube_bounce.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "render.h"
#include "globals.h"
#include "gameplay.h"
#include "util_funcs.h"

#define SPRITE_NUM 40000
#define SPRITE_ADD_FRAME 300
#define SPRITE_MIN_SPEED 0.003f
#define SPRITE_MAX_SPEED 0.01f
#define SPRITE_MIN_SIZE 0.01f
#define SPRITE_MAX_SIZE 0.5f

#define FPS_INTERVAL 1.0 //seconds.

Uint32 fps_lasttime; //the last recorded time.
Uint32 fps_current; //the current FPS.
Uint32 fps_frames = 0; //frames passed since the last recorded fps.

Sprite *sprites[SPRITE_NUM];
float speeds[SPRITE_NUM];
int dirs_x[SPRITE_NUM];
int dirs_y[SPRITE_NUM];
int sprite_count = 0;

int frame_count = 0;

static const Color col = {1.0f, 0.5f, 0.2f, 1.0f};
static const Color lf_col = {0.8f, 0.4f, 0.3f, 1.0f};
static const Color dwn_col = {0.25f, 0.8f, 1.0f, 1.0f};
static const Color up_col = {1.0f, 0.8f, 0.1f, 1.0f};

static const float ASPECT_RATIO = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

int calc_init_dir() {
    int val = i_range_rand(1, 10);
    return val >= 5 ? 1 : -1;
}

void add_sprite(int amount) {
    int total = sprite_count + amount;
    
    for(int i = sprite_count; i < total; i++) {
        if(sprite_count + 1 >= SPRITE_NUM)
            return;
        
        sprites[i] = malloc(sizeof(Sprite));
        sprites[i]->rect.x = f_range_rand(-1.0f, 1.0f);
        sprites[i]->rect.y = f_range_rand(-1.0f, 1.0f);
        sprites[i]->rect.w = SPRITE_MAX_SIZE;
        sprites[i]->rect.h = SPRITE_MAX_SIZE * ASPECT_RATIO;
        sprites[i]->depth = 0.0f;
        sprites[i]->color = col;
        
        dirs_x[i] = calc_init_dir();
        dirs_y[i] = calc_init_dir();
        
        speeds[i] = SPRITE_MIN_SPEED;
        
        sprite_count++;
    }
    
    float new_size = f_clamp(sprites[0]->rect.w * 0.7f, SPRITE_MIN_SIZE, SPRITE_MAX_SIZE);
    for(int i = 0; i < sprite_count; i++) {
        sprites[i]->rect.w = new_size;
        sprites[i]->rect.h = new_size * ASPECT_RATIO;
    }
}

void cube_bounce_run() {
    RND_init("GLPong", SCREEN_WIDTH, SCREEN_HEIGHT);
    
    fps_lasttime = SDL_GetTicks();
    add_sprite(1);
    
    bool running = true;
    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
            }
        }
        
        Rect *rect;
        for(int i = 0; i < sprite_count; i++) {
            rect = &sprites[i]->rect;
            rect->x += speeds[i] * dirs_x[i];
            rect->y += speeds[i] * dirs_y[i];
            
            if(rect->x + rect->w >= 1.0f) {
                dirs_x[i] = -1;
                speeds[i] = f_range_rand(SPRITE_MIN_SPEED, SPRITE_MAX_SPEED);
                sprites[i]->color = col;
            }
            if(rect->x <= -1.0f) {
                dirs_x[i] = 1;
                speeds[i] = f_range_rand(SPRITE_MIN_SPEED, SPRITE_MAX_SPEED);
                sprites[i]->color = lf_col;
            }
            if(rect->y >= 1.0f) {
                dirs_y[i] = -1;
                speeds[i] = f_range_rand(SPRITE_MIN_SPEED, SPRITE_MAX_SPEED);
                sprites[i]->color = up_col;
            }
            if(rect->y - rect->h <= -1.0f) {
                dirs_y[i] = 1;
                speeds[i] = f_range_rand(SPRITE_MIN_SPEED, SPRITE_MAX_SPEED);
                sprites[i]->color = dwn_col;
            }
            
            //printf("x: %f y: %f\n", sprites[i]->rect.x, sprites[i]->rect.y);
        }
        
        frame_count++;
        if(frame_count >= SPRITE_ADD_FRAME) {
            frame_count = 0;
            add_sprite(sprite_count);
        }
        RND_render(&sprites[0], &sprites[0], sprite_count);
        
        fps_frames++;
        if (fps_lasttime < SDL_GetTicks() - FPS_INTERVAL*1000)
        {
            fps_lasttime = SDL_GetTicks();
            fps_current = fps_frames;
            fps_frames = 0;
        }
    }
    
    RND_quit();
    
    for(int i = 0; i < sprite_count; i++) {
        free(sprites[i]);
    }
    
    printf("Last FPS: %u\n", fps_current);
}
