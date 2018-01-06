//
//  render_utils.c
//  GLPong
//
//  Created by 神崎航 on 2018/01/05.
//  Copyright © 2018年 神崎航. All rights reserved.
//

#include "render_utils.h"

#include <stdlib.h>
#include "filesystem.h"

static GLuint _compile_shader(const char *vertsrc, const char *fragsrc) {
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

GLuint add_shader(const char *vert_filepath, const char *frag_filepath) {
    char *vert_src = load_file(vert_filepath);
    char *frag_src = load_file(frag_filepath);
    if(!vert_src) {
        printf("ERROR LOADING VERT SRC FILE: %s\n", vert_filepath);
        return 0;
    }
    if(!frag_src) {
        printf("ERROR LOADING FRAG SRC FILE: %s\n", frag_filepath);
        return 0;
    }
    
    return _compile_shader(vert_src, frag_src);
    free(vert_src);
    free(frag_src);
}
