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

typedef struct _color {
    float r;
    float g;
    float b;
    float a;
} Color;

typedef struct _point {
    float x;
    float y;
} Point;

typedef struct _rect {
    float x;
    float y;
    float w;
    float h;
} Rect;

typedef struct _sprite {
    float rotation;
    int depth;
    Color color;
    Rect rect;
} Sprite;

#endif /* renderer_datatypes_h */
