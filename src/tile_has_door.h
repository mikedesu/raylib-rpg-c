#pragma once

#include "gamestate.h"

static inline entityid tile_has_door(shared_ptr<gamestate> g, vec3 v) {
    massert(g, "Game state is NULL!");

    minfo("tile_has_door: %d %d %d", v.x, v.y, v.z);

    auto df = d_get_current_floor(g->dungeon);
    auto t = df_tile_at(df, v);

    for (size_t i = 0; i < t->entities->size(); i++) {
        const entityid id = t->entities->at(i);
        const entitytype_t type = g->ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (type == ENTITY_DOOR) {
            return id;
        }
    }

    return ENTITYID_INVALID;
}
