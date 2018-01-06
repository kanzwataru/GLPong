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

static void _resize_buf(float **square_buf, int count) {
    free(*square_buf);
    
    int a = count * (SQUARE_VERT_NUM * VERT_SIZE);
    *square_buf = calloc(a, sizeof(float));
    square_count = count;
}

static void _set_buf(float *buf, const Sprite *sprite, const int index) {
    int offset = index * (SQUARE_VERT_NUM * VERT_SIZE);

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
    
    /* Put colours */
    for(int i = 3; i < (SQUARE_VERT_NUM * VERT_SIZE); i += 7) {
        buf[0 + i + offset] = sprite->color.r;
        buf[1 + i + offset] = sprite->color.g;
        buf[2 + i + offset] = sprite->color.b;
        buf[3 + i + offset] = sprite->color.a;
    }
}

static void _set_rot(float *buf, const Sprite *sprite, const int index) {

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

    for(int i = 0; i < count; i++) {
        _set_buf(square_buf, next_sprites[i], i);
        _set_rot(square_buf, next_sprites[i], i);
    }
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (SQUARE_VERT_NUM * VERT_SIZE * count), square_buf, GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, SQUARE_VERT_NUM * count);

    SDL_GL_SwapWindow(window);
    
    //PNG_screenshot(frame_num);
}
