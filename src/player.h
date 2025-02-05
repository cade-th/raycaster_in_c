#pragma once

#include "raylib.h"
#include <math.h>
#define TO_RADIANS(degrees) (degrees * (3.14159f / 180.0f))

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
Tuple_Return raycast_dda(Vector2 start, float angle);
float dist(float ax, float ay, float bx, float by);
Tuple_Return raycast_dda(Vector2 start, float angle);
void render_player(Player *self, float num_rays);
void raycast_fov(Player *self, Vector2 pos, float angle, float fov);
void render_fps(Player *self);
