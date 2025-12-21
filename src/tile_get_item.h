#pragma once

#include "gamestate.h"
#include "recompute_entity_cache.h"

static inline entityid tile_get_item(gamestate& g, tile_t& t) {
    recompute_entity_cache(g, t);
    return t.cached_item;
}
