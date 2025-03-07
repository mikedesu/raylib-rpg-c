#include "libdraw.h"
#include "mprint.h"

void libdraw_init() {
    const char* title = "evildojo666";
    const int window_width = 800, window_height = 480;
    InitWindow(window_width, window_height, title);
    SetTargetFPS(60);
    msuccess("libdraw_init");
}

void libdraw_drawframe() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Libdraw Works!", 10, 10, 20, BLACK);
    EndDrawing();
}

void libdraw_close() {
    CloseWindow();
}

bool libdraw_windowshouldclose() {
    return WindowShouldClose();
}
