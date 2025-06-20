#include "libdraw_sort_inventory_menu.h"
#include "libgame_defines.h"

void draw_sort_inventory_menu(gamestate* const g) {
    massert(g, "gamestate is NULL");
    if (!g->display_sort_inventory_menu) return;
    // Parameters
    const char* menu_title = "Sort By";
    int font_size = 20;
    int menu_spacing = 15;
    const char* menu_text[] = {"Name", "Type"};
    int menu_count = sizeof(menu_text) / sizeof(menu_text[0]);
    int current_selection = g->sort_inventory_menu_selection;
    massert(current_selection >= 0 && current_selection < menu_count, "current_selection is out of bounds");
    // Calculate menu size based on longest text
    int max_text_width = MeasureText(menu_title, font_size);
    for (int i = 0; i < menu_count; i++) {
        int width = MeasureText(menu_text[i], font_size);
        if (width > max_text_width) max_text_width = width;
    }
    // Position menu centered within inventory menu
    float menu_width = max_text_width + 40;
    float menu_height = (font_size + menu_spacing) * menu_count + 30;
    int w = DEFAULT_TARGET_WIDTH;
    int h = DEFAULT_TARGET_HEIGHT;
    float menu_x = (w - menu_width) / 2;
    float menu_y = (h - menu_height) / 2;
    // Draw background box
    DrawRectangle(menu_x, menu_y, menu_width, menu_height, (Color){0x22, 0x22, 0x22, 0xff});
    DrawRectangleLinesEx((Rectangle){menu_x, menu_y, menu_width, menu_height}, 2, WHITE);
    // Draw title
    float title_x = menu_x + (menu_width - MeasureText(menu_title, font_size)) / 2;
    DrawText(menu_title, title_x, menu_y + 10, font_size, WHITE);
    // Draw menu items
    float item_x = menu_x + 20;
    float item_y = menu_y + 40;
    for (int i = 0; i < menu_count; i++) {
        Color color = (i == current_selection) ? YELLOW : WHITE;
        // Draw selection indicator
        if (i == current_selection) {
            DrawText(">", item_x - 15, item_y, font_size, color);
        }
        DrawText(menu_text[i], item_x, item_y, font_size, color);
        item_y += font_size + menu_spacing;
    }
}
