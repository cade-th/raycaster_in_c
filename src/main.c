#include "raylib.h"
#include "math.h"

#define TO_RADIANS(degrees) (degrees * (3.14159f / 180.0f))

//TODO:
//1. Incorporate screen size into map draw

const int screen_width = 512;
const int screen_height = 512;

typedef struct {
    Vector2 pos;
    float velocity;
    float angle;
} Player;

typedef struct {
    int map_size;
    int tile_size;
    int data[8][8];
} World;

Player player_new() {
    Player player = {
        {
            200.0f,
            200.0f,
        },
        2.0f,
        0.0f,
    };
    return player;
}

void player_input_update(Player *player) {
    if (IsKeyDown('W') == 1) {
        player->pos.x += cos(TO_RADIANS(player->angle)) * player-> velocity; 
        player->pos.y += sin(TO_RADIANS(player->angle)) * player-> velocity; 
    
    }
    if (IsKeyDown('A') == 1) {

       player->angle -= 2.0f; 
    }
    if (IsKeyDown('S') == 1) {
        player->pos.x -= cos(TO_RADIANS(player->angle)) * player-> velocity; 
        player->pos.y -= sin(TO_RADIANS(player->angle)) * player-> velocity; 

    }
    if (IsKeyDown('D') == 1) {
       player->angle += 2.0f; 
    }

    if (player->angle >= 360.0f) {
        player->angle -= 360.0f;
    }
    if (player->angle < 0.0f) {
        player->angle += 360.0f;
    }
}


// C doesn't have tuples so I'm doing this as a return value for the raycast function
typedef struct {
    float distance;
    Vector2 hit_point;
} Tuple_Return;

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

    if (disv < dish) {
        ray_pos.x = vx;
        ray_pos.y = vy;
        final_dist = disv;
    } else {
        ray_pos.x = hx;
        ray_pos.y = hy;
        final_dist = dish;
    }

    Tuple_Return final_vec = {
        final_dist,
        {
            ray_pos.x,
            ray_pos.y,
        },
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

// Do this later at some point
void raycast_fov() {
    return true;
}

void render_player(Player *player) {
    DrawCircle(player->pos.x, player->pos.y, 5.0, BLUE);

    Vector2 start_pos = {player->pos.x, player->pos.y};
    Vector2 end_pos = {
        player->pos.x + cos(TO_RADIANS(player->angle)) * 50.0f,
        player->pos.y + sin(TO_RADIANS(player->angle)) * 50.0f,
    };

    DrawLineEx(start_pos, end_pos, 5.0, RED);
}


void render_world(World *world) {  
    for (int i=0; i < world->map_size; i++) {
        for (int j=0; j < world->map_size; j++) {
            Color color = WHITE;
            if (world->data[i][j] == 1) {
                color = BLACK;
            }
            DrawRectangle(i * world->tile_size, j * world->tile_size, 64 - 1, 64 - 1, color);
            }
        }
}

World world_new(int world_size, int tile_size) {
    World world = {
        8,
        64,
        .data = {
            {1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,1},
            {1,0,0,1,0,0,0,1},
            {1,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1}
        }
    };

    return world;
}

int main() {
    // Initialize window
    InitWindow(screen_width, screen_height, "Raycaster in C");

    World world = world_new(8, 64);
    Player player = player_new();

        while (!WindowShouldClose()) {
        player_input_update(&player);
        // Start drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);


        render_world(&world);
        render_player(&player);
        raycast_dda(player.pos, player.angle, &world); 
        
        EndDrawing();
    }

    // Close window
    CloseWindow();

    return 0;
}

