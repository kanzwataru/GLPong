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
#include "util_funcs.h"

#define SQUARE_VERT_NUM 4
#define SQUARE_INDICES_NUM 6
#define VERT_SIZE 7

static mat4x4 identity;
static SDL_Window *window;
static SDL_GLContext context;
static GLuint flat_shader;

static float *square_buf = NULL;
static GLuint *square_indices_buf = NULL;
static int square_count = 0;

static int frame_num = 0;
static float reverse_aspect_ratio;

static const float square_prim[] = {
    -1.0f, 1.0f,
    1.0f, 1.0f,
    -1.0f, -1.0f,
    1.0f, -1.0f,
};

static const GLuint square_prim_indices[] = {
    0, 1, 2,
    1, 2, 3
};

static void _resize_buf(float **buf, GLuint **ebo_buf, int count) {
    free(*buf);
    free(*ebo_buf);
    
    int vbo_len = count * (SQUARE_VERT_NUM * VERT_SIZE);
    int ebo_len = count * SQUARE_INDICES_NUM;
    
    *buf = calloc(vbo_len, sizeof(float));
    GLuint *ebo_p = calloc(ebo_len, sizeof(GLuint));
    *ebo_buf = ebo_p;
    
    /* The EBO is always the same for each square,
       so assign them all right now*/
    int object_i = 0;
    for(int i = 0; i < ebo_len; i += 6) {
        for(int j = 0; j < SQUARE_INDICES_NUM; j++) {
            ebo_p[j + i] = square_prim_indices[j] + (object_i * SQUARE_VERT_NUM);
            //printf("ebo [%d]: %u \n", j+i, ebo_p[j + i]);
        }
        object_i++;
    }
    
    square_count = count;
}

static void _set_buf(float *buf, const Sprite *sprite, const int offset) {
    float halfw = sprite->rect.w * 0.5f;
    float halfh = sprite->rect.h * 0.5f;
    float depth = (float)sprite->depth * 0.01f;
    
    mat4x4 model;
    mat4x4_rotate_Z(model, identity, deg_to_rad(sprite->rotation));

    vec4 temp;
    vec4 result = {0.0f, 0.0f, 0.0f, 0.0f};
    int i, prim_i = 0;
    for(i = 0; i < (SQUARE_VERT_NUM * VERT_SIZE); i += VERT_SIZE) {
        temp[0] = square_prim[0 + prim_i] * halfw;
        temp[1] = square_prim[1 + prim_i] * halfh;
        temp[2] = depth;
        
        prim_i += 2;
        
        mat4x4_mul_vec4(result, model, temp);
        buf[0 + i + offset] = (result[0] + sprite->rect.x) * reverse_aspect_ratio;
        buf[1 + i + offset] = result[1] + sprite->rect.y;
        buf[2 + i + offset] = result[2];
        
        //printf("(%f, %f, %f) \n", buf[0 + i + offset], buf[1 + i + offset], buf[2 + i + offset]);
        
        buf[3 + i + offset] = sprite->color.r;
        buf[4 + i + offset] = sprite->color.g;
        buf[5 + i + offset] = sprite->color.b;
        buf[6 + i + offset] = sprite->color.a;
    }
}

RenderInfo RND_init(const char *title, int width, int height) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    window = SDL_CreateWindow("OpenGL", 100, 100, width, height, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);
    
    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    flat_shader = add_shader("shaders/flat.vert", "shaders/flat.frag");
    reverse_aspect_ratio = 1 / ((float)width / (float)height);
    
    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glUseProgram(flat_shader);
    
    mat4x4_identity(identity);

    RenderInfo rinfo;
    rinfo.aspect_ratio = (float)width / (float)height;
    rinfo.x_min = -rinfo.aspect_ratio;
    rinfo.x_max = rinfo.aspect_ratio;
    rinfo.y_min = -1.0f;
    rinfo.y_max = 1.0f;
    
    return rinfo;
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
        _resize_buf(&square_buf, &square_indices_buf, count);

    int offset;
    for(int i = 0; i < count; i++) {
        offset = i * (SQUARE_VERT_NUM * VERT_SIZE);
        _set_buf(square_buf, next_sprites[i], offset);
    }
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (count * SQUARE_VERT_NUM * VERT_SIZE), square_buf, GL_STREAM_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * (count * SQUARE_INDICES_NUM), square_indices_buf, GL_STATIC_DRAW);
    //glDrawArrays(GL_TRIANGLES, 0, SQUARE_VERT_NUM * count);
    glDrawElements(GL_TRIANGLES, SQUARE_INDICES_NUM * count, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);
    
    //printf("count: %d square_buf: %zu\n", count, sizeof(float) * (SQUARE_VERT_NUM * VERT_SIZE * count));

    //PNG_screenshot(frame_num);
}
