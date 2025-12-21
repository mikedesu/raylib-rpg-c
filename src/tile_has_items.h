#pragma once

#include "gamestate.h"
#include "recompute_entity_cache.h"

static inline bool tile_has_items(gamestate& g, tile_t& t) {
    recompute_entity_cache(g, t);
    return t.cached_item_count > 0;
}
