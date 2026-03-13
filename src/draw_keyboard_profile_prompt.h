#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

static inline void draw_keyboard_profile_prompt(gamestate& g) {
    if (!g.display_keyboard_profile_prompt) {
        return;
    }

    const int box_w = 420;
    const int box_h = 150;
    const int box_x = (DEFAULT_TARGET_WIDTH - box_w) / 2;
    const int box_y = (DEFAULT_TARGET_HEIGHT - box_h) / 2;
    const Rectangle box = {(float)box_x, (float)box_y, (float)box_w, (float)box_h};

    DrawRectangleRec(box, g.window_box_bgcolor);
    DrawRectangleLinesEx(box, 2.0f, g.window_box_fgcolor);

    DrawText("Choose Keyboard Layout", box_x + 16, box_y + 14, 20, g.window_box_fgcolor);
    DrawText("Select a default gameplay profile before entering the dungeon.", box_x + 16, box_y + 42, 10, g.window_box_fgcolor);
    DrawText("You can change this later in ` -> controls.", box_x + 16, box_y + 56, 10, g.window_box_fgcolor);

    for (int i = 0; i < KEYBOARD_PROFILE_COUNT; i++) {
        const bool selected = g.keyboard_profile_selection == (unsigned int)i;
        const int option_y = box_y + 88 + i * 18;
        const char* label = keyboard_profile_label(static_cast<keyboard_profile_t>(i));
        DrawText(selected ? TextFormat("> %s", label) : TextFormat("  %s", label), box_x + 24, option_y, 12, selected ? YELLOW : g.window_box_fgcolor);
    }

    DrawText("Arrows to choose, Enter to confirm", box_x + 16, box_y + box_h - 20, 10, g.window_box_fgcolor);
}
