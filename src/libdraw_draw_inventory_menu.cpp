#include "gamestate_inventory.h"
#include "libdraw_draw_inventory_menu.h"
#include "libgame_defines.h"
#include "spritegroup.h"
#include <raylib.h>

extern unordered_map<entityid, spritegroup_t*> spritegroups2;

void draw_inventory_menu(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    if (!g->display_inventory_menu) {
        return;
    }
    const char* menu_title = "Inventory Menu";
    // Parameters
    int box_pad = g->pad;
    int section_gap = 8;
    int item_list_pad = g->pad;
    int font_size = 10;
    int scale = 4;
    // Measure title
    Vector2 title_size = MeasureTextEx(GetFontDefault(), menu_title, font_size, g->line_spacing);
    // Menu box size
    float menu_width_percent = 0.75f;
    float menu_height_percent = 0.75f;
    int w = DEFAULT_TARGET_WIDTH;
    int h = DEFAULT_TARGET_HEIGHT;
    float menu_width = w * menu_width_percent;
    float menu_height = h * menu_height_percent;
    Rectangle menu_box = {(w - menu_width) / 2.0f, (h - menu_height) / 4.0f, menu_width, menu_height};
    float title_x = menu_box.x + (menu_box.width - title_size.x) / 2.0f;
    float title_y = menu_box.y + box_pad;
    float half_width = (menu_box.width - section_gap) / 2.0f;
    float half_height = menu_box.height - title_size.y - box_pad * 2.0f - box_pad;
    const char* info_title = "Item Info:";
    char info_text[256] = {0};
    spritegroup_t* sg = NULL;
    // Draw menu background and border
    DrawRectangleRec(menu_box, (Color){0x33, 0x33, 0x33, 0x99});
    DrawRectangleLinesEx(menu_box, 2, WHITE);
    // Draw menu title centered at top
    DrawText(menu_title, title_x, title_y, font_size, WHITE);
    // Partition into left/right halves (with gap)
    // Left box: inventory list
    // Right box: item info
    Rectangle left_box = {menu_box.x + box_pad, title_y + title_size.y + box_pad, half_width - box_pad, half_height};
    Rectangle right_box = {left_box.x + half_width + section_gap, left_box.y, half_width - box_pad * 2, half_height};
    // Draw left and right boxes
    float item_y = left_box.y + item_list_pad;
    float info_title_y = right_box.y + item_list_pad;
    float info_text_y = info_title_y + font_size + 8;
    DrawRectangleRec(left_box, (Color){0x22, 0x22, 0x22, 0xff});
    DrawRectangleLinesEx(left_box, 2, WHITE);
    DrawRectangleRec(right_box, (Color){0x22, 0x22, 0x22, 0xff});
    DrawRectangleLinesEx(right_box, 2, WHITE);

    auto inventory = g_get_inventory(g, g->hero_id);
    if (inventory) {
        // lets start with just one block
        float x = left_box.x + 2;
        float y = left_box.y + 2;
        int cols = 7;
        const float w = (left_box.width - 4) / cols;
        int rows = 7;
        const float h = (left_box.height - 4) / rows;

        auto it = inventory->begin();

        //Vector2 inventory_cursor = {0, 0};

        for (int j = 0; j < rows; j++) {
            for (int i = 0; i < cols; i++) {
                Rectangle grid_box = {x, y, w, h};
                Rectangle grid_box2 = {x + 2, y + 2, w - 4, h - 4};
                DrawRectangleLinesEx(grid_box, 1, (Color){0x66, 0x66, 0x66, 255});

                if (it != inventory->end()) {
                    spritegroup_t* sg = spritegroups2[*it];
                    if (sg) {
                        auto sprite = sg_get_current(sg);
                        //DrawTexturePro(*(sprite->texture), (Rectangle){12, 12, 8, 8}, grid_box2, (Vector2){0, 0}, 0.0f, WHITE);
                        DrawTexturePro(*(sprite->texture), (Rectangle){10, 10, 12, 12}, grid_box2, (Vector2){0, 0}, 0.0f, WHITE);
                    }
                    it++;
                }

                if ((float)i == g->inventory_cursor.x && (float)j == g->inventory_cursor.y) {
                    DrawRectangleLinesEx(grid_box, 2, GREEN);
                }

                x += w;
            }
            x = left_box.x + 2;
            y += h;
        }
    }


    //size_t inventory_count = 0;
    //entityid* inventory = g_get_inventory(g, g->hero_id, &inventory_count);
    //massert(inventory, "inventory is NULL");
    //if (inventory == NULL) {
    //    merror("inventory is NULL");
    //    return;
    //}
    // Calculate viewport bounds based on selection
    //size_t max_visible_items = 15;
    //int start_index = 0;
    // If selection is below middle, scroll down
    //if ((size_t)g->inventory_menu_selection > max_visible_items / 2 &&
    //    inventory_count > max_visible_items) {
    //    start_index = g->inventory_menu_selection - max_visible_items / 2;
    //    // Don't scroll past end of inventory
    //    if ((size_t)start_index + max_visible_items > inventory_count) {
    //        start_index = inventory_count - max_visible_items;
    //    }
    //}
    // Draw visible items
    // At the top before the list begins, lets display the current selection number and how many items are in the inventory
    // example: "Item 1 of 20 selected"
    //float item_x = left_box.x + item_list_pad;
    //if (inventory_count > 0) {
    //    char selection_info[64] = {0};
    //    snprintf(selection_info,
    //             sizeof(selection_info),
    //             "Item %d of %zu selected",
    //             g->inventory_menu_selection + 1,
    //             inventory_count);
    //    DrawTextEx(GetFontDefault(),
    //               selection_info,
    //               (Vector2){item_x, item_y},
    //               font_size,
    //               g->line_spacing,
    //               WHITE);
    //} else {
    //    DrawTextEx(
    //        GetFontDefault(),
    //        "No items in inventory",
    //        (Vector2){left_box.x + item_list_pad, left_box.y + item_list_pad},
    //        font_size,
    //        g->line_spacing,
    //        WHITE);
    //}
    //item_y += font_size + 4;
    //for (size_t i = start_index;
    //     i < inventory_count && i < start_index + max_visible_items;
    //     i++) {
    //    entityid item_id = inventory[i];
    //    if (item_id == ENTITYID_INVALID) continue;
    //    char item_display[128];
    //    bool is_equipped = g_is_equipped(g, g->hero_id, item_id);
    //    // Highlight selected item with arrow
    //    //const char* item_name = g_get_name(g, item_id);
    //    string item_name = g_get_name(g, item_id);
    //    if (i == (size_t)g->inventory_menu_selection) {
    //        snprintf(
    //            item_display, sizeof(item_display), "> %s", item_name.c_str());
    //        // Draw selection highlight background
    //        DrawRectangle(left_box.x,
    //                      item_y - 2,
    //                      left_box.width,
    //                      font_size + 4,
    //                      (Color){0x44, 0x44, 0x44, 0xFF});
    //    } else {
    //        //snprintf(item_display, sizeof(item_display), "  %s", g_get_name(g, item_id));
    //        snprintf(
    //            item_display, sizeof(item_display), "  %s", item_name.c_str());
    //    }
    //    if (is_equipped)
    //        strncat(item_display,
    //                " (Equipped)",
    //                sizeof(item_display) - strlen(item_display) - 1);
    //    DrawTextEx(GetFontDefault(),
    //               item_display,
    //               (Vector2){item_x, item_y},
    //               font_size,
    //               g->line_spacing,
    //               WHITE);
    //    item_y += font_size + 4;
    //}
    // Draw item info in right_box
    //if (g->inventory_menu_selection >= 0 &&
    //    (size_t)g->inventory_menu_selection < inventory_count) {
    //    entityid item_id = inventory[g->inventory_menu_selection];
    //    //const char* name = g_get_name(g, item_id);
    //    string name = g_get_name(g, item_id);
    //    itemtype item_type = g_get_itemtype(g, item_id);
    //    if (g_has_damage(g, item_id)) {
    //        vec3 dmg_roll = g_get_damage(g, item_id);
    //        int n = dmg_roll.x, sides = dmg_roll.y, modifier = dmg_roll.z;
    //        if (modifier)
    //            snprintf(info_text,
    //                     sizeof(info_text),
    //                     "%s\nType: %d\nDamage: %dd%d+%d",
    //                     name.c_str(),
    //                     item_type,
    //                     n,
    //                     sides,
    //                     modifier);
    //        else
    //            snprintf(info_text,
    //                     sizeof(info_text),
    //                     "%s\nType: %d\nDamage: %dd%d",
    //                     name.c_str(),
    //                     item_type,
    //                     n,
    //                     sides);
    //    } else if (g_has_ac(g, item_id)) {
    //        int ac = g_get_ac(g, item_id);
    //        snprintf(info_text,
    //                 sizeof(info_text),
    //                 "%s\nType: %d\nAC: %d",
    //                 name.c_str(),
    //                 item_type,
    //                 ac);
    //    } else {
    //        snprintf(info_text,
    //                 sizeof(info_text),
    //                 "%s\nType: %d",
    //                 name.c_str(),
    //                 item_type);
    //    }
    //    sg = hashtable_entityid_spritegroup_get(spritegroups, item_id);
    //} else
    //    snprintf(info_text,
    //             sizeof(info_text),
    //             "Select an item to view details here.");
    //DrawTextEx(GetFontDefault(),
    //           info_title,
    //           (Vector2){right_box.x + item_list_pad, info_title_y},
    //           font_size,
    //           g->line_spacing,
    //           (Color){0xaa, 0xaa, 0xaa, 0xff});
    //DrawTextEx(GetFontDefault(),
    //           info_text,
    //           (Vector2){right_box.x + item_list_pad, info_text_y},
    //           font_size,
    //           g->line_spacing,
    //           WHITE);
    //if (sg) {
    //    sprite* s = sg_get_current(sg);
    //    if (s) {
    //        float sprite_width = s->width * scale,
    //              sprite_height = s->height * scale, sprite_margin = -6 * scale;
    //        // Anchor to top-right of right_box, account for margin
    //        const float sprite_x = right_box.x + right_box.width -
    //                               sprite_margin - sprite_width,
    //                    sprite_y = right_box.y + sprite_margin;
    //        DrawTexturePro(
    //            *s->texture,
    //            s->src,
    //            (Rectangle){sprite_x, sprite_y, sprite_width, sprite_height},
    //            (Vector2){0, 0}, // Top-left corner as origin
    //            0.0f,
    //            WHITE);
    //    }
    //}
}
