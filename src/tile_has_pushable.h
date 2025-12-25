#pragma once

#include "gamestate.h"

static inline entityid tile_has_pushable(gamestate& g, int x, int y, int z) {
    massert(z >= 0, "floor is out of bounds");
    massert((size_t)z < g.dungeon.floors.size(), "floor is out of bounds");
    auto df = d_get_floor(g.dungeon, z);
    auto t = df_tile_at(df, (vec3){x, y, z});
    for (int i = 0; (size_t)i < t.entities->size(); i++) {
        const entityid id = tile_get_entity(t, i);
        const bool is_pushable = g.ct.get<pushable>(id).value_or(false);
        if (id != ENTITYID_INVALID && is_pushable)
            return id;
    }
    return ENTITYID_INVALID;
}
