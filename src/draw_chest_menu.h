/** @file draw_chest_menu.h
 *  @brief Chest/inventory transfer menu drawing helpers.
 */

#pragma once

#include "draw_item_detail_text.h"
#include "gamestate.h"
#include "libdraw_context.h"
#include "spritegroup.h"

/** @brief Draw the right-side detail panel for the selected chest item. */
void draw_item_detail_panel(gamestate& g, const Rectangle& right_box, entityid selection_id);

/** @brief Draw the left-side grid of item sprites for an inventory view. */
void draw_inventory_grid(gamestate& g, shared_ptr<vector<entityid>> inventory, const Rectangle& left_box, bool show_equipped_labels);

/** @brief Draw the full chest transfer menu and selected-item detail view. */
void draw_chest_menu(gamestate& g);
