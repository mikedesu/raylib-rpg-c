#include "ComponentTraits.h"
#include "dungeon.h"
#include "entityid.h"
#include "manage_inventory.h"


bool add_to_inventory(shared_ptr<gamestate> g, entityid actor_id, entityid item_id) {
    minfo("add to inventory: %d %d", actor_id, item_id);
    auto maybe_inventory = g->ct.get<inventory>(actor_id);
    if (!maybe_inventory.has_value()) {
        merror("no inventory: %d", actor_id);
        return false;
    }
    auto my_inventory = maybe_inventory.value();
    my_inventory->push_back(item_id);
    msuccess("item id %d added successfully to actor %d's inventory", item_id, actor_id);
    msuccess("inventory size: %ld", my_inventory->size());
    return true;
}


bool remove_from_inventory(shared_ptr<gamestate> g, entityid actor_id, entityid item_id) {
    minfo("remove from inventory: %d %d", actor_id, item_id);
    auto maybe_inventory = g->ct.get<inventory>(actor_id);
    if (!maybe_inventory.has_value()) {
        merror("maybe_inventory has no value for actor id %d", actor_id);
        return false;
    }

    auto my_items = maybe_inventory.value();
    //auto it = find(my_items->begin(), my_items->end(), item_id);
    //if (it == my_items->end()) {
    //    merror("Could not find item with id %d", item_id);
    //    return false;
    //}

    //my_items->erase(it);

    bool success = false;

    minfo("Inventory size: %ld", my_items->size());

    for (auto it = my_items->begin(); it != my_items->end(); it++) {
        minfo("item id %d", *it);
        if (*it == item_id) {
            my_items->erase(it);
            success = true;
            break;
        }
    }

    if (!success) {
        merror("Failed to find item id %d", item_id);
    } else {
        msuccess("Successfully removed item id %d", item_id);
    }

    return success;
}


bool drop_from_inventory(shared_ptr<gamestate> g, entityid actor_id, entityid item_id) {
    minfo("drop from inventory: %d %d", actor_id, item_id);
    if (remove_from_inventory(g, actor_id, item_id)) {
        auto maybe_loc = g->ct.get<location>(actor_id);
        if (!maybe_loc.has_value()) {
            merror("actor id %d has no location -- cannot drop item", actor_id);
            return false;
        }

        auto loc = maybe_loc.value();

        auto df = d_get_current_floor(g->dungeon);
        auto tile = df_tile_at(df, loc);
        if (!tile) {
            merror("No tile at that location: %d %d %d", loc.x, loc.y, loc.z);
            return false;
        }


        const entityid retval = df_add_at(df, item_id, loc.x, loc.y);

        //const entityid retval = tile_add(tile, item_id);
        if (retval == ENTITYID_INVALID) {
            merror("Failed to add to tile");
            return false;
        }
        // update the item_id's location
        g->ct.set<location>(item_id, loc);

        msuccess("Drop item successful");

        return true;
    }
    merror("Remove from inventory failed for some reason");
    return false;
}


bool drop_all_from_inventory(shared_ptr<gamestate> g, entityid actor_id) {
    minfo("drop all from inventory: %d", actor_id);

    auto maybe_inventory = g->ct.get<inventory>(actor_id);
    if (!maybe_inventory.has_value()) {
        merror("no inventory");
        return false;
    }

    auto inventory = maybe_inventory.value();

    while (inventory->size() > 0) {
        auto id = inventory->back();
        //inventory->pop_back();

        drop_from_inventory(g, actor_id, id);
    }

    return true;
}
