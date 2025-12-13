#pragma once

#include "check_hearing.h"
#include "entity_accessors.h"
#include "entity_templates.h"
#include "gamestate.h"
#include "libgame_defines.h"
#include "play_sound.h"
#include "sfx.h"
#include "tile_has_door.h"
#include "tile_npc_living_count.h"

static inline bool try_entity_move(shared_ptr<gamestate> g, entityid id, vec3 v) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
    minfo("try_entity_move: %d, (%d,%d,%d)", id, v.x, v.y, v.z);
    set_update(g, id, true);
    g->ct.set<direction>(id, get_dir_from_xy(v.x, v.y));
    // entity location
    auto maybe_loc = maybe_location(id);
    if (!maybe_loc.has_value())
        return false;
    const vec3 loc = maybe_loc.value();
    // entity's new location
    // we will have a special case for traversing floors so ignore v.z
    const vec3 aloc = {loc.x + v.x, loc.y + v.y, loc.z};
    auto df = d_get_floor(g->dungeon, loc.z);
    if (!df) {
        merror("Dungeon floor %d does not exist", loc.z);
        return false;
    }
    // i feel like this might be something we can set elsewhere...like after the player input phase?
    auto tile = df_tile_at(df, aloc);
    if (!tile) {
        merror("Tile does not exist at (%d, %d, %d)", aloc.x, aloc.y, aloc.z);
        return false; // Tile does not exist
    }
    if (!tile_is_walkable(tile->type)) {
        merror("Tile at (%d, %d, %d) is not walkable", aloc.x, aloc.y, aloc.z);
        return false;
    }
    //entityid box_id = tile_has_box(g, aloc.x, aloc.y, aloc.z);
    // we need to
    // 1. check to see if box_id is pushable
    // 2. check to see if the tile in front of box, if pushed, is free/open
    //if (box_id != ENTITYID_INVALID) {
    // 1. check to see if box_id is pushable
    //if (g_is_pushable(g, box_id)) {
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
    if (maybe_door == ENTITYID_INVALID) {
        minfo("No door...");
    } else {
        minfo("Tile has door");
        auto maybe_is_open = g->ct.get<door_open>(maybe_door);
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

    const float mx = v.x * DEFAULT_TILE_SIZE;
    const float my = v.y * DEFAULT_TILE_SIZE;

    auto current_tile = df_tile_at(df, loc);
    massert(current_tile, "Current tile is NULL at (%d, %d, %d)", loc.x, loc.y, loc.z);

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

    set_location(g, id, aloc);
    set_sprite_move(g, id, (Rectangle){mx, my, 0, 0});

    play_sound_if_heard(SFX_STEP_STONE_1, check_hearing(g, g->hero_id, aloc));

    return true;
}
