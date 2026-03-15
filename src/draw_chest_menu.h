/** @file draw_chest_menu.h
 *  @brief Chest/inventory transfer menu drawing helpers.
 */

#pragma once

#include "draw_item_detail_text.h"
#include "gamestate.h"
#include "libdraw_context.h"
#include "spritegroup.h"

/** @brief Draw the right-side detail panel for the selected chest item. */
static inline void draw_item_detail_panel(gamestate& g, const Rectangle& right_box, entityid selection_id) {
    spritegroup* sg = libdraw_ctx.spritegroups[selection_id];
    if (!sg) {
        return;
    }

    auto sprite = sg->get_current();
    DrawTexturePro(*(sprite->get_texture()), (Rectangle){0, 0, 32, 32}, right_box, (Vector2){0, 0}, 0.0f, WHITE);

    const int fontsize = 20;
    const int cur_x = right_box.x + 10;
    int cur_y = right_box.y + 10;
    const int y_incr = 20;
    const vector<string> lines = build_item_detail_lines(g, selection_id);
    for (const string& line : lines) {
        DrawText(line.c_str(), cur_x, cur_y, fontsize, g.window_box_fgcolor);
        cur_y += y_incr;
    }
}

/** @brief Draw the left-side grid of item sprites for an inventory view. */
static inline void draw_inventory_grid(gamestate& g, shared_ptr<vector<entityid>> inventory, const Rectangle& left_box, bool show_equipped_labels) {
    float x = left_box.x + 2;
    float y = left_box.y + 2;
    const int cols = 7;
    const int rows = 7;
    const float w = (left_box.width - 4) / cols;
    const float h = (left_box.height - 4) / rows;
    auto it = inventory->begin();
    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            Rectangle grid_box = {x, y, w, h};
            Rectangle grid_box2 = {x + 2, y + 2, w - 4, h - 4};
            DrawRectangleLinesEx(grid_box, 1, (Color){0x66, 0x66, 0x66, 255});
            if (it != inventory->end()) {
                spritegroup* sg = libdraw_ctx.spritegroups[*it];
                if (sg) {
                    auto sprite = sg->get_current();
                    DrawTexturePro(*(sprite->get_texture()), Rectangle{10, 10, 12, 12}, grid_box2, Vector2{0, 0}, 0.0f, WHITE);
                    if (show_equipped_labels) {
                        size_t index = static_cast<size_t>(j * cols + i);
                        if (index < inventory->size()) {
                            entityid selection_id = inventory->at(index);
                            entityid cur_wpn_id = g.ct.get<equipped_weapon>(g.hero_id).value_or(ENTITYID_INVALID);
                            entityid cur_shield_id = g.ct.get<equipped_shield>(g.hero_id).value_or(ENTITYID_INVALID);
                            bool cur_wpn_selected = selection_id == cur_wpn_id && cur_wpn_id != ENTITYID_INVALID;
                            bool cur_shield_selected = selection_id == cur_shield_id && cur_shield_id != ENTITYID_INVALID;
                            if (cur_wpn_selected || cur_shield_selected) {
                                DrawText("equipped", grid_box2.x, grid_box2.y + grid_box2.height - 10, 10, g.window_box_fgcolor);
                            }
                        }
                    }
                }
                ++it;
            }
            if ((float)i == g.inventory_cursor.x && (float)j == g.inventory_cursor.y) {
                DrawRectangleLinesEx(grid_box, 2, GREEN);
            }
            x += w;
        }
        x = left_box.x + 2;
        y += h;
    }
}

/** @brief Draw the full chest transfer menu and selected-item detail view. */
static inline void draw_chest_menu(gamestate& g) {
    if (!g.display_chest_menu || g.active_chest_id == ENTITYID_INVALID) {
        return;
    }
    auto maybe_inventory = g.ct.get<inventory>(g.chest_deposit_mode ? g.hero_id : g.active_chest_id);
    if (!maybe_inventory.has_value()) {
        return;
    }
    auto inventory = maybe_inventory.value();
    const char* menu_title = g.chest_deposit_mode ? "Treasure Chest - Deposit" : "Treasure Chest";
    const char* hint_text = g.chest_deposit_mode ? "TAB: chest view  ENTER: deposit  ESC/D: close" : "TAB: hero inventory  ENTER: take  ESC/D: close";
    const int section_gap = 8;
    const int font_size = 10;
    const Vector2 title_size = MeasureTextEx(GetFontDefault(), menu_title, font_size, g.line_spacing);
    const int w = DEFAULT_TARGET_WIDTH;
    const int h = DEFAULT_TARGET_HEIGHT;
    const float menu_width = w * 0.75f;
    const float menu_height = h * 0.75f;
    Rectangle menu_box = {(w - menu_width) / 2.0f, (h - menu_height) / 4.0f, menu_width, menu_height};
    float title_x = menu_box.x + (menu_box.width - title_size.x) / 2.0f;
    float title_y = menu_box.y + g.pad;
    float half_width = (menu_box.width - section_gap) / 2.0f;
    float half_height = menu_box.height - title_size.y - g.pad * 3.0f;

    DrawRectangleRec(menu_box, g.window_box_bgcolor);
    DrawRectangleLinesEx(menu_box, 2, g.window_box_fgcolor);
    DrawText(menu_title, title_x, title_y, font_size, g.window_box_fgcolor);
    DrawText(hint_text, menu_box.x + g.pad, menu_box.y + menu_box.height - g.pad, font_size, g.window_box_fgcolor);

    Rectangle left_box = {menu_box.x + g.pad, title_y + title_size.y + g.pad, half_width - g.pad, half_height};
    Rectangle right_box = {left_box.x + half_width + section_gap, left_box.y, half_width - g.pad * 2, half_height};
    DrawRectangleRec(left_box, g.window_box_bgcolor);
    DrawRectangleLinesEx(left_box, 2, g.window_box_fgcolor);
    DrawRectangleRec(right_box, g.window_box_bgcolor);
    DrawRectangleLinesEx(right_box, 2, g.window_box_fgcolor);

    draw_inventory_grid(g, inventory, left_box, g.chest_deposit_mode);

    if (!inventory->empty()) {
        size_t index = static_cast<size_t>(g.inventory_cursor.y) * 7 + static_cast<size_t>(g.inventory_cursor.x);
        if (index < inventory->size()) {
            draw_item_detail_panel(g, right_box, inventory->at(index));
        }
    }
}
