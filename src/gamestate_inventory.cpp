#include "gamestate_inventory.h"

using std::make_shared;

bool g_add_inventory(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_INVENTORY)) {
        merror("g_add_inventory: Failed to add component C_INVENTORY for id %d", id);
        return false;
    }
    if (!g->inventory_list) {
        merror("g->inventory_list is NULL");
        return false;
    }
    // Check if the inventory already exists for the entity
    //if (g->inventory_list->find(id) != g->inventory_list->end()) {
    if (g_has_inventory(g, id)) {
        merror("g_add_inventory: id %d already has an inventory component", id);
        return false;
    }
    //(*g->inventory_list)[id] = make_shared<item_container>(2, 5);
    // this line sometimes crashes...but why?
    //g->inventory_list->insert({id, make_shared<item_container>(2, 5)});
    if (g->inventory_list) {
        g->inventory_list->insert({id, make_shared<set<entityid>>()});
        return true;
    }
    merror("Something fucked up");
    return false;
}


bool g_has_inventory(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->inventory_list) {
        merror("g->inventory_list is NULL");
        return false;
    }
    // Check if the entity has an inventory component
    return g->inventory_list->find(id) != g->inventory_list->end();
}


shared_ptr<set<entityid>> g_get_inventory(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->inventory_list) {
        if (!g_has_inventory(g, id)) {
            merror("g_get_inventory: id %d does not have an inventory component", id);
            return nullptr;
        }
        massert(g->inventory_list->find(id) != g->inventory_list->end(), "g_get_inventory: id %d not found in inventory list", id);
        return g->inventory_list->at(id);
    }
    merror("g_get_inventory: g->inventory_list is NULL");
    return nullptr; // Return nullptr if the id is not found
}


bool g_exists_in_inventory(shared_ptr<gamestate> g, entityid id, entityid item_id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(item_id != ENTITYID_INVALID, "item_id is invalid");
    shared_ptr<set<entityid>> inventory = g_get_inventory(g, id);
    if (!inventory) {
        merror("could not find inventory for id %d", id);
        return false;
    }
    return inventory->find(item_id) != inventory->end();
}


bool g_add_to_inventory(shared_ptr<gamestate> g, entityid id, entityid item_id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(item_id != ENTITYID_INVALID, "item_id is invalid");

    if (g_exists_in_inventory(g, id, item_id)) {
        merror("item is already in inventory");
        return false;
    }

    shared_ptr<set<entityid>> inventory = g_get_inventory(g, id);
    if (!inventory) {
        merror("could not find inventory for id %d", id);
        return false;
    }

    inventory->insert(item_id);
    return true;
}

bool g_remove_from_inventory(shared_ptr<gamestate> g, entityid id, entityid item_id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(item_id != ENTITYID_INVALID, "item_id is invalid");

    if (g_exists_in_inventory(g, id, item_id)) {
        // perform remove
        shared_ptr<set<entityid>> inventory = g_get_inventory(g, id);
        if (!inventory) {
            merror("could not find inventory for id %d", id);
            return false;
        }
        inventory->erase(item_id);
        return true;
    }

    merror("item doesnt exist in the inventory");
    return false;
}
