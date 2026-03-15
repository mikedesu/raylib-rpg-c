/** @file draw_sound_menu.h
 *  @brief Sound-settings menu drawing helper.
 */

#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

/** @brief Draw the sound-settings menu and selected volume control. */
static inline void draw_sound_menu(gamestate& g) {
    if (!g.display_sound_menu) {
        return;
    }

    constexpr int font_size = 12;
    constexpr int title_font_size = 18;
    constexpr int line_height = 16;
    constexpr int padding = 12;
    const int box_w = 360;
    const int box_h = 140;
    const int box_x = (DEFAULT_TARGET_WIDTH - box_w) / 2;
    const int box_y = (DEFAULT_TARGET_HEIGHT - box_h) / 2;
    const Rectangle box = {(float)box_x, (float)box_y, (float)box_w, (float)box_h};

    DrawRectangleRec(box, g.window_box_bgcolor);
    DrawRectangleLinesEx(box, 2.0f, g.window_box_fgcolor);

    DrawText("Sound", box_x + padding, box_y + padding, title_font_size, g.window_box_fgcolor);
    DrawText("Up/Down select, Left/Right adjust, Esc returns", box_x + padding, box_y + padding + 22, font_size, g.window_box_fgcolor);

    const char* labels[] = {"Master", "Music", "Sound Effects"};
    const float values[] = {g.master_volume, g.music_volume, g.sfx_volume};
    int y = box_y + padding + 48;
    for (size_t i = 0; i < 3; i++) {
        const bool selected = g.sound_menu_selection == i;
        DrawText(selected ? TextFormat("> %-14s %3d%%", labels[i], (int)(values[i] * 100.0f))
                          : TextFormat("  %-14s %3d%%", labels[i], (int)(values[i] * 100.0f)),
            box_x + padding,
            y,
            font_size,
            selected ? YELLOW : g.window_box_fgcolor);
        y += line_height;
    }
}
