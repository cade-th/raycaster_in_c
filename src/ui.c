#include "ui.h"
#include "raylib.h"
#include "state.h"
#include "stdio.h"

UI ui_new() {
    UI ui = {

    };
    
    return ui;
}

void ui_render_debug() {
    
    int distance = state.player->positions[0].distance;
    char str[20];
    sprintf(str, "%d", distance);

    DrawText(
        str,
        25,
        25,
        20,
        BLACK
    );


}
