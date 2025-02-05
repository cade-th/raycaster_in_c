#pragma once

#include "raylib.h"

typedef enum renderer_t {
    MINIMAP,
    FPS,
} renderer_t;

typedef struct Renderer{
    renderer_t type; 
    Camera2D camera; 
} Renderer;


Renderer renderer_new();

void render(Renderer *self);

