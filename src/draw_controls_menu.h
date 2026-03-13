#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

static inline void draw_controls_menu(gamestate& g) {
    if (!g.display_controls_menu) {
        return;
    }

    constexpr int font_size = 10;
    constexpr int line_height = 12;
    constexpr int padding = 12;
    const int visible_rows = INPUT_ACTION_COUNT + 4;
    const int box_w = 500;
    const int box_h = visible_rows * line_height + padding * 2 + 24;
    const int box_x = (DEFAULT_TARGET_WIDTH - box_w) / 2;
    const int box_y = (DEFAULT_TARGET_HEIGHT - box_h) / 2;
    const Rectangle box = {(float)box_x, (float)box_y, (float)box_w, (float)box_h};

    DrawRectangleRec(box, Color{14, 20, 32, 235});
    DrawRectangleLinesEx(box, 2.0f, RAYWHITE);

    DrawText("Keyboard Controls", box_x + padding, box_y + padding, 18, RAYWHITE);
    const char* subtitle = g.controls_menu_waiting_for_key
        ? TextFormat("Press a new key for %s. Esc cancels.", gameplay_input_action_label(g.controls_menu_pending_action))
        : "Up/Down select, Enter edit, Left/Right swap profile, Esc closes";
    DrawText(subtitle, box_x + padding, box_y + padding + 20, font_size, LIGHTGRAY);

    int y = box_y + padding + 38;
    const bool profile_selected = g.controls_menu_selection == 0;
    DrawText(profile_selected ? TextFormat("> Profile: %s", keyboard_profile_label(g.keyboard_profile))
                              : TextFormat("  Profile: %s", keyboard_profile_label(g.keyboard_profile)),
        box_x + padding, y, font_size, profile_selected ? YELLOW : RAYWHITE);
    y += line_height;

    const bool reset_selected = g.controls_menu_selection == 1;
    DrawText(reset_selected ? "> Reset Current Profile To Defaults" : "  Reset Current Profile To Defaults",
        box_x + padding, y, font_size, reset_selected ? YELLOW : RAYWHITE);
    y += line_height;

    for (int action = 0; action < INPUT_ACTION_COUNT; action++) {
        const bool selected = g.controls_menu_selection == static_cast<size_t>(action + 2);
        const char* action_label = gameplay_input_action_label(static_cast<gameplay_input_action_t>(action));
        const string binding = g.get_keybinding_label(g.keyboard_profile, static_cast<gameplay_input_action_t>(action));
        DrawText(selected ? TextFormat("> %-22s %s", action_label, binding.c_str())
                          : TextFormat("  %-22s %s", action_label, binding.c_str()),
            box_x + padding, y, font_size, selected ? YELLOW : RAYWHITE);
        y += line_height;
    }
}
