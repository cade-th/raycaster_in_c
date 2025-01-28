#include "raylib.h"
#include "math.h"

#define TO_RADIANS(degrees) (degrees * (3.14159f / 180.0f))

//TODO:
//1. Incorporate screen size into map draw

const int screen_width = 512;
const int screen_height = 512;

typedef struct {
    float x;
    float y; 
    float velocity;
    float angle;
} Player;

Player player_new() {
    Player player = {
        200.0f,
        200.0f,
        2.0f,
        0.0f,
    };
    return player;
}

void player_input_update(Player *player) {
    if (IsKeyDown('W') == 1) {
        player->x += cos(TO_RADIANS(player->angle)) * player-> velocity; 
        player->y += sin(TO_RADIANS(player->angle)) * player-> velocity; 
    
    }
    if (IsKeyDown('A') == 1) {

       player->angle -= 2.0f; 
    }
    if (IsKeyDown('S') == 1) {
        player->x -= cos(TO_RADIANS(player->angle)) * player-> velocity; 
        player->y -= sin(TO_RADIANS(player->angle)) * player-> velocity; 

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

void render_player(Player *player) {
    DrawCircle(player->x, player->y, 5.0, BLUE);

    Vector2 start_pos = {player->x, player->y};
    Vector2 end_pos = {
        player->x + cos(TO_RADIANS(player->angle)) * 50.0f,
        player->y + sin(TO_RADIANS(player->angle)) * 50.0f,
    };

    DrawLineEx(start_pos, end_pos, 5.0, RED);
}

typedef struct {
    int map_size;
    int tile_size;
    int data[8][8];
} World;

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
        
        EndDrawing();
    }

    // Close window
    CloseWindow();

    return 0;
}

