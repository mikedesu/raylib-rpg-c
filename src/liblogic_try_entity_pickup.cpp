#include "ComponentTraits.h"
#include "entitytype.h"
#include "gamestate.h"
#include "liblogic_add_message.h"
#include "massert.h"
#include "sfx.h"


bool try_entity_pickup(shared_ptr<gamestate> g, entityid id) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity is NULL!");
    g->ct.set<update>(id, true);
    // check if the player is on a tile with an item

    vec3 loc = g->ct.get<location>(id).value();

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
        //entitytype_t type = g_get_type(g, itemid);
        entitytype_t type = g->ct.get<entitytype>(itemid).value_or(ENTITY_NONE);
        //    //minfo("Item %s type: %d", g_get_name(g, itemid), type);
        if (type == ENTITY_ITEM) {
            optional<shared_ptr<vector<entityid>>> maybe_inventory = g->ct.get<inventory>(id);
            if (maybe_inventory.has_value()) {
                msuccess("id %d has an inventory", id);
                tile_remove(tile, itemid);
                shared_ptr<vector<entityid>> my_inventory = maybe_inventory.value();
                // add the item_id to my_inventory
                my_inventory->push_back(itemid);
                PlaySound(g->sfx->at(SFX_CONFIRM_01));

            } else {
                merror("id %d does not have an inventory", id);
            }

            //    msuccess("item added to inventory");
            //} else {
            //}

            //    entityid equipped_wpn_id = g_get_equipped_weapon(g, id);
            //    if (equipped_wpn_id == ENTITYID_INVALID) {
            //        tile_remove(tile, itemid);
            //        g_set_equipped_weapon(g, id, itemid);
            //        add_message(g, "Equipped itemid %d", itemid);
            //    } else {
            //        tile_remove(tile, itemid);
            //        g_update_loc(g, equipped_wpn_id, (vec3){-1, -1, -1});
            //        g_set_equipped_weapon(g, id, itemid);
            //        add_message(g, "Equipped itemid %d", itemid);
            //
            //                    // update the loc of the equipped_wpn_id
            //                    g_update_loc(g, equipped_wpn_id, loc);
            //                    tile_add(tile, equipped_wpn_id);
            //                    add_message(g, "Added equipped_wpn_id %d", equipped_wpn_id);
            //                }
            entitytype_t t = g->ct.get<entitytype>(id).value_or(ENTITY_NONE);
            if (t == ENTITY_PLAYER) {
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            }
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
    //add_message(g, "No items to pick up");
    return false;
}
