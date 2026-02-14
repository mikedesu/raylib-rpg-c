#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

static inline void draw_help_menu(gamestate& g) {
    const char* text = "Help Menu\n"
                       "\n"
                       "Movement: numpad\n"
                       "Change direction: numpad 5\n"
                       "Wait one turn: numpad 5 twice\n"
                       "\n"
                       "Pick up item: s\n"
                       "Attack: a\n"
                       "\n"
                       "Open Inventory: i\n"
                       "Equip item: e\n"
                       "Drop item: \n"
                       "Leave Inventory: escape -or- i"
                       "\n"
                       "Open this help menu: ?\n"
                       "\n"
                       "@evildojo666";


    constexpr int font_size = 10;

    constexpr int line_count_0 = 17;
    constexpr int line_count = line_count_0 + 2;

    constexpr int text_height = font_size * line_count;

    const int text_width = MeasureText(text, font_size);

    const int box_x = DEFAULT_TARGET_WIDTH / 2 - text_width / 2 - DEFAULT_PAD * 2;
    const int box_y = DEFAULT_TARGET_HEIGHT / 2 - text_height / 2 - DEFAULT_PAD * 2;

    const int box_width = text_width + DEFAULT_PAD * 2;
    const int box_height = text_height + DEFAULT_PAD * 2;

    constexpr Color box_color = Color{0, 0, 255, 127};
    DrawRectangle(box_x, box_y, box_width, box_height, box_color);

    const float box_x_f = box_x;
    const float box_y_f = box_y;
    const float box_w_f = box_width;
    const float box_h_f = box_height;
    constexpr int thick = 2;
    constexpr Color box_outline_color = WHITE;

    DrawRectangleLinesEx({box_x_f, box_y_f, box_w_f, box_h_f}, thick, box_outline_color);

    const int text_x = box_x + DEFAULT_PAD;
    const int text_y = box_y + DEFAULT_PAD;
    DrawText(text, text_x, text_y, font_size, WHITE);



    //help_menu.push_back("Press Esc to close this menu");
    //const int font_size = 20, sm_font_size = 10, padding = 20, line_spacing = 4;
    // Calculate sizes
    //const int w = DEFAULT_TARGET_WIDTH, h = DEFAULT_TARGET_HEIGHT, title_width = MeasureText(menu_title, font_size);
    // Calculate max text width
    //int max_text_width = title_width;
    //for (size_t i = 0; i < help_menu.size(); i++) {
    //    int width = MeasureText(help_menu[i].c_str(), sm_font_size);
    //    if (width > max_text_width)
    //        max_text_width = width;
    //}
    // Menu box dimensions
    //const int box_width = max_text_width + padding * 2, box_height = help_menu.size() * (sm_font_size + line_spacing) + padding * 2;
    //const int box_x = (w - box_width) / 2, box_y = (h - box_height) / 2;
    //// Draw background
    //DrawRectangle(box_x, box_y, box_width, box_height, (Color){0x33, 0x33, 0x33, 0xcc});
    //DrawRectangleLinesEx((Rectangle){(float)box_x, (float)box_y, (float)box_width, (float)box_height}, 2, WHITE);
    //// Draw title
    //const int title_x = box_x + (box_width - title_width) / 2, title_y = box_y + padding;
    //DrawText(menu_title, title_x, title_y, font_size, WHITE);
    //// Draw help text
    //const int text_x = box_x + padding;
    //int text_y = title_y + font_size + padding;
    //for (size_t i = 0; i < help_menu.size(); i++) {
    //    DrawText(help_menu[i].c_str(), text_x, text_y, sm_font_size, WHITE);
    //    text_y += sm_font_size + line_spacing;
    //}
}
