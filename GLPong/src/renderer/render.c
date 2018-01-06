//
//  renderer.c
//  GLPong
//
//  Created by 神崎航 on 2017/12/31.
//  Copyright © 2017年 神崎航. All rights reserved.
//

#include "render.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl3.h>
#include "linmath.h"

#include "render_utils.h"
#include "render_datatypes.h"
#include "png_export.h"

#define SQUARE_VERT_NUM 6
#define VERT_SIZE 7

static SDL_Window *window;
static SDL_GLContext context;
static GLuint flat_shader;

static float *square_buf = NULL;
static int square_count = 0;

static int frame_num = 0;
static float reverse_aspect_ratio;

static void _resize_buf(float **square_buf, int count) {
    free(*square_buf);
    
    int a = count * (SQUARE_VERT_NUM * VERT_SIZE);
    *square_buf = calloc(a, sizeof(float));
    square_count = count;
}

static void _set_cols(float *buf, const Sprite *sprite, const int offset) {
    for(int i = 3; i < (SQUARE_VERT_NUM * VERT_SIZE); i += 7) {
        buf[0 + i + offset] = sprite->color.r;
        buf[1 + i + offset] = sprite->color.g;
        buf[2 + i + offset] = sprite->color.b;
        buf[3 + i + offset] = sprite->color.a;
    }
}

static void _set_buf(float *buf, const Sprite *sprite, const int offset) {
    buf[0 + offset] = sprite->rect.x;
    buf[1 + offset] = sprite->rect.y;
    buf[2 + offset] = sprite->depth;
    
    buf[7 + offset] = sprite->rect.x + sprite->rect.w;
    buf[8 + offset] = sprite->rect.y;
    buf[9 + offset] = sprite->depth;
    
    buf[14 + offset] = sprite->rect.x;
    buf[15 + offset] = sprite->rect.y - sprite->rect.h;
    buf[16 + offset] = sprite->depth;
    
    buf[21 + offset] = buf[7 + offset];
    buf[22 + offset] = buf[8 + offset];
    buf[23 + offset] = sprite->depth;
    
    buf[28 + offset] = buf[14 + offset];
    buf[29 + offset] = buf[15 + offset];
    buf[30 + offset] = sprite->depth;
    
    buf[35 + offset] = sprite->rect.x + sprite->rect.w;
    buf[36 + offset] = sprite->rect.y - sprite->rect.h;
    buf[37 + offset] = sprite->depth;
    
    _set_cols(buf, sprite, offset);
}

static void _set_buf_nopos(float *buf, const Sprite *sprite, const int offset) {
    float halfw = sprite->rect.w * 0.5f;
    float halfh = sprite->rect.h * 0.5f;
    
    buf[0 + offset] = -halfw;
    buf[1 + offset] = halfh;
    buf[2 + offset] = sprite->depth;
    
    buf[7 + offset] = halfw;
    buf[8 + offset] = halfh;
    buf[9 + offset] = sprite->depth;
    
    buf[14 + offset] = -halfw;
    buf[15 + offset] = -halfh;
    buf[16 + offset] = sprite->depth;
    
    buf[21 + offset] = buf[7 + offset];
    buf[22 + offset] = buf[8 + offset];
    buf[23 + offset] = sprite->depth;
    
    buf[28 + offset] = buf[14 + offset];
    buf[29 + offset] = buf[15 + offset];
    buf[30 + offset] = sprite->depth;
    
    buf[35 + offset] = halfw;
    buf[36 + offset] = -halfh;
    buf[37 + offset] = sprite->depth;
    
    _set_cols(buf, sprite, offset);
}

static void _set_rot_pos(float *buf, const Sprite *sprite, const int offset) {
    mat4x4 model;
    mat4x4_identity(model);
    
    mat4x4 q;
    mat4x4_identity(q);
    
    mat4x4_rotate_Z(model, q, deg_to_rad(sprite->rotation));
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            //printf("%f \n", model[i][j]);
        }
    }
    
    for(int i = 0; i < (SQUARE_VERT_NUM * VERT_SIZE); i += 7) {
        vec4 temp;
        temp[0] = buf[0 + i + offset];
        temp[1] = buf[1 + i + offset];
        temp[2] = buf[2 + i + offset];
        
        vec4 result = {0.0f, 0.0f, 0.0f, 0.0f};
        
        mat4x4_mul_vec4(result, model, temp);
        buf[0 + i + offset] = (result[0] * reverse_aspect_ratio) + sprite->rect.x;
        buf[1 + i + offset] = result[1] + sprite->rect.y;
        buf[2 + i + offset] = result[2];
        
        //printf("(%f, %f, %f) \n", buf[0 + i + offset], buf[1 + i + offset], buf[2 + i + offset]);
    }
}

void RND_init(const char *title, int width, int height) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    window = SDL_CreateWindow("OpenGL", 100, 100, width, height, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);
    
    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    
    flat_shader = add_shader("shaders/flat.vert", "shaders/flat.frag");
    reverse_aspect_ratio = (float)height / (float)width;
    
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glUseProgram(flat_shader);
}

void RND_quit(void) {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void RND_render(Sprite **prev_sprites, Sprite **next_sprites, int count) {
    frame_num++;
    
    glClearColor(0.6f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if(count != square_count)
        _resize_buf(&square_buf, count);

    int offset;
    for(int i = 0; i < count; i++) {
        offset = i * (SQUARE_VERT_NUM * VERT_SIZE);
        //_set_buf(square_buf, next_sprites[i], offset);
        _set_buf_nopos(square_buf, next_sprites[i], offset);
        _set_rot_pos(square_buf, next_sprites[i], offset);
    }
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (SQUARE_VERT_NUM * VERT_SIZE * count), square_buf, GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, SQUARE_VERT_NUM * count);

    SDL_GL_SwapWindow(window);
    
    //PNG_screenshot(frame_num);
}
