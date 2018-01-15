//
//  renderer.h
//  GLPong
//
//  Created by 神崎航 on 2017/12/31.
//  Copyright © 2017年 神崎航. All rights reserved.
//

/*
    Render flow
 (per-frame)
 
 main loop <- update world
           -> render frame
 
 render frame <- render each mesh
                 - transform verts on cpu
                 - calculate smear
                 - upload VBO
 */

#ifndef render_h
#define render_h

#include <stdio.h>
#include "render_datatypes.h"

RenderInfo RND_init(const char *title, int width, int height);
void RND_quit(void);

void RND_beginframe(const Color *bg_color);
void RND_render(Sprite **prev, Sprite **next, int count);
void RND_endframe(void);

#endif /* renderer_h */
