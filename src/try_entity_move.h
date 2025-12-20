#pragma once

#include "check_hearing.h"
#include "entity_accessors.h"
#include "entity_templates.h"
#include "gamestate.h"
#include "libgame_defines.h"
#include "play_sound.h"
#include "sfx.h"
#include "tile_has_box.h"
#include "tile_has_door.h"
#include "tile_npc_living_count.h"

static inline bool try_entity_move(gamestate& g, entityid id, vec3 v) {
    massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
    minfo("try_entity_move: %d, (%d,%d,%d)", id, v.x, v.y, v.z);
    g.ct.set<update>(id, true);
    g.ct.set<direction>(id, get_dir_from_xy(v.x, v.y));
    // entity location
    auto maybe_loc = g.ct.get<location>(id);
    if (!maybe_loc.has_value())
        return false;
    const vec3 loc = maybe_loc.value();
    // entity's new location
    // we will have a special case for traversing floors so ignore v.z
    const vec3 aloc = {loc.x + v.x, loc.y + v.y, loc.z};
    auto df = d_get_floor(g.dungeon, loc.z);
    // i feel like this might be something we can set elsewhere...like after the player input phase?
    auto tile = df_tile_at(df, aloc);
    if (!tile_is_walkable(tile.type)) {
        merror("Tile at (%d, %d, %d) is not walkable", aloc.x, aloc.y, aloc.z);
        return false;
    }

    const entityid maybe_box_id = tile_has_box(g, aloc.x, aloc.y, aloc.z);
    // we need to
    // 1. check to see if box_id is pushable
    // 2. check to see if the tile in front of box, if pushed, is free/open
    if (maybe_box_id != ENTITYID_INVALID) {
        minfo("Box present");
        const bool maybe_pushable = g.ct.get<pushable>(maybe_box_id).value_or(false);
        if (maybe_pushable) {
            //const vec3 box_loc = g.ct.get<location>(maybe_box_id).value_or((vec3){-1, -1, -1});
            //const vec3 box_new_loc = {box_loc.x + v.x, box_loc.y + v.y, box_loc.z};
            //tile_t& box_new_loc_tile = df_tile_at(d_get_floor(g.dungeon, g.dungeon.current_floor), box_new_loc);
            //if (tile_entity_count(box_new_loc_tile) == 0) {
            if (try_entity_move(g, maybe_box_id, v)) {
                return true;
            } else {
                minfo("try entity move failed");
                return false;
            }

        } else {
            // box is not pushable
            minfo("Box NOT pushable");
            return false;
        }
    } else {
        // no boxes
        minfo("Box NOT present");
    }
    // 1. check to see if box_id is pushable
    //if (g->ct.get<Pushable>(box_id).value_or(false)) {
    // 2. check to see if the tile in front of box, if pushed, is free/open
    // get the box's location
    //vec3 box_loc = g_get_loc(g, box_id);
    //    vec3 box_loc = g->ct.get<Location>(box_id).value();
    // compute the location in front of the box
    //    vec3 box_new_loc = {box_loc.x + v.x, box_loc.y + v.y, box_loc.z};
    // get the tile at the new location
    //    shared_ptr<tile_t> box_new_loc_tile = df_tile_at(d_get_floor(g->dungeon, g->dungeon->current_floor), box_new_loc);
    // check to see tile has no entities
    //    if (tile_entity_count(box_new_loc_tile) == 0) {
    //        try_entity_move(g, box_id, v);
    //    }
    //}
    //return false;
    //}
    if (tile_npc_living_count(g, aloc.x, aloc.y, aloc.z) > 0) {
        merror("Tile at (%d, %d, %d) has living NPCs", aloc.x, aloc.y, aloc.z);
        return false;
    }


    const entityid maybe_door = tile_has_door(g, aloc);
    if (maybe_door != ENTITYID_INVALID) {
        minfo("Tile has door");
        auto maybe_is_open = g.ct.get<door_open>(maybe_door);
        if (maybe_is_open.has_value()) {
            bool is_open = maybe_is_open.value();
            if (!is_open) {
                merror("door closed");
                return false;
            }
        } else {
            merror("Door should have an open value");
            return false;
        }
    }
    // if door, door is open

    const float mx = v.x * DEFAULT_TILE_SIZE;
    const float my = v.y * DEFAULT_TILE_SIZE;

    auto current_tile = df_tile_at(df, loc);
    //massert(current_tile, "Current tile is NULL at (%d, %d, %d)", loc.x, loc.y, loc.z);

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
    g.ct.set<spritemove>(id, (Rectangle){mx, my, 0, 0});

    if (check_hearing(g, g.hero_id, aloc))
        PlaySound(g.sfx[SFX_STEP_STONE_1]);

    return true;
}
