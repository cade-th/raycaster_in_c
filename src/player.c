#include "../include/player.h"
#include "../include/world.h"
#include "../include/state.h"

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

    DrawCircle(
        world_to_screen(self->pos, &state.renderer->camera).x, 
        world_to_screen(self->pos, &state.renderer->camera).y, 
        5.0,
        BLUE);

    Vector2 start_pos = {self->pos.x, self->pos.y};
    Vector2 end_pos = {
        self->pos.x + cos(TO_RADIANS(self->angle)) * 50.0f,
        self->pos.y + sin(TO_RADIANS(self->angle)) * 50.0f,
    };

    DrawLineEx(
        world_to_screen(start_pos, &state.renderer->camera),
        world_to_screen(end_pos, &state.renderer->camera),
        5.0, 
        RED);
    /*

    for (int i = 0; i < num_rays; i++) {
        Vector2 ray_pos = self->positions[i].hit_point;
        // Test
        Vector2 start_pos = {self->pos.x, self->pos.y};
        Vector2 end_pos = {
           ray_pos.x, 
           ray_pos.y, 
        };
        DrawLineEx(
            world_to_screen(start_pos, &state.renderer->camera),
            world_to_screen(end_pos, &state.renderer->camera),
            5.0, 
            RED);
    }

    */
}

void raycast_fov(Player *self, Vector2 pos, float angle, float fov) {

    float start_angle = angle - (fov / 2.0f);
    float angle_step = fov / ((float)state.num_rays- 1.0);

    for (int i = 0; i < state.num_rays; i++) {
        float angle_new = start_angle + (i * angle_step);
        Tuple_Return point = raycast_dda(pos, angle_new);

        // Fix fisheye effect
        float ca = angle - angle_new;
        point.distance *= cos(TO_RADIANS(ca));

        self->positions[i].distance = point.distance;
        self->positions[i].hit_point = point.hit_point;
        self->positions[i].type = point.type;
    }
}

void raycast_single(Player *self, Vector2 pos, float angle, float fov) {

        Tuple_Return point = raycast_dda(pos, self->angle);

        self->positions[0].distance = point.distance;
        self->positions[0].hit_point = point.hit_point;
        self->positions[0].type = point.type;
}


void render_fps(Player *self) {
    float column_width = ceil((float)screen_width / state.num_rays);

    for (int i = 0; i < state.num_rays; i++) {

        float wall_height = (screen_height * state.world->tile_size) / self->positions[i].distance;
        float y = (screen_height - wall_height) / 2.0;
        float x = i * column_width;
        float height = wall_height;

        Color ceiling_color = (Color){ 135, 206, 250, 255 };
        Color floor_color = (Color){ 139, 69, 19, 255 };

        int texture_width = 32;
        float hit_offset;

        // for textures
        if (self->positions[i].type == VERTICAL) {
            hit_offset = fmod(self->positions[i].hit_point.y, state.world->tile_size);
        } else {
            hit_offset = fmod(self->positions[i].hit_point.x, state.world->tile_size);
        }

        int texture_column = (int)((hit_offset / state.world->tile_size) * texture_width);

        Rectangle source_rect = {
            texture_column + 32,
            32,    
            1,   
            32    
        };

        Rectangle dest_rect = {
            x, y, column_width, height
        };

        // Ceiling
        // If I don't add 10 to y here than there is some gray area in the horizon
        DrawRectangle(x, 0, column_width, y + 10, ceiling_color);

        // Ray is out of the map so don't draw a wall
        if (self->positions[i].distance < 7000.0f) {
            // Wall
            DrawTexturePro(
                *state.atlas,          
                source_rect,     
                dest_rect,       
                (Vector2){0, 0}, 
                0.0f,            
                WHITE            
            );
        }
                // Floor
        DrawRectangle(x, y + height, column_width, screen_height - (y + height), floor_color);
    }
}



float dist(float ax, float ay, float bx, float by) {
    return sqrtf((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
}

Tuple_Return raycast_dda(Vector2 start, float angle) {

    Vector2 ray_pos = { 0.0, 0.0 };
    float y_offset, x_offset, map_x, map_y;
    int dof = 0;
    float hx = start.x;
    float hy = start.y;
    float vx = start.x;
    float vy = start.y; 
    float dish = 10000.0;
    float disv = 10000.0;
          
    // Horizontal Line Check
    // ======================================================== 
    float atan = -1.0f / (tan(TO_RADIANS(angle)));

    // Direction Up? 
    if (sin(TO_RADIANS(angle)) < 0.0f) {
        // This snaps the value back to the nearest 64th value via a right and then left bitshift 
        ray_pos.y = (((int)start.y >> 6) << 6) - 0.001;
        ray_pos.x = start.x + (start.y - ray_pos.y) * atan;
        y_offset = -(state.world->tile_size);
        x_offset = y_offset * atan;
    }
    // Direciton Down?
    else if (sin(TO_RADIANS(angle)) > 0.0f) {
        ray_pos.y = (((int)start.y >> 6) << 6) + state.world->tile_size;
        ray_pos.x = start.x + (start.y - ray_pos.y) * atan;
        y_offset = state.world->tile_size;
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

        if (map_x >= 0 && map_y >= 0 && map_x < state.world->map_size && map_y < state.world->map_size) {
            if (state.world->data[(int)map_x][(int)map_y] == STONE) {
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
        x_offset = -(state.world->tile_size);
        y_offset = x_offset * atan;
    }
    // Direciton Down?
    else if (cos(TO_RADIANS(angle)) > 0.0f) {
        ray_pos.x = (((int)start.x >> 6) << 6) + state.world->tile_size;
        ray_pos.y = start.y + (start.x - ray_pos.x) * atan;
        x_offset = state.world->tile_size;
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

        if (map_x >= 0 && map_y >= 0 && map_x < state.world->map_size && map_y < state.world->map_size) {
            if (state.world->data[(int)map_x][(int)map_y] == STONE) {
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

    if (disv >= 10000.0f && dish >= 10000.0f) {
        // No wall hit, ray exited the map
        final_dist = 10000.0f;  // Large distance to ignore the ray during rendering
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



