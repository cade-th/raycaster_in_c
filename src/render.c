#include "render.h"
#include "player.h"

Renderer renderer_new() {
    Camera2D camera = {
        (Vector2) {0.0,0.0},
        (Vector2) {0.0,0.0},
        0.0,
        1.0,
    };

    Renderer renderer = {
        FPS,
        camera,
    };
    return renderer;
}

void render(Renderer *self, Player *player, int num_rays, World *world, Texture2D *atlas) {
        raycast_fov(player, player->pos, player->angle, 60.0, num_rays,  world);

        if (self->type == MINIMAP) {
            render_world(world, &self->camera, atlas);
            render_player(player, num_rays);
        }
        if (self->type == FPS) {
            render_fps(player, num_rays, world, atlas);
        }
}


