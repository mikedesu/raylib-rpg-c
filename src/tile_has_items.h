/** @file tile_has_items.h
 *  @brief Helper for checking whether a tile currently holds cached items.
 */

#pragma once

#include "gamestate.h"
#include "recompute_entity_cache.h"

/** @brief Recompute tile caches and return whether the tile currently has any items. */
static inline bool tile_has_items(gamestate& g, tile_t& t) {
    recompute_entity_cache(g, t);
    return t.get_item_count() > 0;
}
