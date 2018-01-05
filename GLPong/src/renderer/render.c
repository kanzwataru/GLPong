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

#include "render_datatypes.h"
#include "filesystem.h"
#include "png_export.h"

#define SQUARE_VERT_NUM 6
#define VERT_SIZE 7

SDL_Window *window;
SDL_GLContext context;
GLuint flat_shader;

float *square_buf = NULL;
int square_count = 0;

int frame_num = 0;

GLuint _compile_shader(const char *vertsrc, const char *fragsrc) {
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertsrc, NULL);
    glCompileShader(vert);
    
    int success;
    char infolog[512];
    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vert, 512, NULL, infolog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s\n" , infolog);
    }
    
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragsrc, NULL);
    glCompileShader(frag);
    
    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag, 512, NULL, infolog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n %s\n", infolog);
    }
    
    GLuint shader = glCreateProgram();
    glAttachShader(shader, vert);
    glAttachShader(shader, frag);
    glLinkProgram(shader);
    
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader, 512, NULL, infolog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n %s\n", infolog);
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
    
    return shader;
}

void _resize_buf(int count) {
    free(square_buf);
    
    int a = count * (SQUARE_VERT_NUM * VERT_SIZE);
    square_buf = (float *)calloc(a, sizeof(float));
    square_count = count;

    /*
    for(int i = 0; i < count; i++) {
        for(int j = 0; j < SQUARE_VERT_NUM; j++) {
            square_buf[(i * SQUARE_VERT_NUM) + j] = square_primitive[j];
        }
    }
     */
    
    //glBufferData(GL_ARRAY_BUFFER, sizeof(square_primitive) * count, square_buf, GL_STATIC_DRAW);
}

void _set_buf(float *buf, const Sprite *sprite, const int index) {
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

void _verts_from_sprite(float *verts, const Sprite *sprite) {
    verts[0] = sprite->rect.x;
    verts[1] = sprite->rect.y;
    verts[2] = sprite->depth;
    
    verts[3] = sprite->rect.x + sprite->rect.w;
    verts[4] = sprite->rect.y;
    verts[5] = sprite->depth;
    
    verts[6] = sprite->rect.x;
    verts[7] = sprite->rect.y - sprite->rect.h;
    verts[8] = sprite->depth;
    
    verts[9] = verts[3];
    verts[10] = verts[4];
    verts[11] = sprite->depth;
    
    verts[12] = verts[6];
    verts[13] = verts[7];
    verts[14] = sprite->depth;
    
    verts[15] = sprite->rect.x + sprite->rect.w;
    verts[16] = sprite->rect.y - sprite->rect.h;
    verts[17] = sprite->depth;
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
    
    const char *vert_src = load_file("shaders/flat.vert");
    const char *frag_src = load_file("shaders/flat.frag");
    if(!vert_src) {
        printf("ERROR VERT SRC");
        return;
    }
    if(!frag_src) {
        printf("ERROR FRAG SRC");
        return;
    }
    flat_shader = _compile_shader(vert_src, frag_src);
    
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
        _resize_buf(count);
    
    //float verts[SQUARE_VERT_NUM * 3];
    for(int i = 0; i < count; i++) {
        //_verts_from_sprite(&verts[0], next_sprites[i]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);
        //glDrawArrays(GL_TRIANGLES, 0, SQUARE_VERT_NUM * count);
        _set_buf(square_buf, next_sprites[i], i);
    }
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (SQUARE_VERT_NUM * VERT_SIZE * count), square_buf, GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, SQUARE_VERT_NUM * count);

    SDL_GL_SwapWindow(window);
    
    //PNG_screenshot(frame_num);
}
