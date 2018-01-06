//
//  util_funcs.c
//  GLPong
//
//  Created by 神崎航 on 2017/12/31.
//  Copyright © 2017年 神崎航. All rights reserved.
//
#include <stdlib.h>

#include "util_funcs.h"
#include "globals.h"

float f_clamp(float value, float min, float max) {
    if(value > max)
        return max;
    if(value < min)
        return min;
    
    return value;
}

int i_clamp(int value, int min, int max) {
    if(value > max)
        return max;
    if(value < min)
        return min;
    
    return value;
}

float remap(float value, float oldmin, float oldmax, float newmin, float newmax) {
    float real_range = oldmax - oldmin;
    float new_range = newmax - newmin;
    return (((value - oldmin) * new_range) / real_range) + newmin;
}

int i_range_rand(int min, int max) {
    return rand()%(max-min)+min;
}

float f_range_rand(float min, float max) {
    return 0.0001 * (float)i_range_rand((int)(min * 10000), (int)(max * 10000));
}
