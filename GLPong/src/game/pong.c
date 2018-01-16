#include "pong.h"

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "globals.h"
#include "render.h"
#include "gameplay.h"

static RenderInfo rinfo;
static World prev_world;
static World world;

#define GATE_TILES_NUM 90
#define SCORE_TILES_NUM 20
static Sprite gate_tiles[GATE_TILES_NUM];
static Sprite pause_sprite;

static Sprite player_score_tiles[SCORE_TILES_NUM];
static Sprite ai_score_tiles[SCORE_TILES_NUM];
static Sprite score_tile;

static void init_score_tiles(void) {
    score_tile.color = RND_GREY;
    score_tile.depth = 2;
    score_tile.rect.w = SCORE_W;
    score_tile.rect.h = SCORE_H;
    score_tile.rect.y = SCORE_Y;
    score_tile.rotation = 0;

    for(int i = 0; i < SCORE_TILES_NUM; ++i) {
        player_score_tiles[i] = score_tile;
        player_score_tiles[i].rect.x = PLAYER_SCORE_X + (i * (SCORE_W * 2));

        ai_score_tiles[i] = score_tile;
        ai_score_tiles[i].rect.x = AI_SCORE_X - (i * (SCORE_W * 2));
    }
}

static void init_gate_tiles(void) {
    for(int i = 0; i < GATE_TILES_NUM; ++i) {
        gate_tiles[i].rect.x = 0.0f;
        gate_tiles[i].rect.y = i * (GATE_H * 4) - 1;
        gate_tiles[i].rect.w = GATE_W;
        gate_tiles[i].rect.h = GATE_H;
        gate_tiles[i].rotation = 0;
        gate_tiles[i].depth = 0;
        gate_tiles[i].color = RND_WHITE;
    }
}

static void init_pause_sprite(void) {
    Color col = {0.8f, 0.8f, 0.8f, 0.1f};
    pause_sprite.color = col;
    pause_sprite.depth = 2;
    pause_sprite.rect.x = 0.0f;
    pause_sprite.rect.y = 0.0f;
    pause_sprite.rect.w = rinfo.x_max * 2;
    pause_sprite.rect.h = 2.0f;
    pause_sprite.rotation = 0;
}

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
    Sprite *sprites[GATE_TILES_NUM + (SCORE_TILES_NUM * 2) + 4] = {NULL};

    sprites[0] = &world.player.sprite;
    sprites[1] = &world.ai.sprite;
    sprites[2] = &world.ball.sprite;
    int count = 3;

    for(int i = 0; i < GATE_TILES_NUM; ++i) {
        sprites[i + 3] = &gate_tiles[i];
        ++count;
    }

    if(world.paused) {
        sprites[count] = &pause_sprite;
        ++count;
    }

    for(int i = 0; i < world.player.score && i < SCORE_TILES_NUM; ++i) {
        sprites[count] = &player_score_tiles[i];
        ++count;
    }

    for(int i = 0; i < world.ai.score && i < SCORE_TILES_NUM; ++i) {
        sprites[count] = &ai_score_tiles[i];
        ++count;
    }

    RND_beginframe(&RND_BLACK);
    RND_render(&sprites[0], &sprites[0], count);
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
    init_score_tiles();
    init_pause_sprite();
    init_gate_tiles();

    // run game loop
    game_loop();
}
