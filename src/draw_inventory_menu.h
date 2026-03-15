/** @file draw_inventory_menu.h
 *  @brief Full inventory menu drawing helper.
 */

#pragma once

#include "draw_item_detail_text.h"
#include "gamestate.h"
#include "libdraw_context.h"
#include "spritegroup.h"

/** @brief Draw the full inventory grid and selected-item detail panel. */
void draw_inventory_menu(gamestate& g);
