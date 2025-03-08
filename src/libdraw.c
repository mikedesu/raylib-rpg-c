#include "libdraw.h"
#include "gamestate.h"
#include "hashtable_entityid_spritegroup.h"
#include "mprint.h"
#include "textureinfo.h"

hashtable_entityid_spritegroup_t* spritegroups = NULL;
textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

gamestate* g = NULL;

void libdraw_init(const gamestate* const g) {
    if (g) {
        const char* title = "evildojo666";
        const int window_width = 800, window_height = 480;
        InitWindow(window_width, window_height, title);
        SetTargetFPS(60);
        msuccess("libdraw_init");
    } else {
        merror("libdraw_init g is NULL");
    }
}

void libdraw_update_input(inputstate* const is) {
    inputstate_update(is);
}

void libdraw_drawframe(const gamestate* const g) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("evildojo Works!", 10, 10, 20, BLACK);
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Hero ID: %d", g->hero_id);
    DrawText(buffer, 10, 40, 20, BLACK);
    EndDrawing();
}

void libdraw_close() {
    g = NULL;
    CloseWindow();
}

bool libdraw_windowshouldclose() {
    return WindowShouldClose();
}
