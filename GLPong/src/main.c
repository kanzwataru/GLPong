//
//  main.c
//  GLPong
//
//  Created by 神崎航 on 2017/12/30.
//  Copyright © 2017年 神崎航. All rights reserved.
//

#include "cube_bounce.h"
#include "render.h"

#include <SDL2/SDL.h>

int main(int argc, const char * argv[]) {
    cube_bounce_run();
    /*
    RND_init("GLPong", 800, 600);

    while(1) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                return 1;
            }
        }

        Rect rect = {0.5f, 0.5f, 0.5f, 0.5f};
        Color col = {0.2f, 0.5f, 0.3f, 1.0f};

        Sprite sprite;
        sprite.rect = rect;
        sprite.color = col;
        sprite.rotation = 0.0f;
        sprite.depth = 0.0f;

        Sprite *sprite_p = &sprite;
        RND_render(&sprite_p, &sprite_p, 1);
    } */
}
