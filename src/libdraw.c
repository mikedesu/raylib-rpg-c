#include "libdraw.h"
#include "mprint.h"

void libdraw_init() {
    const char* title = "evildojo666";
    const int window_width = 800, window_height = 480;
    InitWindow(window_width, window_height, title);
    SetTargetFPS(60);
    msuccess("libdraw_init");
}

void libdraw_update_input(inputstate* const is) {
    inputstate_update(is);
}

void libdraw_drawframe(const gamestate* const g) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Libdraw Works!", 10, 10, 20, BLACK);
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Hero ID: %d", g->hero_id);
    DrawText(buffer, 10, 40, 20, BLACK);
    EndDrawing();
}

void libdraw_close() {
    CloseWindow();
}

bool libdraw_windowshouldclose() {
    return WindowShouldClose();
}
