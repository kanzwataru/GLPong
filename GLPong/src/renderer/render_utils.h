//
//  render_utils.h
//  GLPong
//
//  Created by 神崎航 on 2018/01/05.
//  Copyright © 2018年 神崎航. All rights reserved.
//

#ifndef render_utils_h
#define render_utils_h

#include <stdio.h>
#include <OpenGL/gl3.h>
#include <math.h>
#include "linmath.h"

static inline double deg_to_rad(double radians) {
    return radians * (180.0 / M_PI);
}

static inline void mat4x4_identity_one(mat4x4 M) {
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            M[i][j] = 1.0f;
}

GLuint add_shader(const char *vert_filepath, const char *frag_filepath);

#endif /* render_utils_h */
