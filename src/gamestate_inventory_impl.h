#pragma once

inline bool gamestate::remove_from_inventory(entityid actor_id, entityid item_id) {
    auto maybe_inventory = ct.get<inventory>(actor_id);
    if (!maybe_inventory.has_value()) {
        merror("maybe_inventory has no value for actor id %d", actor_id);
        return false;
    }
    auto my_items = maybe_inventory.value();
    bool success = false;
    for (auto it = my_items->begin(); it != my_items->end(); it++) {
        if (*it == item_id) {
            my_items->erase(it);
            success = true;
            break;
        }
    }

    if (!success) {
        merror("Failed to find item id %d", item_id);
    }
    else {
        msuccess("Successfully removed item id %d", item_id);
    }
    return success;
}

inline bool gamestate::drop_from_inventory(entityid actor_id, entityid item_id) {
    if (remove_from_inventory(actor_id, item_id)) {
        auto maybe_loc = ct.get<location>(actor_id);
        if (!maybe_loc.has_value()) {
            merror("actor id %d has no location -- cannot drop item", actor_id);
            return false;
        }
        vec3 loc = maybe_loc.value();
        auto df = d.get_current_floor();
        entityid retval = df->df_add_at(item_id, ENTITY_ITEM, loc);
        if (retval == ENTITYID_INVALID) {
            merror("Failed to add to tile");
            return false;
        }
        ct.set<location>(item_id, loc);
        msuccess("Drop item successful");
        return true;
    }
    merror("Remove from inventory failed for some reason");
    return false;
}

inline bool gamestate::drop_all_from_inventory(entityid actor_id) {
    auto maybe_inventory = ct.get<inventory>(actor_id);
    if (!maybe_inventory.has_value()) {
        merror("no inventory");
        return false;
    }
    auto inventory = maybe_inventory.value();
    while (inventory->size() > 0) {
        auto id = inventory->back();
        drop_from_inventory(actor_id, id);
    }
    return true;
}

inline void gamestate::handle_hero_inventory_equip_weapon(entityid item_id) {
    entityid current_weapon = ct.get<equipped_weapon>(hero_id).value_or(ENTITYID_INVALID);
    if (current_weapon == item_id) {
        ct.set<equipped_weapon>(hero_id, ENTITYID_INVALID);
    }
    else {
        ct.set<equipped_weapon>(hero_id, item_id);
    }
    flag = GAMESTATE_FLAG_PLAYER_ANIM;
    controlmode = CONTROLMODE_PLAYER;
    display_inventory_menu = false;
}

inline void gamestate::handle_hero_inventory_equip_shield(entityid item_id) {
    entityid current_shield = ct.get<equipped_shield>(hero_id).value_or(ENTITYID_INVALID);
    if (current_shield == item_id) {
        ct.set<equipped_shield>(hero_id, ENTITYID_INVALID);
    }
    else {
        ct.set<equipped_shield>(hero_id, item_id);
    }
    flag = GAMESTATE_FLAG_PLAYER_ANIM;
    controlmode = CONTROLMODE_PLAYER;
    display_inventory_menu = false;
}

inline void gamestate::handle_hero_inventory_equip_item(entityid item_id) {
    itemtype_t item_type = ct.get<itemtype>(item_id).value_or(ITEM_NONE);
    switch (item_type) {
    case ITEM_NONE: break;
    case ITEM_WEAPON: handle_hero_inventory_equip_weapon(item_id); break;
    case ITEM_SHIELD: handle_hero_inventory_equip_shield(item_id); break;
    default: break;
    }
}

inline void gamestate::handle_hero_inventory_equip() {
    PlaySound(sfx.at(SFX_EQUIP_01));
    size_t index = inventory_cursor.y * 7 + inventory_cursor.x;
    auto my_inventory = ct.get<inventory>(hero_id);
    if (!my_inventory || !my_inventory.has_value()) {
        return;
    }
    auto inventory = my_inventory.value();
    if (index >= inventory->size()) {
        return;
    }
    entityid item_id = inventory->at(index);
    entitytype_t type = ct.get<entitytype>(item_id).value_or(ENTITY_NONE);
    if (type == ENTITY_ITEM) {
        handle_hero_inventory_equip_item(item_id);
    }
}

inline bool gamestate::drop_item_from_hero_inventory() {
    if (!ct.has<inventory>(hero_id)) {
        return false;
    }
    size_t index = inventory_cursor.y * 7 + inventory_cursor.x;
    auto maybe_inventory = ct.get<inventory>(hero_id);
    if (!maybe_inventory.has_value()) {
        return false;
    }
    auto inventory = maybe_inventory.value();
    if (index >= inventory->size()) {
        return false;
    }
    entityid item_id = inventory->at(index);
    inventory->erase(inventory->begin() + index);
    if (item_id == ct.get<equipped_weapon>(hero_id).value_or(ENTITYID_INVALID)) {
        ct.set<equipped_weapon>(hero_id, ENTITYID_INVALID);
    }
    if (item_id == ct.get<equipped_shield>(hero_id).value_or(ENTITYID_INVALID)) {
        ct.set<equipped_shield>(hero_id, ENTITYID_INVALID);
    }
    vec3 loc = ct.get<location>(hero_id).value();
    auto df = d.get_current_floor();
    if (!df->df_add_at(item_id, ENTITY_ITEM, loc)) {
        merror("Failed to add to %d, %d, %d", loc.x, loc.y, loc.z);
        return false;
    }
    ct.set<location>(item_id, loc);
    return true;
}

inline bool gamestate::is_in_inventory(entityid actor_id, entityid item_id) {
    auto maybe_inventory = ct.get<inventory>(actor_id);
    if (!maybe_inventory.has_value()) {
        merror("maybe_inventory has no value for actor id %d", actor_id);
        return false;
    }
    auto my_items = maybe_inventory.value();
    for (auto it = my_items->begin(); it != my_items->end(); it++) {
        if (*it == item_id) {
            return true;
        }
    }
    return false;
}

inline bool gamestate::use_potion(entityid actor_id, entityid item_id) {
    massert(actor_id != ENTITYID_INVALID, "actor_id is invalid");
    massert(item_id != ENTITYID_INVALID, "actor_id is invalid");
    bool is_item = ct.get<entitytype>(item_id).value_or(ENTITY_NONE) == ENTITY_ITEM;
    bool is_potion = ct.get<itemtype>(item_id).value_or(ITEM_NONE) == ITEM_POTION;
    bool in_inventory = is_in_inventory(actor_id, item_id);
    if (is_item && is_potion && in_inventory) {
        optional<vec3> maybe_heal = ct.get<healing>(item_id);
        if (maybe_heal && maybe_heal.has_value()) {
            vec3 heal = maybe_heal.value();
            int amount = do_roll(heal);
            int myhp = ct.get<hp>(actor_id).value_or(-1);
            int mymaxhp = ct.get<maxhp>(actor_id).value_or(-1);
            ct.set<hp>(actor_id, mymaxhp ? mymaxhp : myhp + amount);
            if (actor_id == hero_id) {
                string n = ct.get<name>(actor_id).value_or("no-name");
                add_message_history("%s used a healing potion", n.c_str());
                add_message_history("%s restored %d hp", n.c_str(), amount);
            }
        }
        else {
            merror("Potion has no healing component");
            return false;
        }
        remove_from_inventory(actor_id, item_id);
        return true;
    }
    merror("id %d is not an item, potion, or isnt in the inventory", item_id);
    return false;
}

inline void gamestate::handle_hero_potion_use(entityid id) {
    entitytype_t type = ct.get<entitytype>(id).value_or(ENTITY_NONE);
    if (type != ENTITY_ITEM) {
        return;
    }
    itemtype_t i_type = ct.get<itemtype>(id).value_or(ITEM_NONE);
    if (i_type == ITEM_NONE || i_type != ITEM_POTION) {
        return;
    }
    if (use_potion(hero_id, id)) {
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        controlmode = CONTROLMODE_PLAYER;
        display_inventory_menu = false;
    }
}

inline void gamestate::handle_hero_item_use() {
    size_t index = inventory_cursor.y * 7 + inventory_cursor.x;
    optional<shared_ptr<vector<entityid>>> maybe_inventory = ct.get<inventory>(hero_id);
    if (!maybe_inventory || !maybe_inventory.has_value()) {
        return;
    }
    shared_ptr<vector<entityid>> inventory = maybe_inventory.value();
    if (index >= inventory->size()) {
        return;
    }
    entityid item_id = inventory->at(index);
    entitytype_t type = ct.get<entitytype>(item_id).value_or(ENTITY_NONE);
    if (type != ENTITY_ITEM) {
        return;
    }
    itemtype_t i_type = ct.get<itemtype>(item_id).value_or(ITEM_NONE);
    if (i_type == ITEM_NONE) {
        return;
    }
    if (i_type == ITEM_POTION) {
        handle_hero_potion_use(item_id);
    }
}
