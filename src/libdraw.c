#include "libdraw.h"
#include "mprint.h"

static inputstate* is_ptr = NULL; // Store for drawframe

void libdraw_init() {
    const char* title = "evildojo666";
    const int window_width = 800, window_height = 480;
    InitWindow(window_width, window_height, title);
    SetTargetFPS(60);
    msuccess("libdraw_init");
}

void libdraw_update_input(inputstate* const is) {
    is_ptr = is; // Save for drawing
    inputstate_update(is);
}

void libdraw_drawframe() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Libdraw Works!", 10, 10, 20, BLACK);
    if (is_ptr) {
        if (inputstate_is_pressed(is_ptr, KEY_SPACE)) {
            DrawText("Space Pressed!", 10, 40, 20, BLACK);
        }
        if (inputstate_is_held(is_ptr, KEY_RIGHT)) {
            DrawText("Right Held!", 10, 60, 20, BLACK);
        }
    }
    EndDrawing();
}

void libdraw_close() {
    CloseWindow();
}

bool libdraw_windowshouldclose() {
    return WindowShouldClose();
}
