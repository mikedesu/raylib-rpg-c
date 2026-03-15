/** @file draw_inventory_menu.h
 *  @brief Full inventory menu drawing helper.
 */

#pragma once

#include "draw_item_detail_text.h"
#include "gamestate.h"
#include "libdraw_context.h"
#include "spritegroup.h"

/** @brief Draw the full inventory grid and selected-item detail panel. */
static inline void draw_inventory_menu(gamestate& g) {
    if (!g.display_inventory_menu) return;
    const char* menu_title = "Inventory Menu";
    // Parameters
    const int section_gap = 8, font_size = 10;
    // Measure title
    const Vector2 title_size = MeasureTextEx(GetFontDefault(), menu_title, font_size, g.line_spacing);
    // Menu box size
    const float menu_width_percent = 0.75f;
    const float menu_height_percent = 0.75f;
    const int w = DEFAULT_TARGET_WIDTH;
    const int h = DEFAULT_TARGET_HEIGHT;
    const float menu_width = w * menu_width_percent;
    const float menu_height = h * menu_height_percent;
    Rectangle menu_box = {(w - menu_width) / 2.0f, (h - menu_height) / 4.0f, menu_width, menu_height};
    float title_x = menu_box.x + (menu_box.width - title_size.x) / 2.0f;
    float title_y = menu_box.y + g.pad;
    float half_width = (menu_box.width - section_gap) / 2.0f;
    float half_height = menu_box.height - title_size.y - g.pad * 2.0f - g.pad;
    // Draw menu background and border
    DrawRectangleRec(menu_box, g.window_box_bgcolor);
    DrawRectangleLinesEx(menu_box, 2, g.window_box_fgcolor);
    // Draw menu title centered at top
    DrawText(menu_title, title_x, title_y, font_size, g.window_box_fgcolor);
    // Partition into left/right halves (with gap)
    // Left box: inventory list
    // Right box: item info
    Rectangle left_box = {menu_box.x + g.pad, title_y + title_size.y + g.pad, half_width - g.pad, half_height};
    Rectangle right_box = {left_box.x + half_width + section_gap, left_box.y, half_width - g.pad * 2, half_height};
    // Draw left and right boxes
    DrawRectangleRec(left_box, g.window_box_bgcolor);
    DrawRectangleLinesEx(left_box, 2, g.window_box_fgcolor);
    DrawRectangleRec(right_box, g.window_box_bgcolor);
    DrawRectangleLinesEx(right_box, 2, g.window_box_fgcolor);
    auto my_inventory = g.ct.get<inventory>(g.hero_id);
    //if (my_inventory == nullopt)
    //    return;
    if (my_inventory) {
        // lets start with just one block
        float x = left_box.x + 2;
        float y = left_box.y + 2;
        const int cols = 7;
        const float w = (left_box.width - 4) / cols;
        const int rows = 7;
        const float h = (left_box.height - 4) / rows;
        if (!my_inventory.has_value()) return;
        //if (my_inventory.has_value()) {
        // unpack the inventory optional
        auto unpacked_inventory = my_inventory.value();
        auto it = unpacked_inventory->begin();
        // for each location in the grid...
        for (int j = 0; j < rows; j++) {
            for (int i = 0; i < cols; i++) {
                Rectangle grid_box = {x, y, w, h};
                Rectangle grid_box2 = {x + 2, y + 2, w - 4, h - 4};
                DrawRectangleLinesEx(grid_box, 1, (Color){0x66, 0x66, 0x66, 255});
                if (it != unpacked_inventory->end()) {
                    spritegroup* sg = spritegroups[*it];
                    if (!sg) continue;
                    auto sprite = sg->get_current();
                    DrawTexturePro(*(sprite->get_texture()), Rectangle{10, 10, 12, 12}, grid_box2, Vector2{0, 0}, 0.0f, WHITE);
                    size_t index = j * cols + i;
                    if (index >= 0 && index < unpacked_inventory->size()) {
                        entityid selection_id = unpacked_inventory->at(index);
                        entityid cur_wpn_id = g.ct.get<equipped_weapon>(g.hero_id).value_or(ENTITYID_INVALID);
                        entityid cur_shield_id = g.ct.get<equipped_shield>(g.hero_id).value_or(ENTITYID_INVALID);
                        bool has_wpn = cur_wpn_id != ENTITYID_INVALID;
                        bool has_shield = cur_shield_id != ENTITYID_INVALID;
                        bool cur_wpn_selected = selection_id == cur_wpn_id;
                        bool cur_shield_selected = selection_id == cur_shield_id;
                        if ((cur_wpn_selected && has_wpn) || (cur_shield_selected && has_shield)) 
                            DrawText("equipped", grid_box2.x, grid_box2.y + grid_box2.height - 10, 10, g.window_box_fgcolor);
                    }
                    it++;
                }
                if ((float)i == g.inventory_cursor.x && (float)j == g.inventory_cursor.y) {
                    DrawRectangleLinesEx(grid_box, 2, GREEN);
                }
                x += w;
            }
            x = left_box.x + 2;
            y += h;
        }

        // in the right box, item detail
        if (unpacked_inventory->size() > 0) {
            size_t index = g.inventory_cursor.y * 7 + g.inventory_cursor.x;
            if (index >= 0 && index < unpacked_inventory->size()) {
                entityid selection_id = unpacked_inventory->at(index);
                spritegroup* sg = spritegroups[selection_id];

                if (sg) {
                    //auto sprite = sg_get_current(sg);
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
            }
        }
    }
}
