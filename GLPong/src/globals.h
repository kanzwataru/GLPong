//
//  globals.h
//  GLPong
//
//  Created by 神崎航 on 2017/12/31.
//  Copyright © 2017年 神崎航. All rights reserved.
//

#ifndef globals_h
#define globals_h

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define FIXED_UPDATE_FPS 120
#define MAX_FPS 60

#define GATE_W 3
#define GATE_H 5
#define SCORE_W 10
#define SCORE_H 30
#define PADDLE_W 30
#define PADDLE_H 120
#define PADDLE_GAP 320
#define BALL_SIZE 20

#define PADDLE_ACCEL 0.1
#define PADDLE_DECEL 0.01
#define PADDLE_SPEED 10
#define BALL_SPEED 3
#define SMACK_FORCE_MULT 3
#define AI_PREDICT_STEP 12

static const float TOP = 0;
static const float BTM = SCREEN_HEIGHT - PADDLE_H;
static const int SCREEN_HALF = SCREEN_WIDTH / 2;
static const int PLAYER_SCORE_X = 10;
static const int AI_SCORE_X = SCREEN_WIDTH - (2 * SCORE_W);
static const int SCORE_Y = 10;
static const int AI_MAX_REACT = SCREEN_WIDTH * 0.95;
static const int AI_MIN_REACT = SCREEN_WIDTH * 0.5;
static const int AI_PADDLE_UPPER_BOUND = PADDLE_H * 0.2;
static const int AI_PADDLE_LOWER_BOUND = PADDLE_H * 0.7;
static const unsigned int UPDATE_STEP_SIZE = 1000/FIXED_UPDATE_FPS;

#endif /* globals_h */
