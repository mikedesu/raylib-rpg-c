#pragma once

#include "gamestate.h"
#include "recompute_entity_cache.h"

static inline bool tile_has_live_npcs(shared_ptr<gamestate> g, shared_ptr<tile_t> t) {
    if (!t)
        return false;
    recompute_entity_cache(g, t);
    return t->cached_live_npcs > 0;
}
