#pragma once

#include "player.h"
#include "world.h"
#include "render.h"
#include "ui.h"

typedef struct State {
    Player *player;
    World *world;
    Renderer *renderer;
    UI *ui;
    Texture2D *atlas;
    int num_rays;

} State;

extern State state;

void input_update();

#define screen_width 1024
#define screen_height 1024

