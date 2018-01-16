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

#define GATE_W 0.005f
#define GATE_H 0.01f
#define SCORE_W 10
#define SCORE_H 30
#define PADDLE_W 0.08f
#define PADDLE_H 0.4f
#define PADDLE_GAP 1.1f
#define BALL_SIZE 0.08f

#define PADDLE_ACCEL 0.002f
#define PADDLE_DECEL 0.06f
#define PADDLE_SPEED 0.03f
#define BALL_SPEED 0.01f
#define SMACK_FORCE_MULT 50
#define AI_PREDICT_STEP 12
#define AI_STOP_MIN 0.1
#define AI_STOP_MAX 0.8

static const float TOP = 1.0f - (PADDLE_H / 2);
static const float BTM = -1.0f + (PADDLE_H / 2);
static const int SCREEN_HALF = 0.0f;
static const int PLAYER_SCORE_X = 10;
static const int AI_SCORE_X = SCREEN_WIDTH - (2 * SCORE_W);
static const int SCORE_Y = 10;
static const float AI_MAX_REACT = PADDLE_GAP - 0.1f;
static const float AI_MIN_REACT = SCREEN_HALF;
static const int AI_PADDLE_UPPER_BOUND = PADDLE_H * 0.2;
static const int AI_PADDLE_LOWER_BOUND = PADDLE_H * 0.7;
static const unsigned int UPDATE_STEP_SIZE = 1000/FIXED_UPDATE_FPS;

#endif /* globals_h */
