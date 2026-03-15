/** @file draw_item_detail_text.h
 *  @brief Helper for building item-detail text lines for UI panels.
 */

#pragma once

#include "gamestate.h"
#include <vector>

/** @brief Build the display lines for an item's detail/preview panel. */
vector<string> build_item_detail_lines(gamestate& g, entityid selection_id);
