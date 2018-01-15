//
//  gameplay.h
//  GLPong
//
//  Created by 神崎航 on 2017/12/31.
//  Copyright © 2017年 神崎航. All rights reserved.
//

#ifndef gameplay_h
#define gameplay_h

#include <stdio.h>
#include <stdbool.h>
#include "render_datatypes.h"

typedef struct {
    Sprite sprite;
    float speed;
    int direction;
    int score;
} Paddle;

typedef struct {
    Sprite sprite;
    float vel_x;
    float vel_y;
    Rect *parent;
} Ball;

typedef struct {
    Paddle player;
    Paddle ai;
    Ball ball;
    bool paused;
} World;

void GMP_down_event(void);
void GMP_stop_event(void);
void GMP_up_event(void);
void GMP_serve_event(void);
void GMP_pause_event(void);

void GMP_init(const RenderInfo renderinfo);
World GMP_update(void);

#endif /* gameplay_h */
