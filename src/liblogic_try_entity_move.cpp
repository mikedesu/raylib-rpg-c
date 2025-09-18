#include "ComponentTraits.h"
#include "dungeon_tile.h"
#include "gamestate.h"
#include "libgame_defines.h"
#include "massert.h"

#include <memory>

using std::shared_ptr;

entityid tile_has_box(shared_ptr<gamestate> g, int x, int y, int z);
int tile_npc_living_count(shared_ptr<gamestate> g, int x, int y, int z);

bool try_entity_move(shared_ptr<gamestate> g, entityid id, vec3 v) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
    g_set_update(g, id, true);
    g_update_dir(g, id, get_dir_from_xy(v.x, v.y));
    // entity location
    //vec3 loc = g_get_loc(g, id);
    vec3 loc = g->ct.get<Location>(id).value();


    // entity's new location
    // we will have a special case for traversing floors so ignore v.z
    vec3 aloc = {loc.x + v.x, loc.y + v.y, loc.z};
    shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, loc.z);
    if (!df) {
        return false; // Floor does not exist
    }
    // i feel like this might be something we can set elsewhere...like after the player input phase?
    shared_ptr<tile_t> tile = df_tile_at(df, aloc);
    if (!tile) {
        return false; // Tile does not exist
    }
    if (!tile_is_walkable(tile->type)) {
        return false;
    }
    entityid box_id = tile_has_box(g, aloc.x, aloc.y, aloc.z);
    // we need to
    // 1. check to see if box_id is pushable
    // 2. check to see if the tile in front of box, if pushed, is free/open
    if (box_id != ENTITYID_INVALID) {
        // 1. check to see if box_id is pushable
        if (g_is_pushable(g, box_id)) {
            // 2. check to see if the tile in front of box, if pushed, is free/open
            // get the box's location
            //vec3 box_loc = g_get_loc(g, box_id);
            vec3 box_loc = g->ct.get<Location>(box_id).value();
            // compute the location in front of the box
            vec3 box_new_loc = {box_loc.x + v.x, box_loc.y + v.y, box_loc.z};
            // get the tile at the new location
            shared_ptr<tile_t> box_new_loc_tile = df_tile_at(d_get_floor(g->dungeon, g->dungeon->current_floor), box_new_loc);
            // check to see tile has no entities
            if (tile_entity_count(box_new_loc_tile) == 0) {
                try_entity_move(g, box_id, v);
            }
        }
        return false;
    }
    //if (tile_has_closed_door(g, ex, ey, floor)) {
    //    return;
    //}
    if (tile_npc_living_count(g, aloc.x, aloc.y, aloc.z) > 0) {
        return false;
    }
    float mx = v.x * DEFAULT_TILE_SIZE;
    float my = v.y * DEFAULT_TILE_SIZE;
    shared_ptr<tile_t> current_tile = df_tile_at(df, loc);
    massert(current_tile, "Current tile is NULL at (%d, %d, %d)", loc.x, loc.y, loc.z);
    // remove the entity from the current tile
    if (!df_remove_at(df, id, loc.x, loc.y)) {
        return false;
    }
    // add the entity to the new tile
    if (!df_add_at(df, id, aloc.x, aloc.y)) {
        return false;
    }
    //g_update_loc(g, id, aloc);
    g->ct.set<Location>(id, aloc);
    //g_update_sprite_move(g, id, (Rectangle){mx, my, 0, 0});

    g->ct.set<SpriteMove>(id, (Rectangle){mx, my, 0, 0});
    //g->ct<SpriteMove>[id]=(Rectangle){mx, my, 0, 0};

    return true;
    //if (player_on_tile(g, ex, ey, z)) {
    //    return;
    //}
    //if (!df_remove_at(df, id, loc.x, loc.y)) return;
    //if (!df_remove_at(df, id, loc.x, loc.y)) return;
    //if (!df_add_at(df, id, ex, ey)) return;
    //g_update_location(g, id, (vec3){ex, ey, z});
    //g_update_sprite_move(g, id, (vec3){x * DEFAULT_TILE_SIZE, y * DEFAULT_TILE_SIZE, 0});
    //g_update_sprite_move(g,
    //                     id,
    //                     (Rectangle){(float)(x * DEFAULT_TILE_SIZE),
    //                                 (float)(y * DEFAULT_TILE_SIZE),
    //                                 0,
    //                                 0});
    //if (id == g->hero_id) {
    //    update_player_tiles_explored(g);
    //}
    // at this point the move is 'successful'
    //update_equipped_shield_dir(g, id);
    // get the entity's new tile
    //tile_t* const new_tile = df_tile_at(df, (vec3){ex, ey, z});
    //if (!new_tile) {
    //    return;
    //}
    // do not remove!!!
    // check if the tile has a pressure plate
    //if (new_tile->has_pressure_plate) {
    // do something
    // print the pressure plate event
    //}
    // check if the tile is an ON TRAP
    //if (new_tile->type == TILE_FLOOR_STONE_TRAP_ON_00) {
    // do something
    //e->stats.hp--;
    //e->is_damaged = true;
    //    g_set_damaged(g, id, true);
    //e->do_update = true;
    //    g_set_update(g, id, true);
    //}
    //if (g_is_type(g, id, ENTITY_PLAYER)) g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
}
