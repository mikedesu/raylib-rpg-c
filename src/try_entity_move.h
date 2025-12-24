#pragma once

#include "check_hearing.h"
#include "dungeon.h"
#include "entity_accessors.h"
#include "entity_templates.h"
#include "gamestate.h"
//#include "handle_box_push.h"
#include "libgame_defines.h"
#include "play_sound.h"
#include "sfx.h"
#include "tile_has_box.h"
#include "tile_has_door.h"
#include "tile_npc_living_count.h"

static inline bool try_entity_move(gamestate& g, entityid id, vec3 v);

static inline bool handle_box_push(gamestate& g, entityid id, vec3 v) {
    const bool can_push = g.ct.get<pushable>(id).value_or(false);
    if (can_push) {
        return try_entity_move(g, id, v);
    }
    // box is not pushable
    minfo("Box NOT pushable");
    return false;
}

static inline bool try_entity_move(gamestate& g, entityid id, vec3 v) {
    massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
    //minfo("try_entity_move: %d, (%d,%d,%d)", id, v.x, v.y, v.z);
    g.ct.set<update>(id, true);
    g.ct.set<direction>(id, get_dir_from_xy(v.x, v.y));
    // entity location
    massert(g.ct.has<location>(id), "id %d has no location", id);
    //auto maybe_loc = g.ct.get<location>(id);
    //if (!maybe_loc.has_value()) {
    //    return false;
    //}
    const vec3 loc = g.ct.get<location>(id).value_or((vec3){-1, -1, -1});
    massert(!vec3_equal(loc, (vec3){-1, -1, -1}), "id %d has no location", id);
    // entity's new location
    // we will have a special case for traversing floors so ignore v.z
    const vec3 aloc = {loc.x + v.x, loc.y + v.y, loc.z};
    auto df = d_get_floor(g.dungeon, loc.z);
    auto tile = df_tile_at(df, aloc);
    if (!tile_is_walkable(tile.type)) {
        return false;
    }

    const entityid box_id = tile_has_box(g, aloc.x, aloc.y, aloc.z);
    // we need to
    // 1. check to see if box_id is pushable
    // 2. check to see if the tile in front of box, if pushed, is free/open
    if (box_id != ENTITYID_INVALID) {
        return handle_box_push(g, box_id, v);
    }

    if (tile_has_live_npcs(g, g.tile_at_cur_floor(aloc))) {
        return false;
    }

    const entityid door_id = tile_has_door(g, aloc);
    if (door_id != ENTITYID_INVALID) {
        //minfo("Tile has door");
        massert(g.ct.has<door_open>(door_id), "door_id %d doesnt have a door_open component", door_id);
        if (!g.ct.get<door_open>(door_id).value_or(false))
            return false;
    }

    // if door, door is open

    // remove the entity from the current tile
    if (!df_remove_at(df, id, loc.x, loc.y)) {
        merror("Failed to remove entity %d from tile at (%d, %d, %d)", id, loc.x, loc.y, loc.z);
        return false;
    }

    // add the entity to the new tile
    if (!df_add_at(df, id, aloc.x, aloc.y)) {
        merror("Failed to add entity %d to tile at (%d, %d, %d)", id, aloc.x, aloc.y, aloc.z);
        return false;
    }

    g.ct.set<location>(id, aloc);

    const float mx = v.x * DEFAULT_TILE_SIZE, my = v.y * DEFAULT_TILE_SIZE;
    g.ct.set<spritemove>(id, (Rectangle){mx, my, 0, 0});

    if (check_hearing(g, g.hero_id, aloc))
        PlaySound(g.sfx[SFX_STEP_STONE_1]);

    return true;
}
