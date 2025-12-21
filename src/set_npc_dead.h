#pragma once

#include "gamestate.h"

static inline void set_npc_dead(gamestate& g, entityid id) {
    g.ct.set<dead>(id, true);
    const vec3 loc = g.ct.get<location>(id).value_or((vec3){-1, -1, -1});
    massert(!vec3_equal(loc, (vec3){-1, -1, -1}), "id %d has no location", id);

    tile_t& target_tile = df_tile_at(d_get_current_floor(g.dungeon), loc);
    target_tile.dirty_entities = true;
}
