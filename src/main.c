#include "raylib.h"
#include <stdio.h>
#include "math.h"
#include "textures.h"

#define TO_RADIANS(degrees) (degrees * (3.14159f / 180.0f))

//TODO:
//1. Incorporate screen size into map draw
//2. Fix rays being drawn at infinite distance

const int screen_width = 512;
const int screen_height = 512;

typedef enum {
    VERTICAL,
    HORIZONTAL,
}wall_t;
// C doesn't have tuples so I'm doing this as a return value for the raycast function
typedef struct {
    float distance;
    Vector2 hit_point;
    wall_t type;
} Tuple_Return;

typedef struct {
    Vector2 pos;
    float velocity;
    float angle;
    Tuple_Return positions[250];
} Player;

typedef struct {
    int map_size;
    float tile_size;
    int data[8][8];
} World;

typedef enum {
    MINIMAP,
    FPS,
} renderer_t;

typedef struct {
   renderer_t type; 
} Renderer;


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
        printf("Hello?");
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
            if (world->data[(int)map_x][(int)map_y] == 1) {
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
            if (world->data[(int)map_x][(int)map_y] == 1) {
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

void render_minimap(Player *self, float num_rays) {
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

// world->tile_size = 64
// the screen is 512x512 pixels
// the texture in the atlas is 32x32 pixels, at (0,0) in the atlas
// the raycaster is casting 200 rays 
// int texture_column = (int)((hit_offset / world->tile_size) * texture_width);
// hit_offset = fmod(self->positions[i].hit_point.y, world->tile_size);
void render_fps(Player *self, int num_rays, World *world, Texture2D *atlas) {
    float column_width = ceil((float)screen_width / num_rays);

    for (int i = 0; i < num_rays; i++) {
        float wall_height = (screen_height * world->tile_size) / self->positions[i].distance;
        float y = (screen_height - wall_height) / 2.0;
        float x = i * column_width;
        float height = wall_height;

        Color ceiling_color = (Color){ 135, 206, 250, 255 };
        Color floor_color = (Color){ 139, 69, 19, 255 };

        // Assume world->tile_size = 64 and texture width is 32
        int texture_width = 32;
        float hit_offset;

        // Choose the correct coordinate based on the wall hit type
        if (self->positions[i].type == VERTICAL) {
            // Vertical collision: use the y coordinate of the hit point.
            hit_offset = fmod(self->positions[i].hit_point.y, world->tile_size);
        } else if (self->positions[i].type == HORIZONTAL) {
            // Horizontal collision: use the x coordinate of the hit point.
            hit_offset = fmod(self->positions[i].hit_point.x, world->tile_size);
        }

        // Map the hit offset to a texture column (assuming your texture is 32 pixels wide)
        int texture_column = (int)((hit_offset / world->tile_size) * texture_width);

        // Corrected source rectangle to render texture at (0,0)
        Rectangle source_rect = {
            texture_column,
            32,    // Y position in the atlas
            1,   // Width of the sub-texture
            32    // Height of the sub-texture
        };

        Rectangle dest_rect = {
            x, y, column_width, height
        };

        // Draw the ceiling section
        DrawRectangle(x, 0, column_width, y, ceiling_color);

        // Draw the wall segment with texture mapping
        if (self->positions[i].distance < 1000) {
            DrawTexturePro(
                *atlas,          // The texture atlas
                source_rect,     // Now correctly using texture at (0,0)
                dest_rect,       // Destination rectangle on screen
                (Vector2){0, 0}, // Origin offset (top-left corner)
                0.0f,            // Rotation
                WHITE            // Tint color (none)
            );
        }

        // Draw the floor section
        DrawRectangle(x, y + height - 1, column_width, screen_height - (y + height), floor_color);
    }
}

void render_world(World *world) {  
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

Renderer renderer_new() {
    Renderer renderer = {
        MINIMAP,
    };
    return renderer;
}

void render(Renderer *self, Player *player, int num_rays, World *world, Texture2D *atlas) {
        raycast_fov(player, player->pos, player->angle, 60.0, num_rays,  world);

        if (self->type == MINIMAP) {
            render_world(world);
            render_minimap(player, num_rays);
        }
        if (self->type == FPS) {
            render_fps(player, num_rays, world, atlas);
        }
}

int main() {
    // Initialize window
    InitWindow(screen_width, screen_height, "Raycaster in C");

    Texture2D atlas = LoadTexture("player_sheet.png");

    World world = world_new(8, 64);
    Player player = player_new();
    Renderer renderer = renderer_new();

    int num_rays = 200;

    // Low fps cuz this hurts my rasberry pi rn
    SetTargetFPS(20);

        while (!WindowShouldClose()) {
            player_input_update(&player, &renderer);
            // Start drawing
            BeginDrawing();
            ClearBackground(BLACK);

            render(&renderer, &player, num_rays, &world, &atlas);
                
            EndDrawing();
    }

    // Close window
    CloseWindow();

    return 0;
}

