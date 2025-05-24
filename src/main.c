#include "raylib.h"
#include <stdio.h>
#include <math.h>

#include "../include/world.h"
#include "../include/player.h"
#include "../include/render.h"
#include "../include/state.h"
#include "../include/ui.h"

//TODO:
//1. Incorporate screen size into map draw
//3. Use LUT's instead of direction sin and cos calculations
//4. Make an editor renderer
//5. Add collision detection

// Make the global state available for everyone
State state;

int main() {
    // Initialize window
    InitWindow(screen_width, screen_height, "Raycaster in C");

    Texture2D atlas = LoadTexture("player_sheet.png");

    // Poor man's dependency injection?
    World world = world_new_temp(64, 64);
    Player player = player_new();
    Renderer renderer = renderer_new();
    UI ui = ui_new();
    int num_rays = 200;

    state.player = &player; 
    state.world = &world;
    state.renderer = &renderer;
    state.num_rays = num_rays;
    state.atlas = &atlas;
    state.ui = &ui;


    world.data[5][5] = STONE;
    world.data[5][6] = STONE;
    world.data[5][6] = STONE;

    world.data[16][16] = STONE;

    // Low fps cuz this hurts my rasberry pi rn
    SetTargetFPS(20);

        while (!WindowShouldClose()) {
            input_update();
            // Start drawing
            BeginDrawing();
            ClearBackground(GRAY);

            render(state.renderer);
                
            EndDrawing();
    }

    // Close window
    CloseWindow();

    return 0;
}

