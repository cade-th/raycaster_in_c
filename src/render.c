#include "render.h"
#include "ui.h"
#include "player.h"
#include "state.h"

Renderer renderer_new() {
    Camera2D camera = {
        (Vector2) {0.0,0.0},
        (Vector2) {0.0,0.0},
        0.0,
        0.5,
    };

    Renderer renderer = {
        MINIMAP,
        camera,
    };
    return renderer;
}

void render(Renderer *self){

        if (self->type == MINIMAP) {
            render_world(state.world, &self->camera);
            render_player(state.player, state.num_rays);
        }
        if (self->type == FPS) {
         raycast_fov(
            state.player,
            state.player->pos,
            state.player->angle, 
            60.0);
         render_fps(state.player);
        }

        // ui_render_debug();
}


