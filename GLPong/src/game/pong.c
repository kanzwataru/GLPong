#include "pong.h"

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "globals.h"
#include "render.h"
#include "gameplay.h"

static RenderInfo rinfo;
static World prev_world;
static World world;

static bool handle_input(void) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            return false;
        }
        if(event.type == SDL_KEYDOWN) {
            if(event.key.keysym.sym == SDLK_UP)
                GMP_up_event();
            else if(event.key.keysym.sym == SDLK_DOWN)
                GMP_down_event();
            else if(event.key.keysym.sym == SDLK_SPACE)
                GMP_serve_event();
            else if(event.key.keysym.sym == SDLK_ESCAPE)
                return false;
            else if(event.key.keysym.sym == SDLK_p)
                GMP_pause_event();
        }
        else if(event.type == SDL_KEYUP) {
            if(event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN)
                GMP_stop_event();
        }
    }

    return true;
}

static void render(void) {
    Sprite *sprites[3];
    sprites[0] = &world.player.sprite;
    sprites[1] = &world.ai.sprite;
    sprites[2] = &world.ball.sprite;

    RND_beginframe(&RND_BLACK);
    RND_render(&sprites[0], &sprites[0], 3);
    RND_endframe();
}

static void game_loop(void) {
    unsigned int last_time = 0;
    unsigned int accumulator = 0;
    unsigned int current_time;
    unsigned int delta_time;

    while(handle_input()) {
        current_time = SDL_GetTicks();
        delta_time = current_time - last_time;
        last_time = current_time;

        accumulator += delta_time;
        while(accumulator > UPDATE_STEP_SIZE) {
            prev_world = world;
            world = GMP_update();
            accumulator -= UPDATE_STEP_SIZE;
        }

        render();
    }
}

void run_pong(void) {
    // initialize renderer
    rinfo = RND_init("GLPong", SCREEN_WIDTH, SCREEN_HEIGHT);

    // initialize game
    GMP_init(rinfo);

    // run game loop
    game_loop();
}
