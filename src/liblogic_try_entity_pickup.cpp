#include "gamestate.h"
#include "gamestate_equipped_weapon.h"
#include "liblogic_add_message.h"
#include "massert.h"


bool try_entity_pickup(shared_ptr<gamestate> g, entityid id) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity is NULL!");
    g_set_update(g, id, true);
    // check if the player is on a tile with an item
    vec3 loc = g_get_loc(g, id);
    shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, loc.z);
    if (!df) {
        merror("Failed to get dungeon floor");
        return false;
    }
    shared_ptr<tile_t> tile = df_tile_at(df, loc);
    if (!tile) {
        merror("Failed to get tile");
        return false;
    }
    if (tile->entities->size() == 0) {
        add_message(g, "No items on tile");
        return false;
    }
    for (size_t i = 0; i < tile->entities->size(); i++) {
        entityid itemid = tile->entities->at(i);
        entitytype_t type = g_get_type(g, itemid);
        //    //minfo("Item %s type: %d", g_get_name(g, itemid), type);
        if (type == ENTITY_ITEM) {
            // now, we need to check what type of item it is
            itemtype itype = g_get_item_type(g, itemid);
            if (itype == ITEM_WEAPON) {
                // if it is a weapon, we will "pick it up" and equip it
                // if we have no weapon equipped, the weapon will
                // simply come off the tile and be attached to the
                // equipped weapon slot
                //
                // if we have a weapon equipped, for right now, we will
                // swap the equipped weapon with the weapon on the tile,
                // that is, the weapon on the tile will be removed,
                // the equipped weapon will be detached and added to the tile,
                // and we will equip the new weapon

                entityid equipped_wpn_id = g_get_equipped_weapon(g, id);
                if (equipped_wpn_id == ENTITYID_INVALID) {
                    tile_remove(tile, itemid);
                    g_set_equipped_weapon(g, id, itemid);
                    add_message(g, "Equipped itemid %d", itemid);
                } else {
                    tile_remove(tile, itemid);
                    g_update_loc(g, equipped_wpn_id, (vec3){-1, -1, -1});
                    g_set_equipped_weapon(g, id, itemid);
                    add_message(g, "Equipped itemid %d", itemid);

                    // update the loc of the equipped_wpn_id
                    g_update_loc(g, equipped_wpn_id, loc);
                    tile_add(tile, equipped_wpn_id);
                    add_message(g, "Added equipped_wpn_id %d", equipped_wpn_id);
                }
                if (g_get_type(g, id) == ENTITY_PLAYER) g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
                break;
            }
        }
        //        add_message_history(g,
        //                            "%s picked up a %s",
        //                            g_get_name(g, id).c_str(),
        //                            g_get_name(g, itemid).c_str());
        //        bool result = g_add_to_inventory(g, id, itemid);
        //        if (!result) {
        //            merror("Failed to add item to inventory");
        //            return false;
        //        } else {
        //            //minfo("Item %s added to inventory", g_get_name(g, itemid));
        //            tile_remove(tile, itemid);
        //        }
        //        if (g_is_type(g, id, ENTITY_PLAYER)) {
        //            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        //        }
        //        return true;
    }
    //add_message(g, "No items to pick up");
    return false;
}
