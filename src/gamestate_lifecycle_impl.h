/** @file gamestate_lifecycle_impl.h
 *  @brief Core gameplay lifecycle, visibility, and tick helpers implemented on `gamestate`.
 */

#pragma once

inline void gamestate::update_tile(tile_t& tile) {
    tile.set_explored(true);
    tile.set_visible(true);
}

inline bool gamestate::path_blocked(vec3 a, vec3 b) {
    vector<vec3> path = calculate_path_with_thickness(a, b);
    auto df = d.get_current_floor();
    for (auto loc : path) {
        tile_t& t = df->tile_at(loc);
        if (tiletype_is_none(t.get_type())) {
            return true;
        }
        else if (tiletype_is_wall(t.get_type())) {
            return true;
        }
        entityid door_id = t.get_cached_door();
        if (door_id != INVALID) {
            bool door_is_open = ct.get<door_open>(door_id).value_or(false);
            if (!door_is_open) {
                return true;
            }
        }
    }
    return false;
}

inline bool gamestate::visibility_path_blocked(vec3 a, vec3 b) {
    vector<vec3> path = calculate_path_with_thickness(a, b);
    auto df = d.get_current_floor();
    for (auto loc : path) {
        tile_t& t = df->tile_at(loc);
        if (tiletype_is_none(t.get_type())) {
            return true;
        }
        if (tiletype_is_wall(t.get_type())) {
            return true;
        }
        entityid door_id = t.get_cached_door();
        if (door_id != INVALID && !vec3_equal(loc, b)) {
            bool door_is_open = ct.get<door_open>(door_id).value_or(false);
            if (!door_is_open) {
                return true;
            }
        }
    }
    return false;
}

inline bool gamestate::update_player_tiles_explored() {
    if (current_scene != SCENE_GAMEPLAY) {
        return false;
    }
    if (hero_id == ENTITYID_INVALID) {
        merror2("hero_id is invalid");
        return false;
    }
    auto df = d.get_current_floor();
    auto maybe_loc = ct.get<location>(hero_id);
    if (!maybe_loc.has_value()) {
        merror2("hero location lacks value");
        return false;
    }
    vec3 hero_loc = maybe_loc.value();
    for (int y = 0; y < df->get_height(); y++) {
        for (int x = 0; x < df->get_width(); x++) {
            df->tile_at(vec3{x, y, hero_loc.z}).set_visible(false);
        }
    }
    int light_radius0 = ct.get<light_radius>(hero_id).value_or(1);
    int min_x = std::max(0, hero_loc.x - light_radius0);
    int max_x = std::min(df->get_width() - 1, hero_loc.x + light_radius0);
    int min_y = std::max(0, hero_loc.y - light_radius0);
    int max_y = std::min(df->get_height() - 1, hero_loc.y + light_radius0);
    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            if (abs(x - hero_loc.x) + abs(y - hero_loc.y) > light_radius0) {
                continue;
            }
            vec3 loc = {x, y, hero_loc.z};
            if (visibility_path_blocked(hero_loc, loc)) {
                continue;
            }
            tile_t& t = df->tile_at(loc);
            update_tile(t);
        }
    }
    return true;
}

inline bool gamestate::update_player_state() {
    if (hero_id == ENTITYID_INVALID) {
        merror2("hero_id is invalid");
        return false;
    }
    if (ct.get<dead>(hero_id).value_or(true)) {
        merror2("hero_id is dead");
        gameover = true;
        return true;
    }
    ct.set<blocking>(hero_id, false);
    ct.set<block_success>(hero_id, false);
    ct.set<damaged>(hero_id, false);
    return true;
}

inline void gamestate::update_npcs_state() {
    minfo2("BEGIN update_npcs_state");
    auto df = d.get_current_floor();
    (void)df;
    minfo2("begin loop");
    for (entityid id = 0; id < next_entityid; id++) {
        auto type = ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (type == ENTITY_NPC) {
            ct.set<damaged>(id, false);
            update_npc_behavior(id);
        }
    }
}

inline void gamestate::update_npc_behavior(entityid id) {
    if (ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_NPC) {
        return;
    }
    if (ct.get<dead>(id).value_or(true)) {
        ct.set<entity_default_action>(id, ENTITY_DEFAULT_ACTION_NONE);
        ct.set<target_id>(id, ENTITYID_INVALID);
        return;
    }

    const bool is_aggressive = ct.get<aggro>(id).value_or(false);
    if (hero_id == ENTITYID_INVALID || !is_aggressive || !ct.has<location>(hero_id)) {
        ct.set<entity_default_action>(id, ENTITY_DEFAULT_ACTION_RANDOM_MOVE);
        ct.set<target_id>(id, ENTITYID_INVALID);
        return;
    }

    const vec3 npc_loc = ct.get<location>(id).value_or(vec3{-1, -1, -1});
    const vec3 hero_loc = ct.get<location>(hero_id).value_or(vec3{-1, -1, -1});
    if (vec3_invalid(npc_loc) || vec3_invalid(hero_loc) || npc_loc.z != hero_loc.z) {
        ct.set<entity_default_action>(id, ENTITY_DEFAULT_ACTION_RANDOM_MOVE);
        ct.set<target_id>(id, ENTITYID_INVALID);
        return;
    }

    ct.set<target_id>(id, hero_id);
    if (is_entity_adjacent(id, hero_id)) {
        ct.set<entity_default_action>(id, ENTITY_DEFAULT_ACTION_ATTACK_TARGET_IF_ADJACENT);
        return;
    }

    ct.set<entity_default_action>(id, ENTITY_DEFAULT_ACTION_MOVE_TO_TARGET_AND_ATTACK_TARGET_IF_ADJACENT);
}

inline void gamestate::logic_init() {
    minfo("gamestate.logic_init");
    srand(time(NULL));
    SetRandomSeed(time(NULL));
    constexpr float parts = 1.0;
    massert(!d.is_initialized, "dungeon is already initialized");
    create_and_add_df_1(BIOME_STONE, 8, 8, 4, parts);
    create_and_add_df_1(BIOME_STONE, 24, 24, 4, parts);
    create_and_add_df_1(BIOME_STONE, 16, 16, 4, parts);
    const bool stairs_assigned = assign_random_stairs();
    massert(stairs_assigned, "failed to assign dungeon stairs");
    if (!stairs_assigned) {
        merror("failed to assign dungeon stairs");
        return;
    }
    d.current_floor = 0;
    d.is_initialized = true;

    massert(d.floors.size() > 0, "dungeon.floors.size is 0");
    setup_floor_four_pressure_plate_tutorial();
    place_doors();
    place_props();
    place_first_floor_chest();
    auto df = d.get_current_floor();
    constexpr int num_boxes = 1;
    for (int i = 0; i < num_boxes; i++) {
        create_box_at_with(df->get_random_loc());
    }
    create_weapon_at_with(ct, df->get_random_loc(), sword_init());
    create_shield_at_with(ct, df->get_random_loc(), shield_init());
    auto green_slime_init = [](CT& ct, const entityid id) {
        ct.set<name>(id, "green slime");
        ct.set<dialogue_text>(id, "The slime jiggles quietly.");
        ct.set<aggro>(id, false);
        ct.set<level>(id, 1);
        ct.set<xp>(id, 0);
    };
    auto armed_orc_init = [this](CT& ct, const entityid id) {
        vector<with_fun> weapon_inits = {
            dagger_init(),
            sword_init(),
            axe_init(),
        };
        uniform_int_distribution<int> weapon_dist(0, static_cast<int>(weapon_inits.size()) - 1);
        const entityid weapon_id = create_weapon_with(weapon_inits[weapon_dist(mt)]);
        const entityid potion_id = create_potion_with(potion_init(POTION_HP_SMALL));
        add_to_inventory(id, weapon_id);
        add_to_inventory(id, potion_id);
        ct.set<equipped_weapon>(id, weapon_id);
        ct.set<aggro>(id, true);
    };

    const race_t friendly_race = static_cast<race_t>(GetRandomValue(RACE_NONE + 1, RACE_COUNT - 1));
    const vec3 friendly_loc = d.get_floor(0)->get_random_loc();
    create_npc_at_with(friendly_race, friendly_loc, [](CT& ct, const entityid id) {
        ct.set<aggro>(id, false);
    });

    auto floor_one = d.get_floor(1);
    for (int i = 0; i < 9; i++) {
        const vec3 slime_loc = floor_one->get_random_loc();
        create_npc_at_with(RACE_GREEN_SLIME, slime_loc, green_slime_init);
    }

    const vec3 floor_two_orc_loc = vec3_valid(floor_four_tutorial_orc_spawn) ? floor_four_tutorial_orc_spawn : d.get_floor(2)->get_random_loc();
    create_orc_at_with(floor_two_orc_loc, armed_orc_init);
    msuccess("end creating monsters...");
    add_message("Welcome to the game! Press enter to cycle messages.");
    add_message("For help, press ?");
    msuccess("liblogic_init: Game state initialized");
}

inline void gamestate::restart_game() {
    const unsigned int previous_restart_count = restart_count;
    const int previous_target_width = targetwidth;
    const int previous_target_height = targetheight;
    const int previous_window_width = windowwidth;
    const int previous_window_height = windowheight;
    logic_close();
    reset();
    targetwidth = previous_target_width > 0 ? previous_target_width : DEFAULT_TARGET_WIDTH;
    targetheight = previous_target_height > 0 ? previous_target_height : DEFAULT_TARGET_HEIGHT;
    windowwidth = previous_window_width > 0 ? previous_window_width : DEFAULT_WIN_WIDTH;
    windowheight = previous_window_height > 0 ? previous_window_height : DEFAULT_WIN_HEIGHT;
    cam2d.offset = Vector2{targetwidth / 4.0f, targetheight / 4.0f};
    logic_init();
    do_restart = false;
    restart_count = previous_restart_count + 1;
    current_scene = SCENE_TITLE;
    frame_dirty = true;
}

inline void gamestate::handle_test_flag() {
    minfo2(
        "handle test flag: %s",
        flag == GAMESTATE_FLAG_PLAYER_ANIM    ? "player anim"
        : flag == GAMESTATE_FLAG_PLAYER_INPUT ? "player input"
        : flag == GAMESTATE_FLAG_NPC_TURN     ? "npc turn"
        : flag == GAMESTATE_FLAG_NPC_ANIM     ? "npc anim"
                                              : "Unknown");
    if (flag == GAMESTATE_FLAG_PLAYER_ANIM) {
#ifndef NPCS_ALL_AT_ONCE
        entity_turn++;
        if (entity_turn >= next_entityid) {
            entity_turn = 1;
        }
#endif
        flag = GAMESTATE_FLAG_NPC_TURN;
    }
    else if (flag == GAMESTATE_FLAG_NPC_ANIM) {
#ifndef NPCS_ALL_AT_ONCE
        entity_turn++;
        if (entity_turn >= next_entityid) {
            entity_turn = 1;
        }
        if (entity_turn == hero_id) {
            flag = GAMESTATE_FLAG_PLAYER_INPUT;
            turn_count++;
        }
        else {
            flag = GAMESTATE_FLAG_NPC_TURN;
        }
#else
        flag = GAMESTATE_FLAG_PLAYER_INPUT;
        turn_count++;
#endif
    }
}

inline void gamestate::tick(inputstate& is) {
    minfo3("tick");

    if (!update_player_tiles_explored()) {
        merror3("update player tiles explored failed");
    }

    if (!update_player_state()) {
        merror3("update player state failed");
    }

    update_npcs_state();
    handle_input(is);
    handle_npcs();
    update_damage_popups(test ? (1.0f / DEFAULT_TARGET_FPS) : std::max(GetFrameTime(), 1.0f / 240.0f));
#ifdef DEBUG
    update_debug_panel_buffer(is);
#endif
    currenttime = time(NULL);
    currenttimetm = localtime(&currenttime);
    strftime(currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", currenttimetm);
    if (test) {
        handle_test_flag();
    }
    ticks++;
    minfo2("end tick");
}
