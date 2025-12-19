#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

static inline void draw_help_menu(gamestate& g) {
    //massert(g, "gamestate is NULL");
    // Menu parameters
    const char* menu_title = "Help Menu";
    vector<string> help_menu;
    help_menu.push_back("Movement: qweadzxc");
    help_menu.push_back("Attack: Spacebar");
    help_menu.push_back("Interact: Enter");
    help_menu.push_back("Inventory: I");
    help_menu.push_back("Settings: Esc");
    help_menu.push_back("Quit: Q");
    help_menu.push_back("");
    help_menu.push_back("Press Esc to close this menu");
    const int font_size = 20, sm_font_size = 10, padding = 20, line_spacing = 4;
    // Calculate sizes
    const int w = DEFAULT_TARGET_WIDTH, h = DEFAULT_TARGET_HEIGHT, title_width = MeasureText(menu_title, font_size);
    // Calculate max text width
    int max_text_width = title_width;
    for (size_t i = 0; i < help_menu.size(); i++) {
        int width = MeasureText(help_menu[i].c_str(), sm_font_size);
        if (width > max_text_width)
            max_text_width = width;
    }
    // Menu box dimensions
    const int box_width = max_text_width + padding * 2, box_height = help_menu.size() * (sm_font_size + line_spacing) + padding * 2;
    const int box_x = (w - box_width) / 2, box_y = (h - box_height) / 2;
    // Draw background
    DrawRectangle(box_x, box_y, box_width, box_height, (Color){0x33, 0x33, 0x33, 0xcc});
    DrawRectangleLinesEx((Rectangle){(float)box_x, (float)box_y, (float)box_width, (float)box_height}, 2, WHITE);
    // Draw title
    const int title_x = box_x + (box_width - title_width) / 2, title_y = box_y + padding;
    DrawText(menu_title, title_x, title_y, font_size, WHITE);
    // Draw help text
    const int text_x = box_x + padding;
    int text_y = title_y + font_size + padding;
    for (size_t i = 0; i < help_menu.size(); i++) {
        DrawText(help_menu[i].c_str(), text_x, text_y, sm_font_size, WHITE);
        text_y += sm_font_size + line_spacing;
    }
}
