#pragma once

inline void gamestate::handle_camera_move(inputstate& is) {
    if (inputstate_is_held(is, KEY_RIGHT)) {
        cam2d.offset.x += cam2d.zoom;
        frame_dirty = true;
    }
    else if (inputstate_is_held(is, KEY_LEFT)) {
        cam2d.offset.x -= cam2d.zoom;
        frame_dirty = true;
    }
    else if (inputstate_is_held(is, KEY_UP)) {
        cam2d.offset.y -= cam2d.zoom;
        frame_dirty = true;
    }
    else if (inputstate_is_held(is, KEY_DOWN)) {
        cam2d.offset.y += cam2d.zoom;
        frame_dirty = true;
    }
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
    if (index < 0) {
        return;
    }
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

inline void gamestate::handle_input_inventory(inputstate& is) {
    if (controlmode != CONTROLMODE_INVENTORY || !display_inventory_menu) {
        return;
    }
    if (inputstate_is_pressed(is, KEY_I) || inputstate_is_pressed(is, KEY_ESCAPE)) {
        controlmode = CONTROLMODE_PLAYER;
        display_inventory_menu = false;
        PlaySound(sfx[SFX_BAG_CLOSE]);
        return;
    }
    if (inputstate_is_pressed(is, KEY_LEFT)) {
        PlaySound(sfx[SFX_CONFIRM_01]);
        if (inventory_cursor.x > 0) {
            inventory_cursor.x--;
        }
    }
    else if (inputstate_is_pressed(is, KEY_RIGHT)) {
        PlaySound(sfx[SFX_CONFIRM_01]);
        inventory_cursor.x++;
    }
    else if (inputstate_is_pressed(is, KEY_UP)) {
        PlaySound(sfx[SFX_CONFIRM_01]);
        if (inventory_cursor.y > 0) {
            inventory_cursor.y--;
        }
    }
    else if (inputstate_is_pressed(is, KEY_DOWN)) {
        PlaySound(sfx[SFX_CONFIRM_01]);
        inventory_cursor.y++;
    }
    else if (inputstate_is_pressed(is, KEY_E)) {
        handle_hero_inventory_equip();
    }
    else if (inputstate_is_pressed(is, KEY_Q)) {
        PlaySound(sfx[SFX_DISCARD_ITEM]);
        drop_item_from_hero_inventory();
    }
    else if (inputstate_is_pressed(is, KEY_ENTER)) {
        handle_hero_item_use();
        PlaySound(sfx[SFX_CONFIRM_01]);
    }
}

inline void gamestate::cycle_messages() {
    if (msg_system.size() > 0) {
        string msg = msg_system.front();
        unsigned int len = msg.length();
        if (len > msg_history_max_len_msg) {
            msg_history_max_len_msg = len;
            constexpr int font_size = DEFAULT_MSG_HISTORY_FONT_SIZE;
            int measure = MeasureText(msg.c_str(), font_size);
            msg_history_max_len_msg_measure = measure;
        }
        msg_history.push_back(msg_system.front());
        msg_system.erase(msg_system.begin());
    }
    if (msg_system.size() == 0) {
        msg_system_is_active = false;
    }
}

inline bool gamestate::handle_cycle_messages(inputstate& is) {
    if (msg_system_is_active && inputstate_is_pressed(is, KEY_ENTER)) {
        PlaySound(sfx[SFX_CONFIRM_01]);
        cycle_messages();
        return true;
    }
    return false;
}

inline bool gamestate::handle_cycle_messages_test() {
    if (!msg_system_is_active) {
        return false;
    }
    PlaySound(sfx[SFX_CONFIRM_01]);
    cycle_messages();
    return true;
}

inline void gamestate::handle_camera_zoom(inputstate& is) {
    if (inputstate_is_pressed(is, KEY_MINUS)) {
        minfo("camera zoom in");
        cam2d.zoom += DEFAULT_ZOOM_INCR;
        frame_dirty = true;
    }
    else if (inputstate_is_pressed(is, KEY_EQUAL)) {
        minfo("camera zoom out");
        cam2d.zoom -= (cam2d.zoom > 1.0) * DEFAULT_ZOOM_INCR;
        frame_dirty = true;
    }
}

inline void gamestate::change_player_dir(direction_t dir) {
    if (ct.get<dead>(hero_id).value_or(true)) {
        return;
    }
    ct.set<direction>(hero_id, dir);
    ct.set<update>(hero_id, true);
    player_changing_dir = false;
    frame_dirty = true;
}

inline bool gamestate::handle_change_dir(inputstate& is) {
    if (!player_changing_dir) {
        return false;
    }
    optional<bool> maybe_player_is_dead = ct.get<dead>(hero_id);
    if (!maybe_player_is_dead.has_value()) {
        return true;
    }
    bool is_dead = maybe_player_is_dead.value();
    if (inputstate_is_pressed(is, KEY_S) || inputstate_is_pressed(is, KEY_KP_5)) {
        player_changing_dir = false;
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
    }
    else if (inputstate_is_pressed(is, KEY_UP) || inputstate_is_pressed(is, KEY_W) || inputstate_is_pressed(is, KEY_KP_8)) {
        change_player_dir(DIR_UP);
    }
    else if (inputstate_is_pressed(is, KEY_DOWN) || inputstate_is_pressed(is, KEY_X) || inputstate_is_pressed(is, KEY_KP_2)) {
        change_player_dir(DIR_DOWN);
    }
    else if (inputstate_is_pressed(is, KEY_LEFT) || inputstate_is_pressed(is, KEY_A) || inputstate_is_pressed(is, KEY_KP_4)) {
        change_player_dir(DIR_LEFT);
    }
    else if (inputstate_is_pressed(is, KEY_RIGHT) || inputstate_is_pressed(is, KEY_D) || inputstate_is_pressed(is, KEY_KP_6)) {
        change_player_dir(DIR_RIGHT);
    }
    else if (inputstate_is_pressed(is, KEY_Q) || inputstate_is_pressed(is, KEY_KP_7)) {
        change_player_dir(DIR_UP_LEFT);
    }
    else if (inputstate_is_pressed(is, KEY_E) || inputstate_is_pressed(is, KEY_KP_9)) {
        change_player_dir(DIR_UP_RIGHT);
    }
    else if (inputstate_is_pressed(is, KEY_Z) || inputstate_is_pressed(is, KEY_KP_1)) {
        change_player_dir(DIR_DOWN_LEFT);
    }
    else if (inputstate_is_pressed(is, KEY_C) || inputstate_is_pressed(is, KEY_KP_3)) {
        change_player_dir(DIR_DOWN_RIGHT);
    }
    else if (inputstate_is_pressed(is, KEY_APOSTROPHE)) {
        if (is_dead) {
            return add_message("You cannot attack while dead");
        }
        ct.set<attacking>(hero_id, true);
        ct.set<update>(hero_id, true);
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        player_changing_dir = false;
    }
    return true;
}

inline bool gamestate::handle_change_dir_intent(inputstate& is) {
    if (inputstate_is_pressed(is, KEY_KP_5)) {
        player_changing_dir = true;
        return true;
    }
    return false;
}

inline bool gamestate::handle_display_inventory(inputstate& is) {
    if (inputstate_is_pressed(is, KEY_I)) {
        display_inventory_menu = true;
        controlmode = CONTROLMODE_INVENTORY;
        frame_dirty = true;
        PlaySound(sfx[SFX_BAG_OPEN]);
        return true;
    }
    return false;
}

inline bool gamestate::handle_restart(inputstate& is) {
    if (inputstate_is_pressed(is, KEY_R)) {
        do_restart = true;
        return true;
    }
    return false;
}

inline void gamestate::handle_input_gameplay_controlmode_player(inputstate& is) {
    if (flag != GAMESTATE_FLAG_PLAYER_INPUT) {
        return;
    }
    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        do_quit = true;
        return;
    }
    handle_camera_zoom(is);
    if (handle_cycle_messages(is)) {
        return;
    }
    if (inputstate_is_pressed(is, KEY_SLASH)) {
        display_help_menu = true;
        controlmode = CONTROLMODE_HELP_MENU;
    }
    bool is_dead = ct.get<dead>(hero_id).value_or(true);
    if (is_dead) {
        handle_restart(is);
        return;
    }
    if (test && framecount % 20 == 0) {
        if (handle_cycle_messages_test()) {
            return;
        }
        bool test_is_dead = ct.get<dead>(hero_id).value_or(true);
        if (test_is_dead) {
            return;
        }
        uniform_int_distribution<int> dist(-1, 1);
        int vx = dist(mt);
        int vy = dist(mt);
        while (vx == 0 && vy == 0) {
            vx = dist(mt);
            vy = dist(mt);
        }
        if (try_entity_move(hero_id, vec3{vx, vy, 0})) {
            msuccess2("hero moved randomly successfully");
        }
        else {
            minfo2("hero failed to move randomly");
        }
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return;
    }
    else if (test) {
        return;
    }
    else if (inputstate_is_pressed(is, KEY_GRAVE)) {
        display_option_menu = true;
        controlmode = CONTROLMODE_OPTION_MENU;
        return;
    }
    else if (inputstate_is_pressed(is, KEY_SPACE)) {
        try_entity_pull(hero_id);
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return;
    }
    else if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        do_quit = true;
        return;
    }
    if (handle_change_dir(is) || handle_change_dir_intent(is) || handle_display_inventory(is)) {
        return;
    }
    else if (handle_move_up(is, is_dead) || handle_move_down(is, is_dead) || handle_move_left(is, is_dead) || handle_move_right(is, is_dead)) {
        return;
    }
    else if (
        handle_move_up_left(is, is_dead) || handle_move_up_right(is, is_dead) || handle_move_down_left(is, is_dead) ||
        handle_move_down_right(is, is_dead)) {
        return;
    }
    else if (handle_attack(is, is_dead) || handle_pickup_item(is, is_dead) || handle_traverse_stairs(is, is_dead) || handle_open_door(is, is_dead)) {
        return;
    }
}

inline void gamestate::handle_input_action_menu(inputstate& is) {
    massert(controlmode == CONTROLMODE_ACTION_MENU, "controlmode isnt in action menu: %d", controlmode);
    if (inputstate_is_pressed(is, KEY_SPACE)) {
        display_action_menu = false;
        controlmode = CONTROLMODE_PLAYER;
        return;
    }
    else if (inputstate_is_pressed(is, KEY_DOWN)) {
        action_selection++;
    }
    else if (inputstate_is_pressed(is, KEY_UP)) {
        action_selection--;
    }
}

inline void gamestate::handle_input_option_menu(inputstate& is) {
    massert(controlmode == CONTROLMODE_OPTION_MENU, "controlmode isnt in option menu: %d", controlmode);
    if (inputstate_is_pressed(is, KEY_GRAVE)) {
        display_option_menu = false;
        controlmode = CONTROLMODE_PLAYER;
    }
    else if (inputstate_is_pressed(is, KEY_UP)) {
        options_menu.decr_selection();
    }
    else if (inputstate_is_pressed(is, KEY_DOWN)) {
        options_menu.incr_selection();
    }
    else if (inputstate_is_pressed(is, KEY_ENTER)) {
        minfo("Enter pressed");
    }
}

inline void gamestate::handle_input_help_menu(inputstate& is) {
    massert(controlmode == CONTROLMODE_HELP_MENU, "controlmode isnt in help menu: %d", controlmode);
    if (inputstate_any_pressed(is)) {
        display_help_menu = false;
        controlmode = CONTROLMODE_PLAYER;
    }
}

inline void gamestate::handle_input_gameplay_scene(inputstate& is) {
    minfo2("handle input gameplay scene");
    if (inputstate_is_pressed(is, KEY_B)) {
        if (controlmode == CONTROLMODE_PLAYER) {
            controlmode = CONTROLMODE_CAMERA;
        }
        else if (controlmode == CONTROLMODE_CAMERA) {
            controlmode = CONTROLMODE_PLAYER;
        }
        frame_dirty = true;
    }

    if (controlmode == CONTROLMODE_CAMERA) {
        handle_camera_move(is);
    }
    else if (controlmode == CONTROLMODE_PLAYER) {
        handle_input_gameplay_controlmode_player(is);
    }
    else if (controlmode == CONTROLMODE_INVENTORY) {
        handle_input_inventory(is);
    }
    else if (controlmode == CONTROLMODE_ACTION_MENU) {
        handle_input_action_menu(is);
    }
    else if (controlmode == CONTROLMODE_OPTION_MENU) {
        handle_input_option_menu(is);
    }
    else if (controlmode == CONTROLMODE_HELP_MENU) {
        handle_input_help_menu(is);
    }
}

inline void gamestate::handle_input(inputstate& is) {
    minfo2("handle input: current_scene: %d", current_scene);
    if (inputstate_is_pressed(is, KEY_P)) {
        debugpanelon = !debugpanelon;
        minfo2("Toggling debug panel: %s", debugpanelon ? "ON" : "OFF");
    }
    if (current_scene == SCENE_TITLE) {
        handle_input_title_scene(is);
    }
    else if (current_scene == SCENE_MAIN_MENU) {
        handle_input_main_menu_scene(is);
    }
    else if (current_scene == SCENE_CHARACTER_CREATION) {
        handle_input_character_creation_scene(is);
    }
    else if (current_scene == SCENE_GAMEPLAY) {
        handle_input_gameplay_scene(is);
    }
}
