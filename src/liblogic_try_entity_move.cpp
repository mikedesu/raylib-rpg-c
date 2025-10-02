#include "libgame_defines.h"
#include "liblogic_try_entity_move.h"
#include "massert.h"

entityid tile_has_box(shared_ptr<gamestate> g, int x, int y, int z);
int tile_npc_living_count(shared_ptr<gamestate> g, int x, int y, int z);

bool try_entity_move(shared_ptr<gamestate> g, entityid id, vec3 v) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
    minfo("try_entity_move: %d, (%d,%d,%d)", id, v.x, v.y, v.z);
    g->ct.set<Update>(id, true);
    g->ct.set<Direction>(id, get_dir_from_xy(v.x, v.y));

    // entity location
    vec3 loc = g->ct.get<Location>(id).value();

    // entity's new location
    // we will have a special case for traversing floors so ignore v.z
    vec3 aloc = {loc.x + v.x, loc.y + v.y, loc.z};
    shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, loc.z);
    if (!df) {
        merror("Dungeon floor %d does not exist", loc.z);
        return false; // Floor does not exist
    }

    // i feel like this might be something we can set elsewhere...like after the player input phase?
    shared_ptr<tile_t> tile = df_tile_at(df, aloc);

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

    float mx = v.x * DEFAULT_TILE_SIZE;
    float my = v.y * DEFAULT_TILE_SIZE;

    shared_ptr<tile_t> current_tile = df_tile_at(df, loc);
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

    g->ct.set<Location>(id, aloc);

    g->ct.set<SpriteMove>(id, (Rectangle){mx, my, 0, 0});

    return true;
}
