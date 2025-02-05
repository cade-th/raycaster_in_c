#pragma once

#include "state.h"
#include "raylib.h"
#include <math.h>
#include "world.h"

#define TO_RADIANS(degrees) (degrees * (3.14159f / 180.0f))

// Need this forward declaration or else the compiler bitches
struct Renderer;

typedef enum wall_t {
    VERTICAL,
    HORIZONTAL,
} wall_t;

// C doesn't have tuples so I'm doing this as a return value for the raycast function
typedef struct Tuple_Return{
    float distance;
    Vector2 hit_point;
    wall_t type;
} Tuple_Return;

typedef struct Player {
    Vector2 pos;
    float velocity;
    float angle;
    Tuple_Return positions[250];
} Player;

Player player_new();
Tuple_Return raycast_dda(Vector2 start, float angle, World *world);
float dist(float ax, float ay, float bx, float by);
void player_input_update(Player *self, struct Renderer *renderer);
Tuple_Return raycast_dda(Vector2 start, float angle, World *world);
void render_player(Player *self, float num_rays);
void raycast_fov(Player *self, Vector2 pos, float angle, float fov, int num_rays, World *world);
void render_fps(Player *self, int num_rays, World *world, Texture2D *atlas);
