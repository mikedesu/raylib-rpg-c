#pragma once

#include "entity_templates.h"
#include "gamestate.h"

static inline entityid create_door_with(shared_ptr<gamestate> g, function<void(shared_ptr<gamestate>, entityid)> doorInitFunction) {
    massert(g, "gamestate is NULL");
    const auto id = g_add_entity(g);
    g->ct.set<entitytype>(id, ENTITY_DOOR);
    doorInitFunction(g, id);
    return id;
}

static inline entityid create_door_at_with(shared_ptr<gamestate> g, vec3 loc, function<void(shared_ptr<gamestate>, entityid)> doorInitFunction) {
    //shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, loc.z);
    dungeon_floor_t& df = d_get_floor(g->dungeon, loc.z);
    shared_ptr<tile_t> tile = df_tile_at(df, loc);

    massert(tile, "failed to get tile");

    if (!tile_is_walkable(tile->type))
        return ENTITYID_INVALID;
    if (tile_has_live_npcs(g, tile))
        return ENTITYID_INVALID;

    const auto id = create_door_with(g, doorInitFunction);
    if (id == ENTITYID_INVALID)
        return ENTITYID_INVALID;

    minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
    if (!df_add_at(df, id, loc.x, loc.y))
        return ENTITYID_INVALID;

    g->ct.set<location>(id, loc);
    g->ct.set<door_open>(id, false);
    g->ct.set<update>(id, true);

    return id;
}
