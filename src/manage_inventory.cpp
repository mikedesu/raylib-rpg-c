#include "manage_inventory.h"

bool add_to_inventory(shared_ptr<gamestate> g, entityid actor_id, entityid item_id) {
    optional<shared_ptr<vector<entityid>>> maybe_inventory = g->ct.get<inventory>(actor_id);
    if (maybe_inventory.has_value()) {
        msuccess("id %d has an inventory", actor_id);
        shared_ptr<vector<entityid>> my_inventory = maybe_inventory.value();
        my_inventory->push_back(item_id);
        return true;
    }
    merror("id %d does not have an inventory", actor_id);
    return false;
}
