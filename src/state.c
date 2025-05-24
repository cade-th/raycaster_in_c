#include "../include/state.h"
#include "../include/world.h"
#include "../include/render.h"
#include "../include/player.h"



void input_update() {
    if (IsKeyDown('W') == 1) {
        state.player->pos.x += cos(TO_RADIANS(state.player->angle)) * state.player->velocity; 
        state.player->pos.y += sin(TO_RADIANS(state.player->angle)) * state.player->velocity; 
    
    }
    if (IsKeyDown('A') == 1) {
        state.player->angle -= 10.0f; 
    }
    if (IsKeyDown('S') == 1) {
        state.player->pos.x -= cos(TO_RADIANS(state.player->angle)) * state.player-> velocity; 
        state.player->pos.y -= sin(TO_RADIANS(state.player->angle)) * state.player-> velocity; 

    }
    if (IsKeyDown('D') == 1) {
       state.player->angle += 10.0f; 
    }

    // Printf's don't work here for some reason?
    if (IsKeyDown('U') == 1) {
        state.renderer->type = MINIMAP; 
    }
    if (IsKeyDown('I') == 1) {
        state.renderer->type = FPS; 
    }

    if (IsKeyPressed('R') == 1) {
        state.renderer->camera.zoom -= 0.05;
    }
    if (IsKeyPressed('T') == 1) {
        state.renderer->camera.zoom += 0.05;
    }

    // renderer->camera.target.x = player->pos.x * world->tile_size; 
    // renderer->camera.target.y = player->pos.y * world->tile_size;


    if (state.player->angle >= 360.0f) {
        state.player->angle -= 360.0f;
    }
    if (state.player->angle < 0.0f) {
        state.player->angle += 360.0f;
    }
}


