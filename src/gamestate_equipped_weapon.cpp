#include "gamestate_equipped_weapon.h"

bool g_add_equipped_weapon(shared_ptr<gamestate> g, entityid id, entityid wpn_id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_EQUIPPED_WEAPON)) {
        merror("g_add_equipped_weapon: Failed to add component C_EQUIPPED_WEAPON for id %d", id);
        return false;
    }
    if (!g->equipped_weapon_list) {
        merror("g->equipped_weapon_list is NULL");
        return false;
    }
    // Check if the equipped weapon already exists for the entity
    (*g->equipped_weapon_list)[id] = wpn_id; // Insert or update the equipped weapon
    msuccess("Entity id %d weapon set to %d", id, wpn_id);
    return true;
}


bool g_has_equipped_weapon(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->equipped_weapon_list) {
        merror("g->equipped_weapon_list is NULL");
        return false;
    }
    // Check if the entity has an equipped weapon component
    return g->equipped_weapon_list->find(id) != g->equipped_weapon_list->end();
}


//bool g_set_equipped_weapon(shared_ptr<gamestate> g, entityid id, entityid wpn_id) {
bool g_equip_weapon(shared_ptr<gamestate> g, entityid id, entityid wpn_id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->equipped_weapon_list) {
        merror("g->equipped_weapon_list is NULL");
        return false;
    }
    // Check if the entity has an equipped weapon component
    if (g_has_equipped_weapon(g, id)) {
        // Update the equipped weapon for the entity
        (*g->equipped_weapon_list)[id] = wpn_id; // Update the equipped weapon
        return true;
    }
    merror("g_set_equipped_weapon: id %d does not have an equipped weapon component", id);
    return false;
}


bool g_unequip_weapon(shared_ptr<gamestate> g, entityid id, entityid wpn_id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->equipped_weapon_list) {
        merror("g->equipped_weapon_list is NULL");
        return false;
    }
    // Check if the entity has an equipped weapon component
    if (g_has_equipped_weapon(g, id)) {
        // Update the equipped weapon for the entity to ENTITYID_INVALID
        (*g->equipped_weapon_list)[id] = ENTITYID_INVALID; // Unequip the weapon
        return true;
    }
    merror("g_unequip_weapon: id %d does not have an equipped weapon component", id);
    return false;
}


entityid g_get_equipped_weapon(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->equipped_weapon_list) {
        if (g_has_equipped_weapon(g, id)) {
            massert(g->equipped_weapon_list->find(id) != g->equipped_weapon_list->end(), "g_get_equipped_weapon: id %d not found in equipped weapon list", id);
            return g->equipped_weapon_list->at(id);
        }
    }
    //merror("g_get_equipped_weapon: id %d does not have an equipped weapon component", id);
    return ENTITYID_INVALID; // Return ENTITYID_INVALID if not found
}
