//
//  png_export.c
//  GLPong
//
//  Created by 神崎航 on 2018/01/02.
//  Copyright © 2018年 神崎航. All rights reserved.
//

#include "png_export.h"

#include <stdlib.h>
#include <OpenGL/gl3.h>

#include "globals.h"
#include "lodepng.h"

char _path[1000];
unsigned char *_image = NULL;

void _export_png(const char *filepath, const unsigned char *image) {
    unsigned int error = lodepng_encode32_file(filepath, image, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
}

void PNG_screenshot(int frame_num) {
    if(!_image)
        _image = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4);
    sprintf(_path, "/Users/wataru/Pictures/glpong_output/out_%d.png", frame_num);
    
    glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, _image);
    _export_png(_path, _image);
}
