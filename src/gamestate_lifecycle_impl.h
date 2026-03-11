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
            if (path_blocked(hero_loc, loc)) {
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

inline void gamestate::update_spells_state() {
    minfo2("update_spells_state");
    for (entityid id = 0; id < next_entityid; id++) {
        if (id == hero_id || ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_SPELL) {
            continue;
        }
        bool is_complete = ct.get<spell_complete>(id).value_or(false);
        bool is_destroyed = ct.get<destroyed>(id).value_or(false);
        if (is_complete && is_destroyed) {
            auto df = d.get_current_floor();
            vec3 loc = ct.get<location>(id).value_or(vec3{-1, -1, -1});
            massert(!vec3_invalid(loc), "location is invalid");
            if (!df->df_remove_at(id, loc)) {
                merror("failed to remove id %d at %d, %d", id, loc.x, loc.y);
            }
        }
    }
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
        }
    }
}

inline void gamestate::logic_init() {
    minfo("gamestate.logic_init");
    srand(time(NULL));
    SetRandomSeed(time(NULL));
    constexpr float parts = 1.0;
    massert(!d.is_initialized, "dungeon is already initialized");
    create_and_add_df_1(BIOME_STONE, 8, 8, 2, parts);
    create_and_add_df_1(BIOME_STONE, 16, 16, 2, parts);
    massert(assign_random_stairs(), "failed to assign dungeon stairs");
    d.current_floor = 0;
    d.is_initialized = true;

    massert(d.floors.size() > 0, "dungeon.floors.size is 0");
    place_doors();
    auto df = d.get_current_floor();
    constexpr int num_boxes = 1;
    for (int i = 0; i < num_boxes; i++) {
        create_box_at_with(df->get_random_loc());
    }
    create_weapon_at_with(ct, df->get_random_loc(), sword_init());
    create_shield_at_with(ct, df->get_random_loc(), shield_init());
    constexpr int monster_count = 1;
    for (int j = 0; j < monster_count; j++) {
        vec3 random_loc = d.get_floor(0)->get_random_loc();
        create_orc_at_with(random_loc, [this](CT& ct, const entityid id) {
            entityid wpn_id = create_weapon_with([](CT& ct, const entityid id) {
                ct.set<name>(id, "Dagger");
                ct.set<description>(id, "Stabby stabby.");
                ct.set<weapontype>(id, WEAPON_DAGGER);
                ct.set<damage>(id, vec3{1, 4, 0});
                ct.set<durability>(id, 100);
                ct.set<max_durability>(id, 100);
                ct.set<rarity>(id, RARITY_COMMON);
            });
            entityid potion_id = create_potion_with([](CT& ct, const entityid id) {
                ct.set<name>(id, "small healing potion");
                ct.set<description>(id, "a small healing potion");
                ct.set<potiontype>(id, POTION_HP_SMALL);
                ct.set<healing>(id, vec3{1, 6, 0});
            });
            add_to_inventory(id, wpn_id);
            add_to_inventory(id, potion_id);
            ct.set<equipped_weapon>(id, wpn_id);
        });
    }
    msuccess("end creating monsters...");
    add_message("Welcome to the game! Press enter to cycle messages.");
    add_message("For help, press ?");
    msuccess("liblogic_init: Game state initialized");
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
    update_spells_state();
    handle_input(is);
    handle_npcs();
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
