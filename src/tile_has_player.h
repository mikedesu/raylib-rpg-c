/** @file tile_has_player.h
 *  @brief Helper for checking whether a shared tile currently contains the player.
 */

#pragma once

#include "gamestate.h"
#include "recompute_entity_cache.h"

/** @brief Recompute tile caches and return whether the tile currently contains the player. */
static inline bool tile_has_player(shared_ptr<gamestate> g, shared_ptr<tile_t> t) {
    if (!t)
        return false;
    recompute_entity_cache(g, t);
    return t->cached_player_present;
}
