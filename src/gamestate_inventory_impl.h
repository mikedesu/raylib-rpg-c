/** @file gamestate_inventory_impl.h
 *  @brief Inventory/chest management helpers implemented on `gamestate`.
 */

#pragma once

inline bool gamestate::use_mini_inventory_menu() const {
    return prefer_mini_inventory_menu;
}

inline size_t gamestate::get_inventory_selection_index() const {
    if (!use_mini_inventory_menu()) {
        return static_cast<size_t>(inventory_cursor.y) * 7 + static_cast<size_t>(inventory_cursor.x);
    }
    return static_cast<size_t>(mini_inventory_scroll_offset) + static_cast<size_t>(inventory_cursor.y);
}

inline void gamestate::clamp_inventory_selection(size_t item_count) {
    if (!use_mini_inventory_menu()) {
        if (item_count == 0) {
            inventory_cursor = Vector2{0, 0};
            return;
        }
        const size_t max_index = item_count - 1;
        size_t index = static_cast<size_t>(inventory_cursor.y) * 7 + static_cast<size_t>(inventory_cursor.x);
        if (index > max_index) {
            inventory_cursor.x = static_cast<float>(max_index % 7);
            inventory_cursor.y = static_cast<float>(max_index / 7);
        }
        return;
    }

    if (item_count == 0) {
        mini_inventory_scroll_offset = 0;
        inventory_cursor = Vector2{0, 0};
        return;
    }

    const size_t visible_count = std::max(1U, mini_inventory_visible_count);
    const size_t max_index = item_count - 1;
    size_t index = get_inventory_selection_index();
    if (index > max_index) {
        index = max_index;
    }
    const size_t max_scroll = item_count > visible_count ? item_count - visible_count : 0;
    if (index < mini_inventory_scroll_offset) {
        mini_inventory_scroll_offset = static_cast<unsigned int>(index);
    }
    else if (index >= mini_inventory_scroll_offset + visible_count) {
        mini_inventory_scroll_offset = static_cast<unsigned int>(index - visible_count + 1);
    }
    mini_inventory_scroll_offset = static_cast<unsigned int>(std::min(static_cast<size_t>(mini_inventory_scroll_offset), max_scroll));
    inventory_cursor.x = 0.0f;
    inventory_cursor.y = static_cast<float>(index - mini_inventory_scroll_offset);
}

inline void gamestate::move_inventory_selection(int delta) {
    if (!use_mini_inventory_menu()) {
        if (delta < 0) {
            if (inventory_cursor.x > 0) {
                inventory_cursor.x--;
            }
            else if (inventory_cursor.y > 0) {
                inventory_cursor.y--;
                inventory_cursor.x = 6;
            }
        }
        else if (delta > 0) {
            inventory_cursor.x++;
            if (inventory_cursor.x > 6) {
                inventory_cursor.x = 0;
                inventory_cursor.y++;
            }
        }
        return;
    }

    int next_index = static_cast<int>(get_inventory_selection_index()) + delta;
    if (next_index < 0) {
        next_index = 0;
    }
    const size_t current_index = static_cast<size_t>(next_index);
    mini_inventory_scroll_offset = static_cast<unsigned int>(std::min(static_cast<size_t>(mini_inventory_scroll_offset), current_index));
    inventory_cursor.x = 0.0f;
    inventory_cursor.y = static_cast<float>(current_index - mini_inventory_scroll_offset);
}

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
    size_t index = get_inventory_selection_index();
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
    size_t index = get_inventory_selection_index();
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
            vec2 actor_hp = ct.get<hp>(actor_id).value_or(vec2{-1, -1});
            if (vec2_invalid(actor_hp)) {
                merror("actor has no hp component");
                return false;
            }
            actor_hp.x = std::min(actor_hp.y, actor_hp.x + amount);
            ct.set<hp>(actor_id, actor_hp);
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
    run_use_item_action(hero_id, id);
}

inline void gamestate::handle_hero_item_use() {
    size_t index = get_inventory_selection_index();
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

inline bool gamestate::transfer_inventory_item(entityid from_id, entityid to_id, entityid item_id) {
    if (from_id == ENTITYID_INVALID || to_id == ENTITYID_INVALID || item_id == ENTITYID_INVALID) {
        return false;
    }
    if (!is_in_inventory(from_id, item_id)) {
        return false;
    }
    if (!remove_from_inventory(from_id, item_id)) {
        return false;
    }
    if (!add_to_inventory(to_id, item_id)) {
        add_to_inventory(from_id, item_id);
        return false;
    }
    if (from_id == hero_id) {
        if (item_id == ct.get<equipped_weapon>(hero_id).value_or(ENTITYID_INVALID)) {
            ct.set<equipped_weapon>(hero_id, ENTITYID_INVALID);
        }
        if (item_id == ct.get<equipped_shield>(hero_id).value_or(ENTITYID_INVALID)) {
            ct.set<equipped_shield>(hero_id, ENTITYID_INVALID);
        }
    }
    return true;
}

inline bool gamestate::open_chest_menu(entityid chest_id) {
    if (chest_id == ENTITYID_INVALID || ct.get<entitytype>(chest_id).value_or(ENTITY_NONE) != ENTITY_CHEST) {
        return false;
    }
    ct.set<door_open>(chest_id, true);
    ct.set<update>(chest_id, true);
    active_chest_id = chest_id;
    display_chest_menu = true;
    display_inventory_menu = false;
    chest_deposit_mode = false;
    mini_inventory_scroll_offset = 0;
    inventory_cursor = Vector2{0, 0};
    controlmode = CONTROLMODE_CHEST;
    frame_dirty = true;
    if (IsAudioDeviceReady() && sfx.size() > SFX_CHEST_OPEN) {
        PlaySound(sfx.at(SFX_CHEST_OPEN));
    }
    return true;
}

inline void gamestate::close_chest_menu() {
    if (active_chest_id != ENTITYID_INVALID) {
        ct.set<door_open>(active_chest_id, false);
        ct.set<update>(active_chest_id, true);
    }
    display_chest_menu = false;
    chest_deposit_mode = false;
    active_chest_id = ENTITYID_INVALID;
    controlmode = CONTROLMODE_PLAYER;
    frame_dirty = true;
}

inline void gamestate::toggle_chest_menu_mode() {
    if (!display_chest_menu || active_chest_id == ENTITYID_INVALID) {
        return;
    }
    chest_deposit_mode = !chest_deposit_mode;
    mini_inventory_scroll_offset = 0;
    inventory_cursor = Vector2{0, 0};
    frame_dirty = true;
}

inline void gamestate::handle_chest_menu_confirm() {
    if (!display_chest_menu || active_chest_id == ENTITYID_INVALID) {
        return;
    }
    entityid source_id = chest_deposit_mode ? hero_id : active_chest_id;
    entityid target_id = chest_deposit_mode ? active_chest_id : hero_id;
    auto maybe_inventory = ct.get<inventory>(source_id);
    if (!maybe_inventory.has_value()) {
        return;
    }
    auto items = maybe_inventory.value();
    size_t index = get_inventory_selection_index();
    if (index >= items->size()) {
        return;
    }
    entityid item_id = items->at(index);
    if (transfer_inventory_item(source_id, target_id, item_id)) {
        frame_dirty = true;
        if (IsAudioDeviceReady() && sfx.size() > SFX_CONFIRM_01) {
            PlaySound(sfx.at(SFX_CONFIRM_01));
        }
        auto updated_inventory = ct.get<inventory>(source_id).value_or(make_shared<vector<entityid>>());
        clamp_inventory_selection(updated_inventory->size());
    }
}

inline void gamestate::handle_input_chest(inputstate& is) {
    if (controlmode != CONTROLMODE_CHEST || !display_chest_menu) {
        return;
    }
    if (inputstate_is_pressed(is, KEY_D) || inputstate_is_pressed(is, KEY_ESCAPE)) {
        close_chest_menu();
        return;
    }
    if (inputstate_is_pressed(is, KEY_TAB)) {
        toggle_chest_menu_mode();
        return;
    }
    if (inputstate_is_pressed(is, KEY_LEFT)) {
        if (use_mini_inventory_menu()) {
            move_inventory_selection(-1);
        }
        else if (inventory_cursor.x > 0) {
            inventory_cursor.x--;
        }
    }
    else if (inputstate_is_pressed(is, KEY_RIGHT)) {
        if (use_mini_inventory_menu()) {
            move_inventory_selection(1);
        }
        else {
            inventory_cursor.x++;
        }
    }
    else if (inputstate_is_pressed(is, KEY_UP)) {
        if (use_mini_inventory_menu()) {
            move_inventory_selection(-1);
        }
        else if (inventory_cursor.y > 0) {
            inventory_cursor.y--;
        }
    }
    else if (inputstate_is_pressed(is, KEY_DOWN)) {
        if (use_mini_inventory_menu()) {
            move_inventory_selection(1);
        }
        else {
            inventory_cursor.y++;
        }
    }
    else if (inputstate_is_pressed(is, KEY_ENTER)) {
        handle_chest_menu_confirm();
    }
    auto items = ct.get<inventory>(chest_deposit_mode ? hero_id : active_chest_id).value_or(make_shared<vector<entityid>>());
    clamp_inventory_selection(items->size());
    frame_dirty = true;
}
