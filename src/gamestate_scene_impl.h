#pragma once

inline void gamestate::handle_input_title_scene(inputstate& is) {
    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        do_quit = true;
        return;
    }
    if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
        current_scene = SCENE_MAIN_MENU;
        frame_dirty = true;
        PlaySound(sfx[SFX_CONFIRM_01]);
    }
}

inline void gamestate::handle_input_main_menu_scene(inputstate& is) {
    if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
        if (title_screen_selection == 0) {
            current_scene = SCENE_CHARACTER_CREATION;
            frame_dirty = true;
        }
        PlaySound(sfx.at(SFX_CONFIRM_01));
    }
    else if (inputstate_is_pressed(is, KEY_DOWN)) {
        title_screen_selection++;
        if (title_screen_selection >= max_title_screen_selections) {
            title_screen_selection = 0;
        }
        PlaySound(sfx.at(SFX_CONFIRM_01));
    }
    else if (inputstate_is_pressed(is, KEY_UP)) {
        title_screen_selection--;
        if (title_screen_selection < 0) {
            title_screen_selection = max_title_screen_selections - 1;
        }
        PlaySound(sfx.at(SFX_CONFIRM_01));
    }
    else if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        do_quit = true;
        PlaySound(sfx.at(SFX_CONFIRM_01));
    }
    frame_dirty = true;
}

inline void gamestate::make_all_npcs_target_player() {
    massert(hero_id != ENTITYID_INVALID, "hero_id is invalid");
    for (entityid id = 0; id < next_entityid; id++) {
        entitytype_t t = ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (t != ENTITY_NPC) {
            continue;
        }
        ct.set<target_id>(id, hero_id);
    }
}

inline void gamestate::handle_input_character_creation_scene(inputstate& is) {
    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        do_quit = true;
        return;
    }
    if (inputstate_is_pressed(is, KEY_ENTER)) {
        PlaySound(sfx.at(SFX_CONFIRM_01));
        int myhd = chara_creation.hitdie;
        int maxhp_roll = -1;
        while (maxhp_roll < 1) {
            maxhp_roll = do_roll_best_of_3(vec3{1, myhd, 0}) + get_stat_bonus(chara_creation.constitution);
        }
        shared_ptr<dungeon_floor> df = d.floors[0];
        vec3 start_loc = df->get_random_loc();
        massert(!vec3_invalid(start_loc), "start_loc is (-1,-1,-1) - no valid start location exists");
        entity_turn = create_player_at_with(start_loc, "darkmage", player_init(maxhp_roll));
        massert(hero_id != ENTITYID_INVALID, "heroid is invalid");
        make_all_npcs_target_player();
        current_scene = SCENE_GAMEPLAY;
    }
    else if (inputstate_is_pressed(is, KEY_SPACE)) {
        PlaySound(sfx.at(SFX_CONFIRM_01));
        chara_creation.strength = do_roll_best_of_3((vec3){3, 6, 0});
        chara_creation.dexterity = do_roll_best_of_3((vec3){3, 6, 0});
        chara_creation.intelligence = do_roll_best_of_3((vec3){3, 6, 0});
        chara_creation.wisdom = do_roll_best_of_3((vec3){3, 6, 0});
        chara_creation.constitution = do_roll_best_of_3((vec3){3, 6, 0});
        chara_creation.charisma = do_roll_best_of_3((vec3){3, 6, 0});
    }
    else if (inputstate_is_pressed(is, KEY_LEFT)) {
        PlaySound(sfx.at(SFX_CONFIRM_01));
        int race = chara_creation.race;
        if (chara_creation.race > 1) {
            race--;
        }
        else {
            race = RACE_COUNT - 1;
        }
        chara_creation.race = (race_t)race;
        chara_creation.hitdie = get_racial_hd(chara_creation.race);
    }
    else if (inputstate_is_pressed(is, KEY_RIGHT)) {
        PlaySound(sfx.at(SFX_CONFIRM_01));
        int race = chara_creation.race;
        if (race < RACE_COUNT - 1) {
            race++;
        }
        else {
            race = RACE_NONE + 1;
        }
        chara_creation.race = (race_t)race;
        chara_creation.hitdie = get_racial_hd(chara_creation.race);
    }
    else if (inputstate_is_pressed(is, KEY_UP)) {
        PlaySound(sfx.at(SFX_CONFIRM_01));
        chara_creation.alignment = alignment_prev(chara_creation.alignment);
    }
    else if (inputstate_is_pressed(is, KEY_DOWN)) {
        PlaySound(sfx.at(SFX_CONFIRM_01));
        chara_creation.alignment = alignment_next(chara_creation.alignment);
    }
    frame_dirty = true;
}
