#include "dungeon.h"
#include "liblogic_add_message.h"
#include "manage_inventory.h"
#include <algorithm>


using std::find;


bool add_to_inventory(shared_ptr<gamestate> g, entityid actor_id, entityid item_id) {
    auto maybe_inventory = g->ct.get<inventory>(actor_id);
    if (!maybe_inventory.has_value())
        return false;
    auto my_inventory = maybe_inventory.value();
    my_inventory->push_back(item_id);
    return true;
}


bool remove_from_inventory(shared_ptr<gamestate> g, entityid actor_id, entityid item_id) {
    auto maybe_inventory = g->ct.get<inventory>(actor_id);
    if (!maybe_inventory.has_value())
        return false;

    auto my_items = maybe_inventory.value();
    auto it = find(my_items->begin(), my_items->end(), item_id);
    if (it == my_items->end()) {
        merror("Could not find item with id %d", item_id);
        return false;
    }

    my_items->erase(it);

    return true;
}


bool drop_from_inventory(shared_ptr<gamestate> g, entityid actor_id, entityid item_id) {
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

        df_add_at(df, item_id, loc.x, loc.y);
        return true;
    }
    merror("Remove from inventory failed for some reason");
    return false;
}
