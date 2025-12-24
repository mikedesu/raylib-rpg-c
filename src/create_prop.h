#pragma once


#include "gamestate.h"
#include "tile_has_live_npcs.h"

static inline entityid create_prop_with(gamestate& g, proptype_t type, function<void(gamestate&, entityid)> propInitFunction) {
    const auto id = g.add_entity();
    g.ct.set<entitytype>(id, ENTITY_PROP);
    g.ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
    g.ct.set<update>(id, true);
    g.ct.set<proptype>(id, type);
    propInitFunction(g, id);
    return id;
}


static inline entityid create_prop_at_with(gamestate& g, proptype_t type, vec3 loc, function<void(gamestate&, entityid)> propInitFunction) {
    auto df = d_get_floor(g.dungeon, loc.z);
    auto tile = df_tile_at(df, loc);

    //if (!tile_is_walkable(tile.type)) {
    //    merror("cannot create entity on non-walkable tile");
    //    return ENTITYID_INVALID;
    //}

    //if (tile_has_live_npcs(g, tile)) {
    //    merror("cannot create entity on tile with live NPCs");
    //    return ENTITYID_INVALID;
    //}

    //if (tile_has_box(g, loc.x, loc.y, loc.z) != ENTITYID_INVALID) {
    //    merror("cannot create entity on tile with box");
    //    return ENTITYID_INVALID;
    //}

    const auto id = create_prop_with(g, type, propInitFunction);
    if (id == ENTITYID_INVALID) {
        return ENTITYID_INVALID;
    }

    minfo("attempting prop df_add_at: %d, %d, %d", id, loc.x, loc.y);

    if (!df_add_at(df, id, loc.x, loc.y)) {
        merror("failed df_add_at: %d, %d, %d", id, loc.x, loc.y);
        return ENTITYID_INVALID;
    }

    msuccess("prop add success");

    g.ct.set<location>(id, loc);

    return id;
}
