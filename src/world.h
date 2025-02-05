#pragma once
#include "state.h"
#include "world.h"
#include "raylib.h"

#define WORLD_SIZE 64
typedef enum block_t {
    GRASS,
    STONE, 
} Block_t;

typedef struct World {
    int map_size;
    float tile_size;
    Block_t data[WORLD_SIZE][WORLD_SIZE];
} World;

void render_world_temp(World *world); 
Vector2 world_to_screen(Vector2 world_pos, Camera2D *camera);
Rectangle get_rectangle(Block_t block);
void render_world(World *self, Camera2D *camera, Texture2D *atlas);
World world_new(int world_size, int tile_size); 
