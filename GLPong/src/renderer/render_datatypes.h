//
//  renderer_datatypes.h
//  GLPong
//
//  Created by 神崎航 on 2017/12/31.
//  Copyright © 2017年 神崎航. All rights reserved.
//

#ifndef render_datatypes_h
#define render_datatypes_h

#include <stdint.h>

typedef struct {
    float r;
    float g;
    float b;
    float a;
} Color;

typedef struct {
    float x;
    float y;
} Point;

typedef struct {
    float x;
    float y;
    float w;
    float h;
} Rect;

typedef struct {
    Color color;
    Rect rect;
    float rotation;
    int depth;
} Sprite;

typedef struct {
    float aspect_ratio;
    float x_min;
    float x_max;
    float y_min;
    float y_max;
} RenderInfo;

static const Color RND_WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
static const Color RND_BLACK = {0.0f, 0.0f, 0.0f, 1.0f};

#endif /* renderer_datatypes_h */
