//
//  util_funcs.h
//  GLPong
//
//  Created by 神崎航 on 2017/12/31.
//  Copyright © 2017年 神崎航. All rights reserved.
//

#ifndef util_funcs_h
#define util_funcs_h

#include <stdio.h>

float f_clamp(float value, float min, float max);
int i_clamp(int value, int min, int max);
float remap(float value, float oldmin, float oldmax, float newmin, float newmax);

int i_range_rand(int min, int max);
float f_range_rand(float min, float max);

#endif /* util_funcs_h */
