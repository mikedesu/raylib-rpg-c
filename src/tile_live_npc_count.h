#pragma once

#include "gamestate.h"
#include "recompute_entity_cache.h"
#include <cstddef>

static inline size_t tile_live_npc_count(shared_ptr<gamestate> g, shared_ptr<tile_t> t) {
    recompute_entity_cache(g, t);
    return t->cached_live_npcs;
}


static inline size_t tile_live_npc_count_at(shared_ptr<gamestate> g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    massert(x >= 0 && y >= 0 && z >= 0, "x, y, or z is out of bounds: %d, %d, %d", x, y, z);
    recompute_entity_cache_at(g, x, y, z);
    shared_ptr<dungeon_t> d = g->dungeon;
    massert(d, "failed to get dungeon");
    shared_ptr<dungeon_floor_t> df = d_get_floor(d, z);
    massert(df, "failed to get dungeon floor");
    massert(x < df->width, "x is out of bounds");
    massert(y < df->height, "y is out of bounds");
    shared_ptr<tile_t> t = df_tile_at(df, (vec3){x, y, z});
    massert(t, "failed to get tile");
    return tile_live_npc_count(g, t);
}
