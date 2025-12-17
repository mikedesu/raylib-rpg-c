#pragma once

#include "entityid.h"
#include "entitytype.h"
#include "gamestate.h"
#include "massert.h"

static inline int tile_npc_living_count(shared_ptr<gamestate> g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    massert(z >= 0, "floor is out of bounds");
    massert((size_t)z < g->dungeon->floors->size(), "floor is out of bounds");
    auto df = d_get_floor(g->dungeon, z);
    massert(df, "failed to get dungeon floor");
    auto t = df_tile_at(df, (vec3){x, y, z});
    massert(t, "failed to get tile");
    int count = 0;
    for (int i = 0; (size_t)i < t->entities->size(); i++) {
        const entityid id = tile_get_entity(t, i);
        const entitytype_t type = g->ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (id == ENTITYID_INVALID)
            continue;
        else if (type != ENTITY_NPC && type != ENTITY_PLAYER)
            continue;
        else if (g->ct.get<dead>(id).value_or(true))
            continue;
        count++;
    }
    return count;
}
