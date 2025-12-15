#pragma once


#include "gamestate.h"

static inline void recompute_entity_cache(shared_ptr<gamestate> g, shared_ptr<tile_t> t) {
    massert(g && t, "gamestate or tile is NULL");
    // Only recompute if cache is dirty
    if (!t->dirty_entities)
        return;
    // Reset counters
    t->cached_live_npcs = 0;
    t->cached_player_present = false;
    // Iterate through all entities on the tile
    for (size_t i = 0; i < t->entities->size(); i++) {
        entityid id = t->entities->at(i);
        // Skip dead entities
        if (g->ct.get<dead>(id).value_or(true)) {
            continue;
        }
        // Check entity type
        //entitytype_t type = g_get_type(g, id);
        entitytype_t type = g->ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (type == ENTITY_NPC) {
            t->cached_live_npcs++;
        } else if (type == ENTITY_PLAYER) {
            t->cached_player_present = true;
        }
    }
    // Cache is now clean
    t->dirty_entities = false;
}


static inline void recompute_entity_cache_at(shared_ptr<gamestate> g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    massert(x >= 0 && y >= 0 && z >= 0, "x, y, or z is out of bounds: %d, %d, %d", x, y, z);
    massert((size_t)z < g->dungeon->floors->size(), "z is out of bounds");
    shared_ptr<dungeon_floor_t> df = g->dungeon->floors->at(z);
    massert(df, "failed to get dungeon floor");
    //shared_ptr<tile_t> t = df_tile_at(df, (vec3){x, y, z});
    auto t = df_tile_at(df, (vec3){x, y, z});
    massert(t, "tile not found");
    recompute_entity_cache(g, t);
}
