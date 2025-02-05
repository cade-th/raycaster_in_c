#include "player.h"
#include "render.h"
#include "world.h"

Player player_new() {
    Player player = {
        {
            200.0f,
            200.0f,
        },
        20.0f,
        0.0f,
    };
    return player;
}

void render_player(Player *self, float num_rays) {
    DrawCircle(self->pos.x, self->pos.y, 5.0, BLUE);

    Vector2 start_pos = {self->pos.x, self->pos.y};
    Vector2 end_pos = {
        self->pos.x + cos(TO_RADIANS(self->angle)) * 50.0f,
        self->pos.y + sin(TO_RADIANS(self->angle)) * 50.0f,
    };

    DrawLineEx(start_pos, end_pos, 5.0, RED);

    for (int i = 0; i < num_rays; i++) {
        Vector2 ray_pos = self->positions[i].hit_point;
        // Test
        Vector2 start_pos = {self->pos.x, self->pos.y};
        Vector2 end_pos = {
           ray_pos.x, 
           ray_pos.y, 
        };
        DrawLineEx(start_pos, end_pos, 5.0, RED);
    }
}

void raycast_fov(Player *self, Vector2 pos, float angle, float fov, int num_rays, World *world) {

    float start_angle = angle - (fov / 2.0f);
    float angle_step = fov / ((float)num_rays- 1.0);

    for (int i = 0; i < num_rays; i++) {
        float angle_new = start_angle + (i * angle_step);
        Tuple_Return point = raycast_dda(pos, angle_new, world);

        // Fix fisheye effect
        float ca = angle - angle_new;
        point.distance *= cos(TO_RADIANS(ca));

        self->positions[i].distance = point.distance;
        self->positions[i].hit_point = point.hit_point;
        self->positions[i].type = point.type;
    }
}

void render_fps(Player *self, int num_rays, World *world, Texture2D *atlas) {
    float column_width = ceil((float)screen_width / num_rays);

    for (int i = 0; i < num_rays; i++) {
        float wall_height = (screen_height * world->tile_size) / self->positions[i].distance;
        float y = (screen_height - wall_height) / 2.0;
        float x = i * column_width;
        float height = wall_height;

        Color ceiling_color = (Color){ 135, 206, 250, 255 };
        Color floor_color = (Color){ 139, 69, 19, 255 };

        int texture_width = 32;
        float hit_offset;

        if (self->positions[i].type == VERTICAL) {
            hit_offset = fmod(self->positions[i].hit_point.y, world->tile_size);
        } else {
            hit_offset = fmod(self->positions[i].hit_point.x, world->tile_size);
        }

        int texture_column = (int)((hit_offset / world->tile_size) * texture_width);

        Rectangle source_rect = {
            texture_column,
            0,    
            1,   
            32    
        };

        Rectangle dest_rect = {
            x, y, column_width, height
        };

        // Ceiling
        DrawRectangle(x, 0, column_width, y, ceiling_color);

        // Wall
        // TODO: Fix the max distance thing here
            DrawTexturePro(
                *atlas,          
                source_rect,     
                dest_rect,       
                (Vector2){0, 0}, 
                0.0f,            
                WHITE            
            );
        // Floor
        DrawRectangle(x, y + height - 1, column_width, screen_height - (y + height), floor_color);
    }
}

void player_input_update(Player *self, Renderer *renderer) {
    if (IsKeyDown('W') == 1) {
        self->pos.x += cos(TO_RADIANS(self->angle)) * self-> velocity; 
        self->pos.y += sin(TO_RADIANS(self->angle)) * self-> velocity; 
    
    }
    if (IsKeyDown('A') == 1) {
       self->angle -= 10.0f; 
    }
    if (IsKeyDown('S') == 1) {
        self->pos.x -= cos(TO_RADIANS(self->angle)) * self-> velocity; 
        self->pos.y -= sin(TO_RADIANS(self->angle)) * self-> velocity; 

    }
    if (IsKeyDown('D') == 1) {
       self->angle += 10.0f; 
    }

    // Printf's don't work here for some reason?
    if (IsKeyDown('U') == 1) {
        renderer->type = MINIMAP; 
    }
    if (IsKeyDown('I') == 1) {
        renderer->type = FPS; 
    }


    if (self->angle >= 360.0f) {
        self->angle -= 360.0f;
    }
    if (self->angle < 0.0f) {
        self->angle += 360.0f;
    }
}



float dist(float ax, float ay, float bx, float by) {
    return sqrtf((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
}

Tuple_Return raycast_dda(Vector2 start, float angle, World *world) {

    Vector2 ray_pos = { 0.0, 0.0 };
    float y_offset, x_offset, map_x, map_y;
    int dof = 0;
    float hx = start.x;
    float hy = start.y;
    float vx = start.x;
    float vy = start.y; float dish = 10000.0;
    float disv = 10000.0;
          
    // Horizontal Line Check
    // ======================================================== 
    float atan = -1.0f / (tan(TO_RADIANS(angle)));

    // Direction Up? 
    if (sin(TO_RADIANS(angle)) < 0.0f) {
        // This snaps the value back to the nearest 64th value via a right and then left bitshift 
        ray_pos.y = (((int)start.y >> 6) << 6) - 0.001;
        ray_pos.x = start.x + (start.y - ray_pos.y) * atan;
        y_offset = -(world->tile_size);
        x_offset = y_offset * atan;
    }
    // Direciton Down?
    else if (sin(TO_RADIANS(angle)) > 0.0f) {
        ray_pos.y = (((int)start.y >> 6) << 6) + world->tile_size;
        ray_pos.x = start.x + (start.y - ray_pos.y) * atan;
        y_offset = world->tile_size;
        x_offset = y_offset * atan;
    }
    // Direciton Perfectly Horizontal
    else {
        ray_pos.x = start.x;
        ray_pos.y = start.y;
        dof = 8;

    }

    while (dof < 8) {
        // Get the current map coordinates where the ray is via bitshifting right 
        map_x = (int) ray_pos.x >> 6;
        map_y = (int) ray_pos.y >> 6; 

        if (map_x >= 0 && map_y >= 0 && map_x < world->map_size && map_y < world->map_size) {
            if (world->data[(int)map_x][(int)map_y] == STONE) {
                dof = 8;
                hx = ray_pos.x;
                hy = ray_pos.y;
                dish = dist(start.x, start.y, hx, hy);
            } else {
                ray_pos.x -= x_offset;
                ray_pos.y += y_offset; 
                dof += 1;
            }
        } else {
            dof = 8; 
        }
    }
    
    // Test
    /*
    Vector2 start_pos = {start.x, start.y};
    Vector2 end_pos = {
       ray_pos.x, 
       ray_pos.y, 
    };

    DrawLineEx(start_pos, end_pos, 5.0, RED);
    */

    // Vertical Line Check
    // ==========================================================
    dof = 0;
    atan = -(tan(TO_RADIANS(angle)));

    // Direction Up? 
    if (cos(TO_RADIANS(angle)) < 0.0f) {
        // This snaps the value back to the nearest 64th value via a right and then left bitshift 
        ray_pos.x = (((int)start.x >> 6) << 6) - 0.001;
        ray_pos.y = start.y + (start.x - ray_pos.x) * atan;
        x_offset = -(world->tile_size);
        y_offset = x_offset * atan;
    }
    // Direciton Down?
    else if (cos(TO_RADIANS(angle)) > 0.0f) {
        ray_pos.x = (((int)start.x >> 6) << 6) + world->tile_size;
        ray_pos.y = start.y + (start.x - ray_pos.x) * atan;
        x_offset = world->tile_size;
        y_offset = x_offset * atan;
    }
    // Direciton Perfectly Horizontal
    else {
        ray_pos.x = start.x;
        ray_pos.y = start.y;
        dof = 8;

    }

    while (dof < 8) {
        // Get the current map coordinates where the ray is via bitshifting right 
        map_x = (int) ray_pos.x >> 6;
        map_y = (int) ray_pos.y >> 6; 

        if (map_x >= 0 && map_y >= 0 && map_x < world->map_size && map_y < world->map_size) {
            if (world->data[(int)map_x][(int)map_y] == STONE) {
                dof = 8;
                vx = ray_pos.x;
                vy = ray_pos.y;
                disv = dist(start.x, start.y, vx, vy);
            } else {
                ray_pos.x += x_offset;
                ray_pos.y -= y_offset; 
                dof += 1;
            }
        } else {
            dof = 8; 
        }
    }
    /*
    // Test
    Vector2 start_pos = {start.x, start.y};
    Vector2 end_pos = {
       vx, 
       vy, 
    };

    DrawLineEx(start_pos, end_pos, 5.0, RED);
    */

    float final_dist;
    wall_t type;

    if (disv < dish) {
        // Vertical hit
        ray_pos.x = vx;
        ray_pos.y = vy;
        final_dist = disv;
        type = VERTICAL;

    } else {
        // Horizontal hit
        ray_pos.x = hx;
        ray_pos.y = hy;
        final_dist = dish;
        type = HORIZONTAL;
    }


    Tuple_Return final_vec = {
        final_dist,
        {
            ray_pos.x,
            ray_pos.y,
        },
        type,
    };
    
    /*
    // Test
    Vector2 start_pos = {start.x, start.y};
    Vector2 end_pos = {
       ray_pos.x, 
       ray_pos.y, 
    };

    DrawLineEx(start_pos, end_pos, 5.0, RED);
    */

    return final_vec;
}



