#include "raylib.h"
#include <stdio.h>
#include "math.h"

#include "world.h"
#include "player.h"
#include "render.h"
#include "state.h"

//TODO:
//1. Incorporate screen size into map draw
//2. Fix rays being drawn at infinite distance
//3. Use LUT's instead of direction sin and cos calculations
//4. Make an editor renderer
//5. Add collision detection

State state;

int main() {
    // Initialize window
    InitWindow(screen_width, screen_height, "Raycaster in C");

    Texture2D atlas = LoadTexture("player_sheet.png");

    // Poor man's dependency injection?
    World world = world_new(8, 64);
    Player player = player_new();
    Renderer renderer = renderer_new();
    int num_rays = 200;

    state.player = &player; 
    state.world = &world;
    state.renderer = &renderer;
    state.num_rays = num_rays;
    state.atlas = &atlas;


    world.data[10][10] = STONE;
    world.data[10][11] = STONE;
    world.data[10][12] = STONE;

    // Low fps cuz this hurts my rasberry pi rn
    SetTargetFPS(20);

        while (!WindowShouldClose()) {
            input_update();
            // Start drawing
            BeginDrawing();
            ClearBackground(WHITE);

            render(state.renderer);
                
            EndDrawing();
    }

    // Close window
    CloseWindow();

    return 0;
}

