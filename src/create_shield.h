#pragma once

#include "gamestate.h"

static inline entityid create_shield_with(gamestate& g, function<void(gamestate&, entityid)> shieldInitFunction) {
    const auto id = g_add_entity(g);
    g.ct.set<entitytype>(id, ENTITY_ITEM);
    g.ct.set<itemtype>(id, ITEM_SHIELD);
    shieldInitFunction(g, id);
    return id;
}


static inline entityid create_shield_at_with(gamestate& g, vec3 loc, function<void(gamestate&, entityid)> shieldInitFunction) {
    const auto id = create_shield_with(g, shieldInitFunction);
    minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
    auto df = d_get_floor(g.dungeon, loc.z);
    if (!df_add_at(df, id, loc.x, loc.y))
        return ENTITYID_INVALID;
    g.ct.set<location>(id, loc);
    return id;
}
