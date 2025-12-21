#pragma once

#include "gamestate.h"
#include "recompute_entity_cache.h"

static inline entityid get_cached_npc(gamestate& g, tile_t& t) {
    recompute_entity_cache(g, t); // Force update
    return t.cached_npc;
}
