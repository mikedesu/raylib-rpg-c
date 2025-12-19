#pragma once

#include "gamestate.h"
#include "tile_has_live_npcs.h"

static inline entityid create_weapon_with(gamestate& g, function<void(gamestate&, entityid)> weaponInitFunction) {
    //massert(g, "gamestate is NULL");
    const auto id = g_add_entity(g);
    g.ct.set<entitytype>(id, ENTITY_ITEM);
    g.ct.set<itemtype>(id, ITEM_WEAPON);
    g.ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
    g.ct.set<update>(id, true);
    weaponInitFunction(g, id);
    return id;
}


static inline entityid create_weapon_at_with(gamestate& g, vec3 loc, function<void(gamestate&, entityid)> weaponInitFunction) {
    //massert(g, "gamestate is NULL");
    auto df = d_get_floor(g.dungeon, loc.z);
    auto tile = df_tile_at(df, loc);
    //massert(tile, "failed to get tile");
    if (!tile_is_walkable(tile.type)) {
        merror("cannot create entity on non-walkable tile");
        return ENTITYID_INVALID;
    }
    if (tile_has_live_npcs(g, tile)) {
        merror("cannot create entity on tile with live NPCs");
        return ENTITYID_INVALID;
    }
    const auto id = create_weapon_with(g, weaponInitFunction);
    if (id == ENTITYID_INVALID) {
        return ENTITYID_INVALID;
    }
    minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
    if (!df_add_at(df, id, loc.x, loc.y)) {
        return ENTITYID_INVALID;
    }
    g.ct.set<location>(id, loc);
    return id;
}
