#include "libdraw_title_screen.h"
#include "libgame_defines.h"

void draw_title_screen(shared_ptr<gamestate> g, bool show_menu) {
    massert(g, "gamestate is NULL");
    // Space between title texts
    Color active_color = WHITE;
    Color disabled_color = {0x99, 0x99, 0x99, 0xFF};
    Color selection_color;
    // this will go above the title text
    const char* evidojo_presents_text = "@evildojo666 presents";
    const char* menu_text[2] = {"New Game", "Continue (coming soon)"};
    const char* title_text_0 = "project.";
    const char* title_text_1 = "rpg";
    const char* version_text = g->version.c_str();
    const char* start_text = "Press enter or space to begin";
    char buffer[1024] = {0};
    Color title_text_0_color = {0x66, 0x66, 0x66, 0xFF}, title_text_1_color = {0xFF, 0xFF, 0xFF, 0xFF};
    int sm_font_size = 10;
    int font_size = 40;
    int font_size2 = 20;
    int evidojo_presents_measure = MeasureText(evidojo_presents_text, sm_font_size);
    int measure = MeasureText(title_text_0, font_size);
    int start_measure = MeasureText(start_text, sm_font_size);
    int version_measure = MeasureText(version_text, sm_font_size);
    int padding = 10;
    int w = DEFAULT_TARGET_WIDTH;
    int h = DEFAULT_TARGET_HEIGHT;
    float x = (w - measure) / 2.0f;
    float y = (h - font_size * 2) / 2.0f;
    float start_x = (w - start_measure) / 2.0f;
    float title_text_1_x = x + measure + padding;
    float version_x = (w - version_measure) / 2.0f;
    float version_y = y + font_size + 10;
    float start_y = y + font_size * 1 + 20 + sm_font_size;
    ClearBackground(BLACK);
    // Draw the "evidojo666 presents" text at the top
    int evidojo_x = (w - evidojo_presents_measure) / 2.0f;
    int evidojo_y = y - sm_font_size - 10; // Position it above the title text
    DrawText(evidojo_presents_text, evidojo_x, evidojo_y, sm_font_size, WHITE);
    DrawText(title_text_0, x, y, font_size, title_text_0_color);
    DrawText(title_text_1, title_text_1_x, y, font_size, title_text_1_color);
    DrawText(version_text, version_x, version_y, sm_font_size, WHITE);
    if (!show_menu) {
        DrawText(start_text, start_x, start_y, sm_font_size, WHITE);
        return;
    }
    // If show_menu is true, draw the new game, continue, options selection text
    int menu_count = sizeof(menu_text) / sizeof(menu_text[0]);
    int menu_spacing = 10;
    int current_selection_index = g->title_screen_selection;
    for (int i = 0; i < menu_count; i++) {
        bzero(buffer, sizeof(buffer));
        float menu_x = (w - MeasureText(menu_text[i], sm_font_size)) / 2.0f;
        float menu_y = start_y + (i * (sm_font_size + menu_spacing));
        if (i == current_selection_index) {
            snprintf(buffer, sizeof(buffer), "> %s", menu_text[i]);
        } else {
            snprintf(buffer, sizeof(buffer), "  %s", menu_text[i]);
        }
        selection_color = i == 0 ? active_color : disabled_color; // First item is always active
        DrawText(buffer, menu_x, menu_y, sm_font_size, selection_color);
    }
}
