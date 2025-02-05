#include "world.h"

void render_world_temp(World *world) {  
    for (int i=0; i < world->map_size; i++) {
        for (int j=0; j < world->map_size; j++) {
            Color color = WHITE;
            if (world->data[i][j] == 1) {
                color = BLACK;
            }
            DrawRectangle(i * world->tile_size, j * world->tile_size, world->tile_size , world->tile_size, color);
            }
        }
}


World world_new(int world_size, int tile_size) {
    World world = {
        world_size,
        tile_size,
        .data = {
            {1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,1},
            {1,0,0,1,0,0,0,1},
            {0,0,0,0,0,0,0,1},
            {0,0,0,0,0,1,0,1},
            {1,0,0,0,0,1,0,1},
            {1,0,0,0,0,1,0,1},
            {1,1,1,1,1,1,1,1}
        }
    };

    return world;
}


Vector2 world_to_screen(Vector2 world_pos, Camera2D *camera) {
    Vector2 temp = {
        (world_pos.x - camera->target.x) * camera->zoom + camera->offset.x,
        (world_pos.y - camera->target.y) * camera->zoom + camera->offset.y,
    };
    return temp;
}

Rectangle get_rectangle(Block_t block) {
    Rectangle texture_section;

    switch(block) {
        case STONE:
            texture_section = (Rectangle) {0.0, 32.0, 32.0, 32.0};
            break;
        case GRASS:
            texture_section = (Rectangle) {32.0, 32.0, 32.0, 32.0};
            break;
    }

    return texture_section;
}

// Something is stopping the raycasting here
void render_world(World *self, Camera2D *camera, Texture2D *atlas) {
    
    for (int i=0; i < WORLD_SIZE; i++) {
        for (int j=0; j < WORLD_SIZE; j++) {
            Vector2 tile_world_pos = {
                (float) i * self->tile_size,
                (float) j * self->tile_size,
            };

            // Convert the world position to screen position
            Vector2 tile_screen_pos = world_to_screen(tile_world_pos, camera);

            Rectangle dest_rect = {
                tile_screen_pos.x,
                tile_screen_pos.y,
                self->tile_size * camera->zoom,
                self->tile_size * camera->zoom,
            };

            Rectangle texture_section = get_rectangle(self->data[i][j]);

            DrawTexturePro(
                *atlas,
                texture_section, 
                dest_rect,
                (Vector2){0, 0}, 
                0.0f,            
                WHITE    

            );

        }
    }

    
}


