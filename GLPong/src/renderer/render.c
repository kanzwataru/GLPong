//
//  renderer.c
//  GLPong
//
//  Created by 神崎航 on 2017/12/31.
//  Copyright © 2017年 神崎航. All rights reserved.
//

#include "render.h"

#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>

#include "glad/glad.h"
#include "linmath.h"

#include "render_utils.h"
#include "render_datatypes.h"
#include "png_export.h"
#include "util_funcs.h"

#define SQUARE_VERT_NUM 4
#define SQUARE_INDICES_NUM 6
#define VERT_SIZE 7
#define MAX_BUFFERS 2000

typedef struct {
    float *vertex_buf;
    GLuint *indices_buf;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    int count;
} SquareBuf;

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

static mat4x4 identity;
static SDL_Window *window;
static SDL_GLContext context;
static GLuint flat_shader;

static SquareBuf *square_bufs[MAX_BUFFERS] = {NULL};
static int square_buf_count = 0;

static int frame_num = 0;
static float reverse_aspect_ratio;

static void square_buf_resize(SquareBuf *square_buf, int count) {
    free(square_buf->vertex_buf);
    free(square_buf->indices_buf);
    
    int vbo_len = count * (SQUARE_VERT_NUM * VERT_SIZE);
    int ebo_len = count * SQUARE_INDICES_NUM;
    
    square_buf->vertex_buf = calloc(vbo_len, sizeof(float));
    square_buf->indices_buf = calloc(ebo_len, sizeof(GLuint));
    
    /* The EBO is always the same for each square,
       so assign them all right now*/
    int object_i = 0;
    for(int i = 0; i < ebo_len; i += 6) {
        for(int j = 0; j < SQUARE_INDICES_NUM; j++) {
            square_buf->indices_buf[j + i] = square_prim_indices[j] + (object_i * SQUARE_VERT_NUM);
            //printf("ebo [%d]: %u \n", j+i, batch->indices_buf[j + i]);
        }
        object_i++;
    }
    
    square_buf->count = count;
}

static void square_buf_set(float *buf, const Sprite *sprite, const int offset) {
    float halfw = sprite->rect.w * 0.5f;
    float halfh = sprite->rect.h * 0.5f;
    float depth = -(float)sprite->depth * 0.01f;
    
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

static void buf_init(GLuint *vao, GLuint *vbo, GLuint *ebo, GLuint shader) {
    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    glGenBuffers(1, ebo);

    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);

    /* position */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERT_SIZE * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    /* colour */
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERT_SIZE * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shader);
    glBindVertexArray(0);
}

RenderInfo RND_init(const char *title, int width, int height) {
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Failed to initialize SDL\n");
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    window = SDL_CreateWindow(title, 100, 100, width, height, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);
    if(!context) {
        printf("Failed to make GL context\n");
    }
    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("Failed to load glad\n");
    }
    
    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    flat_shader = add_shader("shaders/flat.vert", "shaders/flat.frag");
    reverse_aspect_ratio = 1 / ((float)width / (float)height);
    
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
    for(int i = 0; i < square_buf_count; ++i) {
        free(square_bufs[i]);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int RND_create_square_buffer() {
    //static SquareBuf squares = {NULL, NULL, 0};
    if(square_buf_count + 1 >= MAX_BUFFERS) {
        printf("ERROR: Reached maximum number of square buffers\n");
        return -1;
    }

    int index = square_buf_count;

    square_bufs[index] = calloc(1, sizeof(SquareBuf));
    ++square_buf_count;

    buf_init(&square_bufs[index]->vao, &square_bufs[index]->vbo, &square_bufs[index]->ebo, flat_shader);

    printf("vao: %u vbo: %u ebo: %u\n", square_bufs[index]->vao, square_bufs[index]->vbo, square_bufs[index]->ebo);
    return index;
}

void RND_beginframe(const Color *bg_color) {
    frame_num++;

    glClearColor(bg_color->r, bg_color->g, bg_color->b, bg_color->a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RND_render(int id, Sprite **prev_sprites, Sprite **next_sprites, int count) {
    if(id > square_buf_count || id < 0) {
        printf("ERROR: Invalid square buffer [%d]", id);
    }
    if(count != square_bufs[id]->count)
        square_buf_resize(square_bufs[id], count);

    int offset;
    for(int i = 0; i < count; i++) {
        if(!next_sprites[i] || !prev_sprites[i]) {
            printf("Sprite id [%d] is NULL \n", i);
            continue;
        }

        offset = i * (SQUARE_VERT_NUM * VERT_SIZE);
        square_buf_set(square_bufs[id]->vertex_buf, next_sprites[i], offset);
    }

    glBindVertexArray(square_bufs[id]->vao);
    glBindBuffer(GL_ARRAY_BUFFER, square_bufs[id]->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, square_bufs[id]->ebo);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (count * SQUARE_VERT_NUM * VERT_SIZE), square_bufs[id]->vertex_buf, GL_STREAM_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * (count * SQUARE_INDICES_NUM), square_bufs[id]->indices_buf, GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, SQUARE_INDICES_NUM * count, GL_UNSIGNED_INT, 0);
}

void RND_endframe(void) {
    SDL_GL_SwapWindow(window);

    //printf("count: %d square_buf: %zu\n", count, sizeof(float) * (SQUARE_VERT_NUM * VERT_SIZE * count));

    //PNG_screenshot(frame_num);
}
