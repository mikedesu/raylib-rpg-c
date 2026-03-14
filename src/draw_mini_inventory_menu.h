/** @file draw_mini_inventory_menu.h
 *  @brief Compact world-adjacent inventory/chest panel drawing helpers.
 */

#pragma once

#include "draw_item_detail_text.h"
#include "gamestate.h"
#include "spritegroup.h"
#include <algorithm>

extern unordered_map<entityid, spritegroup*> spritegroups;

/** @brief Compute the on-screen panel rectangle near the hero for the mini inventory UI. */
static inline Rectangle mini_inventory_panel_for_hero(gamestate& g, float width, float height) {
    const vec3 hero_loc = g.ct.get<location>(g.hero_id).value_or(vec3{0, 0, 0});
    const Vector2 hero_screen = GetWorldToScreen2D(
        Vector2{
            static_cast<float>(hero_loc.x * DEFAULT_TILE_SIZE) + DEFAULT_TILE_SIZE * 0.5f,
            static_cast<float>(hero_loc.y * DEFAULT_TILE_SIZE),
        },
        g.cam2d);
    const float margin = 12.0f;
    const float x = std::clamp(hero_screen.x + 36.0f, margin, g.targetwidth - width - margin);
    const float y = std::clamp(hero_screen.y - height * 0.5f, margin, g.targetheight - height - margin);
    return Rectangle{x, y, width, height};
}

/** @brief Draw the compact inventory/chest panel with preview and scrolling list. */
static inline void draw_mini_inventory_menu(gamestate& g, shared_ptr<vector<entityid>> inventory, const char* title, const char* hint, bool show_equipped) {
    const float width = 260.0f;
    const float row_h = 18.0f;
    const float preview_h = 118.0f;
    const float padding = 10.0f;
    const float footer_h = 18.0f;
    const size_t visible_count = std::max(1U, g.mini_inventory_visible_count);
    const float list_h = row_h * static_cast<float>(visible_count);
    const Rectangle panel = mini_inventory_panel_for_hero(g, width, padding * 3.0f + 14.0f + list_h + preview_h + footer_h + 12.0f);
    DrawRectangleRec(panel, g.window_box_bgcolor);
    DrawRectangleLinesEx(panel, 2, g.window_box_fgcolor);
    DrawText(title, static_cast<int>(panel.x + padding), static_cast<int>(panel.y + padding), 12, g.window_box_fgcolor);

    const float hint_y = panel.y + panel.height - footer_h;
    DrawText(hint, static_cast<int>(panel.x + padding), static_cast<int>(hint_y), 10, g.window_box_fgcolor);

    const Rectangle list_box = {panel.x + padding, panel.y + 28.0f, panel.width - padding * 2.0f, list_h};
    DrawRectangleLinesEx(list_box, 1, g.window_box_fgcolor);

    const size_t selected_index = g.get_inventory_selection_index();
    const size_t scroll = g.mini_inventory_scroll_offset;
    for (size_t i = 0; i < visible_count; i++) {
        const float row_y = list_box.y + row_h * static_cast<float>(i);
        const Rectangle row = {list_box.x, row_y, list_box.width, row_h};
        const size_t item_index = scroll + i;
        if (item_index == selected_index && item_index < inventory->size()) {
            DrawRectangleRec(row, Color{255, 255, 255, 28});
            DrawRectangleLinesEx(row, 1, YELLOW);
        }
        if (item_index >= inventory->size()) {
            continue;
        }

        const entityid item_id = inventory->at(item_index);
        const string item_name = g.ct.get<name>(item_id).value_or("no-name");
        const bool equipped =
            show_equipped &&
            (item_id == g.ct.get<equipped_weapon>(g.hero_id).value_or(ENTITYID_INVALID) ||
             item_id == g.ct.get<equipped_shield>(g.hero_id).value_or(ENTITYID_INVALID));
        DrawText(
            TextFormat("%s%s", item_index == selected_index ? "> " : "  ", item_name.c_str()),
            static_cast<int>(row.x + 6),
            static_cast<int>(row.y + 4),
            10,
            item_index == selected_index ? YELLOW : g.window_box_fgcolor);
        if (equipped) {
            DrawText("E", static_cast<int>(row.x + row.width - 14), static_cast<int>(row.y + 4), 10, g.window_box_fgcolor);
        }
    }

    if (scroll > 0) {
        DrawText("^", static_cast<int>(list_box.x + list_box.width - 12), static_cast<int>(list_box.y - 10), 10, g.window_box_fgcolor);
    }
    if (scroll + visible_count < inventory->size()) {
        DrawText("v", static_cast<int>(list_box.x + list_box.width - 12), static_cast<int>(list_box.y + list_box.height), 10, g.window_box_fgcolor);
    }

    if (inventory->empty() || selected_index >= inventory->size()) {
        DrawText("empty", static_cast<int>(panel.x + padding), static_cast<int>(list_box.y + list_box.height + 10), 10, g.window_box_fgcolor);
        return;
    }

    const entityid selection_id = inventory->at(selected_index);
    const Rectangle preview = {panel.x + padding, list_box.y + list_box.height + 10.0f, panel.width - padding * 2.0f, preview_h};
    DrawRectangleLinesEx(preview, 1, g.window_box_fgcolor);
    spritegroup* sg = spritegroups[selection_id];
    const float preview_sprite_size = 72.0f;
    if (sg) {
        auto sprite = sg->get_current();
        DrawTexturePro(
            *(sprite->get_texture()),
            Rectangle{0, 0, 32, 32},
            Rectangle{preview.x + 8, preview.y + 8, preview_sprite_size, preview_sprite_size},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }
    const vector<string> lines = build_item_detail_lines(g, selection_id);
    const float text_x = preview.x + preview_sprite_size + 18.0f;
    float text_y = preview.y + 8.0f;
    for (size_t i = 0; i < lines.size() && i < 3; i++) {
        DrawText(lines[i].c_str(), static_cast<int>(text_x), static_cast<int>(text_y), i == 0 ? 11 : 10, g.window_box_fgcolor);
        text_y += 20.0f;
    }
    if (!lines.empty()) {
        DrawText(lines.back().c_str(), static_cast<int>(preview.x + 8), static_cast<int>(preview.y + preview_sprite_size + 14.0f), 10, g.window_box_fgcolor);
    }
}
