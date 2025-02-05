#pragma once

#include "state.h"
#include "raylib.h"
#include "world.h"

struct Player;

typedef enum renderer_t {
    MINIMAP,
    FPS,
} renderer_t;

typedef struct Renderer{
    renderer_t type; 
    Camera2D camera; 
} Renderer;


Renderer renderer_new();

void render(Renderer *self, struct Player *player, int num_rays, World *world, Texture2D *atlas);

