//#include "gamestate_equipped_weapon.h"
//#include "gamestate_inventory.h"
#include "ComponentTraits.h"
#include "entityid.h"
#include "libdraw_draw_inventory_menu.h"
#include "libgame_defines.h"
#include "spritegroup.h"
#include <raylib.h>

extern unordered_map<entityid, spritegroup_t*> spritegroups;

void draw_inventory_menu(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    if (!g->display_inventory_menu) {
        return;
    }
    const char* menu_title = "Inventory Menu";
    // Parameters
    int box_pad = g->pad;
    int section_gap = 8;
    int font_size = 10;
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
    DrawRectangleRec(left_box, (Color){0x22, 0x22, 0x22, 0xff});
    DrawRectangleLinesEx(left_box, 2, WHITE);
    DrawRectangleRec(right_box, (Color){0x22, 0x22, 0x22, 0xff});
    DrawRectangleLinesEx(right_box, 2, WHITE);

    //optional<shared_ptr<vector<entityid>>> my_inventory = g->ct.get<inventory>(g->hero_id);
    auto my_inventory = g->ct.get<inventory>(g->hero_id);

    if (my_inventory) {
        // lets start with just one block
        float x = left_box.x + 2;
        float y = left_box.y + 2;
        const int cols = 7;
        const float w = (left_box.width - 4) / cols;
        const int rows = 7;
        const float h = (left_box.height - 4) / rows;


        if (my_inventory.has_value()) {
            // unpack the inventory optional
            //shared_ptr<vector<entityid>> unpacked_inventory = my_inventory.value();
            auto unpacked_inventory = my_inventory.value();

            auto it = unpacked_inventory->begin();

            // for each location in the grid...
            for (int j = 0; j < rows; j++) {
                for (int i = 0; i < cols; i++) {
                    Rectangle grid_box = {x, y, w, h};
                    Rectangle grid_box2 = {x + 2, y + 2, w - 4, h - 4};
                    DrawRectangleLinesEx(grid_box, 1, (Color){0x66, 0x66, 0x66, 255});
                    if (it != unpacked_inventory->end()) {
                        spritegroup_t* sg = spritegroups[*it];
                        if (sg) {
                            auto sprite = sg_get_current(sg);
                            DrawTexturePro(*(sprite->texture), (Rectangle){10, 10, 12, 12}, grid_box2, (Vector2){0, 0}, 0.0f, WHITE);

                            //size_t index = g->inventory_cursor.y * 7 + g->inventory_cursor.x;
                            size_t index = j * cols + i;

                            if (index >= 0 && index < unpacked_inventory->size()) {
                                entityid selection_id = unpacked_inventory->at(index);
                                entityid cur_wpn_id = g->ct.get<equipped_weapon>(g->hero_id).value_or(ENTITYID_INVALID);
                                if (selection_id == cur_wpn_id && cur_wpn_id != ENTITYID_INVALID) {
                                    //DrawText("weapon", grid_box2.x, grid_box2.y + grid_box2.height - 10, 10, WHITE);
                                    DrawText("equipped", grid_box2.x, grid_box2.y + grid_box2.height - 10, 10, WHITE);
                                }
                            }
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

            // in the right box, item detail
            if (unpacked_inventory->size() > 0) {
                size_t index = g->inventory_cursor.y * 7 + g->inventory_cursor.x;
                if (index >= 0 && index < unpacked_inventory->size()) {
                    entityid selection_id = unpacked_inventory->at(index);
                    spritegroup_t* sg = spritegroups[selection_id];
                    if (sg) {
                        auto sprite = sg_get_current(sg);
                        DrawTexturePro(*(sprite->texture), (Rectangle){0, 0, 32, 32}, right_box, (Vector2){0, 0}, 0.0f, WHITE);

                        // new-style component table access
                        string my_name = g->ct.get<name>(selection_id).value_or("no-name");
                        DrawText(my_name.c_str(), right_box.x + 10, right_box.y + 10, 20, WHITE);
                    }
                }
            }
        }
    }
}
