/** @file draw_help_menu.h
 *  @brief Help/controls reference modal drawing helper.
 */

#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

/** @brief Draw the full help modal using the active keyboard profile labels. */
static inline void draw_help_menu(gamestate& g) {
    const string text = TextFormat(
        "Help Menu\n"
        "\n"
        "Profile: %s\n"
        "Move Up: %s\n"
        "Move Down: %s\n"
        "Move Left / Right: %s / %s\n"
        "Move Diagonals: %s %s %s %s\n"
        "Face Mode: %s\n"
        "Face Attack: %s\n"
        "Camera mode: %s\n"
        "Zoom: %s and %s\n"
        "\n"
        "Pick up item: %s\n"
        "Attack: %s\n"
        "Pull: %s\n"
        "Open door / chest: %s\n"
        "Interact / examine: %s\n"
        "Use stairs: %s\n"
        "Toggle full light: %s\n"
        "Level-up select: arrows + enter\n"
        "\n"
        "Inventory: %s\n"
        "Inventory equip/use: e or enter\n"
        "Inventory drop: q\n"
        "Inventory close: esc or inventory key\n"
        "\n"
        "Options: %s\n"
        "Open this help menu: %s\n"
        "Debug panel: p\n"
        "Quit prompt: esc\n"
        "\n"
        "@evildojo666",
        keyboard_profile_label(g.keyboard_profile),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_UP).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_DOWN).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_LEFT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_RIGHT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_UP_LEFT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_UP_RIGHT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_DOWN_LEFT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_DOWN_RIGHT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_DIRECTION_MODE).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_FACE_ATTACK).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_CAMERA_TOGGLE).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_ZOOM_IN).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_ZOOM_OUT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_PICKUP).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_ATTACK).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_PULL).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_OPEN).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_INTERACT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_STAIRS).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_TOGGLE_FULL_LIGHT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_INVENTORY).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_OPTIONS).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_HELP).c_str());

    constexpr int font_size = 20;
    constexpr int line_spacing = 4;
    const int padding = DEFAULT_PAD * 2;

    int max_text_width = 0;
    int line_count = 0;
    size_t line_start = 0;
    while (line_start <= text.size()) {
        const size_t line_end = text.find('\n', line_start);
        const string line = line_end == string::npos ? text.substr(line_start) : text.substr(line_start, line_end - line_start);
        const int line_width = MeasureText(line.c_str(), font_size);
        if (line_width > max_text_width) {
            max_text_width = line_width;
        }
        line_count++;
        if (line_end == string::npos) {
            break;
        }
        line_start = line_end + 1;
    }

    const int text_height = line_count * font_size + (line_count - 1) * line_spacing;

    const int box_width = max_text_width + padding * 2;
    const int box_height = text_height + padding * 2;
    const int box_x = (DEFAULT_TARGET_WIDTH - box_width) / 2;
    const int box_y = (DEFAULT_TARGET_HEIGHT - box_height) / 2;

    DrawRectangle(box_x, box_y, box_width, box_height, g.window_box_bgcolor);

    const float box_x_f = box_x;
    const float box_y_f = box_y;
    const float box_w_f = box_width;
    const float box_h_f = box_height;
    constexpr int thick = 2;
    DrawRectangleLinesEx({box_x_f, box_y_f, box_w_f, box_h_f}, thick, g.window_box_fgcolor);

    const int text_x = box_x + padding;
    int text_y = box_y + padding;
    line_start = 0;
    while (line_start <= text.size()) {
        const size_t line_end = text.find('\n', line_start);
        const string line = line_end == string::npos ? text.substr(line_start) : text.substr(line_start, line_end - line_start);
        DrawText(line.c_str(), text_x, text_y, font_size, g.window_box_fgcolor);
        text_y += font_size + line_spacing;
        if (line_end == string::npos) {
            break;
        }
        line_start = line_end + 1;
    }



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
