//
//  main.c
//  GLPong
//
//  Created by 神崎航 on 2017/12/30.
//  Copyright © 2017年 神崎航. All rights reserved.
//
#include "simple_test.h"
#include "cube_bounce.h"
#include "pong.h"

int main(int argc, const char **argv) {

    if(argc > 1) {
        if(0 == strcmp(argv[1], "cubetest"))
            cube_bounce_run();
        else if(0 == strcmp(argv[1], "simpletest"))
            simple_test();
        else
            run_pong();
    }
    else {
        run_pong();
    }
}
