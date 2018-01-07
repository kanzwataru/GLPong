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

#endif /* renderer_datatypes_h */
