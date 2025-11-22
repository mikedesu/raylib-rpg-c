#include "liblogic_add_message.h"
#include "manage_inventory.h"

bool add_to_inventory(shared_ptr<gamestate> g, entityid actor_id, entityid item_id) {
    optional<shared_ptr<vector<entityid>>> maybe_inventory = g->ct.get<inventory>(actor_id);
    if (maybe_inventory.has_value()) {
        msuccess("id %d has an inventory", actor_id);
        shared_ptr<vector<entityid>> my_inventory = maybe_inventory.value();
        my_inventory->push_back(item_id);

        if (actor_id == g->hero_id) {
            // get item name
            optional<string> maybe_name = g->ct.get<name>(item_id);
            if (maybe_name.has_value()) {
                string name = maybe_name.value();
                add_message(g, "You picked up %s", name.c_str());
            }
        }

        return true;
    }
    merror("id %d does not have an inventory", actor_id);
    return false;
}
