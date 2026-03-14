/** @file draw_window_color_menu.h
 *  @brief Window-box color configuration menu drawing helper.
 */

#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

/** @brief Draw the window-box color configuration menu and preview swatch. */
static inline void draw_window_color_menu(gamestate& g) {
    if (!g.display_window_color_menu) {
        return;
    }

    constexpr int font_size = 11;
    constexpr int title_font_size = 18;
    constexpr int line_height = 14;
    constexpr int padding = 12;
    const int box_w = 420;
    const int box_h = 220;
    const int box_x = (DEFAULT_TARGET_WIDTH - box_w) / 2;
    const int box_y = (DEFAULT_TARGET_HEIGHT - box_h) / 2;
    const Rectangle box = {(float)box_x, (float)box_y, (float)box_w, (float)box_h};

    DrawRectangleRec(box, g.window_box_bgcolor);
    DrawRectangleLinesEx(box, 2.0f, g.window_box_fgcolor);

    DrawText("Window Box Colors", box_x + padding, box_y + padding, title_font_size, g.window_box_fgcolor);
    DrawText("Up/Down select, Left/Right adjust, Enter reset, Esc returns", box_x + padding, box_y + padding + 22, font_size, g.window_box_fgcolor);

    const unsigned char values[] = {
        g.window_box_bgcolor.r, g.window_box_bgcolor.g, g.window_box_bgcolor.b, g.window_box_bgcolor.a,
        g.window_box_fgcolor.r, g.window_box_fgcolor.g, g.window_box_fgcolor.b, g.window_box_fgcolor.a
    };
    const char* labels[] = {
        "Background R", "Background G", "Background B", "Background A",
        "Foreground R", "Foreground G", "Foreground B", "Foreground A",
        "Reset Defaults"
    };

    int y = box_y + padding + 48;
    for (size_t i = 0; i < 9; i++) {
        const bool selected = g.window_color_menu_selection == i;
        const char* text = i < 8 ? TextFormat("%c %-14s %3d", selected ? '>' : ' ', labels[i], values[i])
                                 : (selected ? "> Reset Defaults" : "  Reset Defaults");
        DrawText(text, box_x + padding, y, font_size, selected ? YELLOW : g.window_box_fgcolor);
        y += line_height;
    }

    Rectangle preview = {(float)(box_x + box_w - 112), (float)(box_y + 60), 84.0f, 84.0f};
    DrawRectangleRec(preview, g.window_box_bgcolor);
    DrawRectangleLinesEx(preview, 2.0f, g.window_box_fgcolor);
    DrawText("Preview", (int)preview.x + 10, (int)preview.y + 32, font_size, g.window_box_fgcolor);
}
