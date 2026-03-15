/** @file draw_mini_inventory_menu.h
 *  @brief Compact world-adjacent inventory/chest panel drawing helpers.
 */

#pragma once

#include "draw_item_detail_text.h"
#include "gamestate.h"
#include "libdraw_context.h"
#include "spritegroup.h"
#include <algorithm>

/** @brief Compute the on-screen panel rectangle near the hero for the mini inventory UI. */
Rectangle mini_inventory_panel_for_hero(gamestate& g, float width, float height);

/** @brief Draw the compact inventory/chest panel with preview and scrolling list. */
void draw_mini_inventory_menu(gamestate& g, shared_ptr<vector<entityid>> inventory, const char* title, const char* hint, bool show_equipped);
