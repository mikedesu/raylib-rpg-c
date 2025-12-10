#include "ComponentTraits.h"
#include "add_message.h"
#include "entityid.h"
#include "entitytype.h"
#include "gamestate.h"
#include "manage_inventory.h"
#include "massert.h"
#include "sfx.h"


bool try_entity_pickup(shared_ptr<gamestate> g, entityid id) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity is NULL!");
    g->ct.set<update>(id, true);
    // check if the player is on a tile with an item

    auto maybe_loc = g->ct.get<location>(id);
    if (!maybe_loc.has_value()) {
        return false;
    }

    auto loc = maybe_loc.value();
    auto df = d_get_floor(g->dungeon, loc.z);
    if (!df) {
        merror("Failed to get dungeon floor");
        return false;
    }

    auto tile = df_tile_at(df, loc);
    if (!tile) {
        merror("Failed to get tile");
        return false;
    }

    //if (tile->entities->size() == 0) {
    //    add_message(g, "No items on tile");
    //    return false;
    //}


    bool item_picked_up = false;
    auto item_id = ENTITYID_INVALID;

    for (size_t i = 0; i < tile->entities->size(); i++) {
        entityid itemid = tile->entities->at(i);
        auto type = g->ct.get<entitytype>(itemid).value_or(ENTITY_NONE);
        if (type == ENTITY_ITEM) {
            if (add_to_inventory(g, id, itemid)) {
                tile_remove(tile, itemid);
                PlaySound(g->sfx->at(SFX_CONFIRM_01));
                item_picked_up = true;
                item_id = itemid;
            }
            //optional<shared_ptr<vector<entityid>>> maybe_inventory = g->ct.get<inventory>(id);
            //if (maybe_inventory.has_value()) {
            //    msuccess("id %d has an inventory", id);
            //    tile_remove(tile, itemid);
            //    shared_ptr<vector<entityid>> my_inventory = maybe_inventory.value();
            //    // add the item_id to my_inventory
            //    my_inventory->push_back(itemid);
            //    PlaySound(g->sfx->at(SFX_CONFIRM_01));
            //} else {
            //    merror("id %d does not have an inventory", id);
            //}
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
            auto t = g->ct.get<entitytype>(id).value_or(ENTITY_NONE);
            if (t == ENTITY_PLAYER) {
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            }
            break;
        }
    }

    if (item_picked_up) {
        auto item_name = g->ct.get<name>(item_id).value_or("no-name-item");
        add_message_history(g, "You picked up %s", item_name.c_str());
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
    return item_picked_up;
}
