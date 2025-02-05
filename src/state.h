#pragma once

#include "player.h"
#include "world.h"
#include "render.h"

typedef struct State {
    Player *player;
    World *world;
    Renderer *renderer;
    Texture2D *atlas;
    int num_rays;
} State;

extern State state;

void input_update();

#define screen_width 1024
#define screen_height 1024

