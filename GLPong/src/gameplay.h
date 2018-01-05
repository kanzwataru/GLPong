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

typedef struct _Paddle {
    Rect rect;
    float speed;
    int direction;
    int score;
} Paddle;

typedef struct _ball {
    Rect rect;
    float dir_x;
    float dir_y;
    bool start;
} Ball;

typedef struct _World {
    Paddle player;
    Paddle ai;
    Ball ball;
    bool game_paused;
} World;

#endif /* gameplay_h */
