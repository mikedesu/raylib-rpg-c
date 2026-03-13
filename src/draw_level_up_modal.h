#pragma once

#include "gamestate.h"
#include "libgame_defines.h"
#include <array>

static inline void draw_level_up_modal(gamestate& g) {
    if (!g.display_level_up_modal) {
        return;
    }

    constexpr int title_font_size = 26;
    constexpr int body_font_size = 18;
    constexpr int cell_font_size = 20;
    constexpr int padding = DEFAULT_PAD;
    constexpr int cols = 2;
    constexpr int rows = 3;
    constexpr int cell_w = 200;
    constexpr int cell_h = 52;
    constexpr int cell_gap = 12;

    const std::array<const char*, 6> labels = {
        "Strength",
        "Dexterity",
        "Constitution",
        "Intelligence",
        "Wisdom",
        "Charisma",
    };

    const int grid_w = cols * cell_w + (cols - 1) * cell_gap;
    const int grid_h = rows * cell_h + (rows - 1) * cell_gap;
    const int box_w = grid_w + padding * 2;
    const int box_h = padding * 2 + title_font_size + body_font_size + 20 + grid_h;
    const int box_x = (DEFAULT_TARGET_WIDTH - box_w) / 2;
    const int box_y = (DEFAULT_TARGET_HEIGHT - box_h) / 2;

    DrawRectangle(box_x, box_y, box_w, box_h, g.window_box_bgcolor);
    DrawRectangleLinesEx(Rectangle{static_cast<float>(box_x), static_cast<float>(box_y), static_cast<float>(box_w), static_cast<float>(box_h)}, 2, g.window_box_fgcolor);

    const char* title = "Level Up";
    const char* subtitle = "Choose one attribute to permanently increase by 1";
    const int title_x = box_x + (box_w - MeasureText(title, title_font_size)) / 2;
    const int subtitle_x = box_x + (box_w - MeasureText(subtitle, body_font_size)) / 2;
    DrawText(title, title_x, box_y + padding, title_font_size, g.window_box_fgcolor);
    DrawText(subtitle, subtitle_x, box_y + padding + title_font_size + 8, body_font_size, g.window_box_fgcolor);

    const int grid_x = box_x + padding;
    const int grid_y = box_y + padding + title_font_size + body_font_size + 20;
    for (int i = 0; i < 6; i++) {
        const int col = i % cols;
        const int row = i / cols;
        const int cell_x = grid_x + col * (cell_w + cell_gap);
        const int cell_y = grid_y + row * (cell_h + cell_gap);
        const bool selected = static_cast<int>(g.level_up_selection % 6) == i;
        const Color fill = selected ? Color{0x00, 0x88, 0x44, 220} : Color{0x11, 0x11, 0x66, 180};
        DrawRectangle(cell_x, cell_y, cell_w, cell_h, fill);
        DrawRectangleLinesEx(Rectangle{static_cast<float>(cell_x), static_cast<float>(cell_y), static_cast<float>(cell_w), static_cast<float>(cell_h)}, 2, g.window_box_fgcolor);

        const int text_w = MeasureText(labels[i], cell_font_size);
        const int text_x = cell_x + (cell_w - text_w) / 2;
        const int text_y = cell_y + (cell_h - cell_font_size) / 2;
        DrawText(labels[i], text_x, text_y, cell_font_size, g.window_box_fgcolor);
    }
}
