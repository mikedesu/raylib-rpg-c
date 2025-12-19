#pragma once

#include "gamestate.h"
#include "spell.h"
#include "tile_has_live_npcs.h"

static inline entityid create_spell_with(gamestate& g, function<void(gamestate&, entityid)> spellInitFunction) {
    //massert(g, "gamestate is NULL");
    const auto id = g_add_entity(g);
    g.ct.set<entitytype>(id, ENTITY_SPELL);
    g.ct.set<spelltype>(id, SPELLTYPE_FIRE);
    g.ct.set<spellstate>(id, SPELLSTATE_NONE);
    spellInitFunction(g, id);
    return id;
}


static inline entityid create_spell_at_with(gamestate& g, vec3 loc, function<void(gamestate&, entityid)> spellInitFunction) {
    auto df = d_get_floor(g.dungeon, loc.z);
    auto tile = df_tile_at(df, loc);
    //massert(df, "failed to get df");
    //massert(tile, "failed to get tile");
    if (!tile_is_walkable(tile.type))
        return ENTITYID_INVALID;
    //if (tile_has_live_npcs(g, tile))
    //    return ENTITYID_INVALID;
    const auto id = create_spell_with(g, spellInitFunction);
    if (id == ENTITYID_INVALID)
        return ENTITYID_INVALID;
    minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
    if (!df_add_at(df, id, loc.x, loc.y))
        return ENTITYID_INVALID;
    g.ct.set<location>(id, loc);
    g.ct.set<update>(id, true);
    return id;
}
