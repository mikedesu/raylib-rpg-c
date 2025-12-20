#pragma once

#include "entityid.h"
#include "gamestate.h"
#include "tile_has_box.h"
#include "tile_has_live_npcs.h"

static inline entityid create_box_with(gamestate& g, function<void(gamestate&, entityid)> boxInitFunction) {
    const auto id = g_add_entity(g);
    g.ct.set<entitytype>(id, ENTITY_BOX);
    g.ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
    g.ct.set<update>(id, true);
    g.ct.set<pushable>(id, true);
    boxInitFunction(g, id);
    return id;
}


static inline entityid create_box_at_with(gamestate& g, vec3 loc, function<void(gamestate&, entityid)> boxInitFunction) {
    auto df = d_get_floor(g.dungeon, loc.z);
    auto tile = df_tile_at(df, loc);
    if (!tile_is_walkable(tile.type)) {
        merror("cannot create entity on non-walkable tile");
        return ENTITYID_INVALID;
    }

    if (tile_has_live_npcs(g, tile)) {
        merror("cannot create entity on tile with live NPCs");
        return ENTITYID_INVALID;
    }

    if (tile_has_box(g, loc.x, loc.y, loc.z) != ENTITYID_INVALID) {
        merror("cannot create entity on tile with box");
        return ENTITYID_INVALID;
    }

    const auto id = create_box_with(g, boxInitFunction);
    if (id == ENTITYID_INVALID) {
        return ENTITYID_INVALID;
    }

    minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);

    if (!df_add_at(df, id, loc.x, loc.y)) {
        merror("failed df_add_at: %d, %d, %d", id, loc.x, loc.y);
        return ENTITYID_INVALID;
    }

    g.ct.set<location>(id, loc);
    return id;
}
