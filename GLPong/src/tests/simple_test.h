#ifndef simple_test_h
#define simple_test_h

#include <SDL2/SDL.h>
#include "render.h"

void simple_test(void) {
    RND_init("GLPong", 800, 600);
    int buffer_id = RND_create_square_buffer();

    while(1) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                return;
            }
        }

        Rect rect = {0.5f, 0.5f, 0.5f, 0.5f};
        Color col = {0.6f, 0.7f, 0.6f, 1.0f};
        Color bg_col = {0.3f, 0.6f, 0.3f, 1.0f};

        Sprite sprite;
        sprite.rect = rect;
        sprite.color = col;
        sprite.rotation = 0.0f;
        sprite.depth = 0.0f;

        Sprite *sprite_p = &sprite;
        RND_beginframe(&bg_col);
        RND_render(buffer_id, &sprite_p, &sprite_p, 1);
        RND_endframe();
    }
}

#endif /* simple_test_h */
