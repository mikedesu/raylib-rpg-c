#pragma once

inline void gamestate::open_confirm_prompt(confirm_action_t action, const char* fmt, ...) {
    massert(fmt, "format string is NULL");
    char buffer[MAX_MSG_LENGTH] = {0};
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, MAX_MSG_LENGTH - 1, fmt, args);
    va_end(args);
    confirm_prompt_message = buffer;
    confirm_action = action;
    display_confirm_prompt = true;
    controlmode_before_confirm = controlmode;
    controlmode = CONTROLMODE_CONFIRM_PROMPT;
    frame_dirty = true;
}

inline void gamestate::handle_confirm_quit() {
#ifdef WEB
    restart_game();
#else
    do_quit = true;
#endif
    frame_dirty = true;
}

inline void gamestate::resolve_confirm_prompt(bool confirmed) {
    const confirm_action_t action = confirm_action;
    display_confirm_prompt = false;
    confirm_action = CONFIRM_ACTION_NONE;
    confirm_prompt_message.clear();
    controlmode = controlmode_before_confirm == CONTROLMODE_CONFIRM_PROMPT ? CONTROLMODE_PLAYER : controlmode_before_confirm;
    controlmode_before_confirm = CONTROLMODE_PLAYER;
    frame_dirty = true;
    if (!confirmed) {
        return;
    }
    switch (action) {
    case CONFIRM_ACTION_QUIT:
        handle_confirm_quit();
        break;
    case CONFIRM_ACTION_NONE:
    default:
        break;
    }
}

inline void gamestate::open_interaction_modal(entityid id, const string& title, const string& body) {
    active_interaction_entity_id = id;
    interaction_title = title;
    interaction_body = body;
    display_interaction_modal = true;
    controlmode_before_confirm = controlmode;
    controlmode = CONTROLMODE_INTERACTION;
    frame_dirty = true;
}

inline void gamestate::close_interaction_modal() {
    display_interaction_modal = false;
    active_interaction_entity_id = ENTITYID_INVALID;
    interaction_title.clear();
    interaction_body.clear();
    controlmode = controlmode_before_confirm == CONTROLMODE_INTERACTION ? CONTROLMODE_PLAYER : controlmode_before_confirm;
    controlmode_before_confirm = CONTROLMODE_PLAYER;
    frame_dirty = true;
}

inline void gamestate::handle_input_interaction(inputstate& is) {
    if (controlmode != CONTROLMODE_INTERACTION || !display_interaction_modal) {
        return;
    }
    if (inputstate_any_pressed(is)) {
        if (!test && IsAudioDeviceReady()) {
            PlaySound(sfx[SFX_CONFIRM_01]);
        }
        close_interaction_modal();
    }
}

inline int gamestate::get_level_up_xp_threshold(entityid id) {
    const int actor_level = ct.get<level>(id).value_or(1) < 1 ? 1 : ct.get<level>(id).value_or(1);
    return actor_level * 10;
}

inline void gamestate::open_level_up_modal() {
    display_level_up_modal = true;
    level_up_selection = 0;
    controlmode = CONTROLMODE_LEVEL_UP;
    frame_dirty = true;
}

inline bool gamestate::apply_permanent_attribute_increase(entityid id, unsigned int attribute_index, int amount) {
    if (id == ENTITYID_INVALID || amount == 0) {
        return false;
    }

    switch (attribute_index % 6) {
    case 0:
        ct.set<strength>(id, ct.get<strength>(id).value_or(10) + amount);
        return true;
    case 1:
        ct.set<dexterity>(id, ct.get<dexterity>(id).value_or(10) + amount);
        return true;
    case 2:
        ct.set<constitution>(id, ct.get<constitution>(id).value_or(10) + amount);
        return true;
    case 3:
        ct.set<intelligence>(id, ct.get<intelligence>(id).value_or(10) + amount);
        return true;
    case 4:
        ct.set<wisdom>(id, ct.get<wisdom>(id).value_or(10) + amount);
        return true;
    case 5:
        ct.set<charisma>(id, ct.get<charisma>(id).value_or(10) + amount);
        return true;
    default:
        break;
    }
    return false;
}

inline int gamestate::roll_level_up_max_hp_gain(entityid id) {
    if (id == ENTITYID_INVALID) {
        return 0;
    }
    const vec3 hitdie = ct.get<hd>(id).value_or(vec3{1, 1, 0});
    const int rolled = do_roll(hitdie);
    return rolled > 0 ? rolled : 1;
}

inline void gamestate::apply_level_up_rewards(entityid id) {
    if (id == ENTITYID_INVALID) {
        return;
    }

    const int new_level = ct.get<level>(id).value_or(1) + 1;
    ct.set<level>(id, new_level);

    vec2 hp_value = ct.get<hp>(id).value_or(vec2{1, 1});
    const int hp_gain = roll_level_up_max_hp_gain(id);
    hp_value.y += hp_gain;
    if (hp_value.y < 1) {
        hp_value.y = 1;
    }
    if (hp_value.x > hp_value.y) {
        hp_value.x = hp_value.y;
    }
    ct.set<hp>(id, hp_value);
}

inline void gamestate::apply_level_up_selection() {
    if (hero_id == ENTITYID_INVALID || !display_level_up_modal) {
        return;
    }

    const char* stat_name = "strength";
    switch (level_up_selection % 6) {
    case 0:
        stat_name = "strength";
        break;
    case 1:
        stat_name = "dexterity";
        break;
    case 2:
        stat_name = "constitution";
        break;
    case 3:
        stat_name = "intelligence";
        break;
    case 4:
        stat_name = "wisdom";
        break;
    case 5:
        stat_name = "charisma";
        break;
    default:
        break;
    }

    if (!apply_permanent_attribute_increase(hero_id, level_up_selection, 1)) {
        return;
    }
    apply_level_up_rewards(hero_id);
    pending_level_ups = pending_level_ups > 0 ? pending_level_ups - 1 : 0;
    display_level_up_modal = false;
    controlmode = CONTROLMODE_PLAYER;
    add_message("%s increased by 1", stat_name);
    add_message_history("%s reached level %d", ct.get<name>(hero_id).value_or("hero").c_str(), ct.get<level>(hero_id).value_or(1));
    frame_dirty = true;

    if (pending_level_ups > 0) {
        open_level_up_modal();
    }
}

inline void gamestate::maybe_unlock_level_up(entityid id) {
    if (id != hero_id || ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_PLAYER) {
        return;
    }

    const int current_xp = ct.get<xp>(id).value_or(0);
    int current_level = ct.get<level>(id).value_or(1) + pending_level_ups;
    if (current_level < 1) {
        current_level = 1;
    }
    while (current_xp >= current_level * 10) {
        pending_level_ups++;
        current_level++;
    }

    if (pending_level_ups > 0 && !display_level_up_modal) {
        open_level_up_modal();
    }
}

inline void gamestate::handle_input_level_up(inputstate& is) {
    if (controlmode != CONTROLMODE_LEVEL_UP || !display_level_up_modal) {
        return;
    }
    if (inputstate_is_pressed(is, KEY_LEFT)) {
        if (level_up_selection % 2 == 1) {
            level_up_selection--;
        }
        frame_dirty = true;
    }
    else if (inputstate_is_pressed(is, KEY_RIGHT)) {
        if (level_up_selection % 2 == 0) {
            level_up_selection++;
        }
        frame_dirty = true;
    }
    else if (inputstate_is_pressed(is, KEY_UP)) {
        if (level_up_selection >= 2) {
            level_up_selection -= 2;
        }
        frame_dirty = true;
    }
    else if (inputstate_is_pressed(is, KEY_DOWN)) {
        if (level_up_selection + 2 < 6) {
            level_up_selection += 2;
        }
        frame_dirty = true;
    }
    else if (inputstate_is_pressed(is, KEY_ENTER)) {
        if (!test && IsAudioDeviceReady()) {
            PlaySound(sfx[SFX_CONFIRM_01]);
        }
        apply_level_up_selection();
    }
}

inline void gamestate::handle_input_confirm_prompt(inputstate& is) {
    if (controlmode != CONTROLMODE_CONFIRM_PROMPT) {
        controlmode = CONTROLMODE_CONFIRM_PROMPT;
    }
    if (inputstate_is_pressed(is, KEY_Y)) {
        if (!test && IsAudioDeviceReady()) {
            PlaySound(sfx[SFX_CONFIRM_01]);
        }
        resolve_confirm_prompt(true);
    }
    else if (inputstate_is_pressed(is, KEY_N) || inputstate_is_pressed(is, KEY_ESCAPE)) {
        if (!test && IsAudioDeviceReady()) {
            PlaySound(sfx[SFX_CONFIRM_01]);
        }
        resolve_confirm_prompt(false);
    }
}

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
    if (is_action_pressed(is, INPUT_ACTION_ZOOM_IN)) {
        minfo("camera zoom in");
        cam2d.zoom += DEFAULT_ZOOM_INCR;
        frame_dirty = true;
    }
    else if (is_action_pressed(is, INPUT_ACTION_ZOOM_OUT)) {
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
    if (is_action_pressed(is, INPUT_ACTION_FACE_WAIT)) {
        player_changing_dir = false;
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
    }
    else if (is_action_pressed(is, INPUT_ACTION_FACE_UP)) {
        change_player_dir(DIR_UP);
    }
    else if (is_action_pressed(is, INPUT_ACTION_FACE_DOWN)) {
        change_player_dir(DIR_DOWN);
    }
    else if (is_action_pressed(is, INPUT_ACTION_FACE_LEFT)) {
        change_player_dir(DIR_LEFT);
    }
    else if (is_action_pressed(is, INPUT_ACTION_FACE_RIGHT)) {
        change_player_dir(DIR_RIGHT);
    }
    else if (is_action_pressed(is, INPUT_ACTION_FACE_UP_LEFT)) {
        change_player_dir(DIR_UP_LEFT);
    }
    else if (is_action_pressed(is, INPUT_ACTION_FACE_UP_RIGHT)) {
        change_player_dir(DIR_UP_RIGHT);
    }
    else if (is_action_pressed(is, INPUT_ACTION_FACE_DOWN_LEFT)) {
        change_player_dir(DIR_DOWN_LEFT);
    }
    else if (is_action_pressed(is, INPUT_ACTION_FACE_DOWN_RIGHT)) {
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
    if (is_action_pressed(is, INPUT_ACTION_DIRECTION_MODE)) {
        player_changing_dir = true;
        return true;
    }
    return false;
}

inline bool gamestate::handle_display_inventory(inputstate& is) {
    if (is_action_pressed(is, INPUT_ACTION_INVENTORY)) {
        display_inventory_menu = true;
        controlmode = CONTROLMODE_INVENTORY;
        frame_dirty = true;
        PlaySound(sfx[SFX_BAG_OPEN]);
        return true;
    }
    return false;
}

inline bool gamestate::handle_toggle_full_light(inputstate& is) {
    if (!is_action_pressed(is, INPUT_ACTION_TOGGLE_FULL_LIGHT)) {
        return false;
    }
    shared_ptr<dungeon_floor> df = d.get_current_floor();
    massert(df, "current floor is null");
    df->toggle_full_light();
    add_message_history("Floor %d full-light %s", df->get_floor(), df->get_full_light() ? "enabled" : "disabled");
    frame_dirty = true;
    return true;
}

inline bool gamestate::handle_restart(inputstate& is) {
    if (is_action_pressed(is, INPUT_ACTION_RESTART)) {
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
        open_confirm_prompt(CONFIRM_ACTION_QUIT, "Do You Want To Exit? Press Y or N");
        return;
    }
    handle_camera_zoom(is);
    if (handle_cycle_messages(is)) {
        return;
    }
    if (is_action_pressed(is, INPUT_ACTION_HELP)) {
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
    else if (is_action_pressed(is, INPUT_ACTION_OPTIONS)) {
        display_option_menu = true;
        controlmode = CONTROLMODE_OPTION_MENU;
        return;
    }
    else if (is_action_pressed(is, INPUT_ACTION_PULL)) {
        try_entity_pull(hero_id);
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return;
    }
    if (handle_toggle_full_light(is) || handle_change_dir(is) || handle_change_dir_intent(is) || handle_display_inventory(is)) {
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
    else if (
        handle_attack(is, is_dead) || handle_pickup_item(is, is_dead) || handle_traverse_stairs(is, is_dead) || handle_open_door(is, is_dead) ||
        handle_interact(is, is_dead)) {
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
    if (inputstate_is_pressed(is, KEY_GRAVE) || inputstate_is_pressed(is, KEY_ESCAPE)) {
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
        if (options_menu.get_option(options_menu.get_selection()) == OPTION_CONTROLS) {
            open_controls_menu();
        }
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
    if (controlmode == CONTROLMODE_KEYBOARD_PROFILE) {
        handle_input_keyboard_profile_prompt(is);
        return;
    }
    if (controlmode == CONTROLMODE_LEVEL_UP) {
        handle_input_level_up(is);
        return;
    }
    if (controlmode == CONTROLMODE_CONFIRM_PROMPT) {
        handle_input_confirm_prompt(is);
        return;
    }
    if (controlmode == CONTROLMODE_INTERACTION) {
        handle_input_interaction(is);
        return;
    }
    if (controlmode == CONTROLMODE_CONTROLS_MENU) {
        handle_input_controls_menu(is);
        return;
    }
    if (is_action_pressed(is, INPUT_ACTION_CAMERA_TOGGLE)) {
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
    else if (controlmode == CONTROLMODE_CHEST) {
        handle_input_chest(is);
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
    else if (controlmode == CONTROLMODE_LEVEL_UP) {
        handle_input_level_up(is);
    }
    else if (controlmode == CONTROLMODE_INTERACTION) {
        handle_input_interaction(is);
    }
}

inline void gamestate::handle_input(inputstate& is) {
    minfo2("handle input: current_scene: %d", current_scene);
    if (inputstate_is_pressed(is, KEY_P)) {
        debugpanelon = !debugpanelon;
        minfo2("Toggling debug panel: %s", debugpanelon ? "ON" : "OFF");
    }
    if (display_confirm_prompt || controlmode == CONTROLMODE_CONFIRM_PROMPT) {
        handle_input_confirm_prompt(is);
        return;
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
