#pragma once

#include "gamestate.h"

static inline bool tile_has_solid(gamestate& g, int x, int y, int z) {
    massert(z >= 0, "floor is out of bounds");
    massert((size_t)z < g.dungeon.floors.size(), "floor is out of bounds");
    auto df = d_get_floor(g.dungeon, z);
    auto t = df_tile_at(df, (vec3){x, y, z});
    for (int i = 0; (size_t)i < t.entities->size(); i++) {
        const entityid id = tile_get_entity(t, i);
        const bool is_solid = g.ct.get<solid>(id).value_or(false);
        if (id != ENTITYID_INVALID && is_solid)
            return true;
    }
    return false;
}
