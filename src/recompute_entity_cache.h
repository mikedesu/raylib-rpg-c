#pragma once

#include "entityid.h"
#include "gamestate.h"

static inline void recompute_entity_cache(gamestate& g, tile_t& t) {
    // Only recompute if cache is dirty
    //minfo("recompute entity cache...");
    if (!t.dirty_entities)
        return;
    // Reset counters
    t.cached_live_npcs = 0;
    t.cached_item_count = 0;
    t.cached_player_present = false;
    t.cached_npc = ENTITYID_INVALID;
    t.cached_item = ENTITYID_INVALID;
    // Iterate through all entities on the tile
    for (size_t i = 0; i < t.entities->size(); i++) {
        const entityid id = t.entities->at(i);
        // Skip dead entities
        if (g.ct.get<dead>(id).value_or(false)) {
            continue;
        }
        // Check entity type
        const entitytype_t type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (type == ENTITY_NPC) {
            t.cached_live_npcs++;
            t.cached_npc = id;
        } else if (type == ENTITY_PLAYER) {
            t.cached_player_present = true;
            t.cached_npc = id;
        } else if (type == ENTITY_ITEM) {
            t.cached_item_count++;
            t.cached_item = id;
        }
    }
    // Cache is now clean
    t.dirty_entities = false;
}


static inline void recompute_entity_cache_at(gamestate& g, int x, int y, int z) {
    massert(x >= 0 && y >= 0 && z >= 0, "x, y, or z is out of bounds: %d, %d, %d", x, y, z);
    massert((size_t)z < g.dungeon.floors.size(), "z is out of bounds");
    auto df = g.dungeon.floors[z];
    auto t = df_tile_at(df, (vec3){x, y, z});
    recompute_entity_cache(g, t);
}
