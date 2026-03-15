#pragma once

#include "../gamestate.h"
#include <cxxtest/TestSuite.h>

class GamestateLifecycleTestSuite : public CxxTest::TestSuite {
public:
    void press_key(inputstate& is, int key) {
        const int idx = key / BITS_PER_LONG;
        const int bit = key % BITS_PER_LONG;
        is.pressed[idx] |= (1ULL << bit);
    }

    void hold_key(inputstate& is, int key) {
        const int idx = key / BITS_PER_LONG;
        const int bit = key % BITS_PER_LONG;
        is.held[idx] |= (1ULL << bit);
    }

    void add_floor(gamestate& g, int width = 8, int height = 8) {
        auto df = g.d.create_floor(BIOME_STONE, width, height);
        df->df_set_all_tiles(TILE_FLOOR_STONE_00);
        g.d.add_floor(df);
        g.d.is_initialized = true;
    }

    void testGamestateEmpty() {
    }

    void testGamestateDefaults() {
        gamestate g;

        TS_ASSERT_EQUALS(g.version, std::string(GAME_VERSION));
        TS_ASSERT_EQUALS(g.hero_id, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.entity_turn, 1);
        TS_ASSERT_EQUALS(g.next_entityid, 1);
        TS_ASSERT_EQUALS(g.new_entityid_begin, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.new_entityid_end, ENTITYID_INVALID);
        TS_ASSERT(g.cam_lockon);
        TS_ASSERT(!g.cam_changed);
        TS_ASSERT(!g.dirty_entities);
        TS_ASSERT(!g.msg_system_is_active);
        TS_ASSERT_EQUALS(g.current_scene, SCENE_TITLE);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_PLAYER);
        TS_ASSERT_EQUALS(g.confirm_action, CONFIRM_ACTION_NONE);
        TS_ASSERT_EQUALS(g.music_volume, DEFAULT_MUSIC_VOLUME);
        TS_ASSERT_DELTA(g.sfx_volume, DEFAULT_MASTER_VOLUME, 0.001f);
        TS_ASSERT_EQUALS(g.window_box_bgcolor.r, 0);
        TS_ASSERT_EQUALS(g.window_box_bgcolor.g, 0);
        TS_ASSERT_EQUALS(g.window_box_bgcolor.b, 255);
        TS_ASSERT_EQUALS(g.window_box_bgcolor.a, 128);
        TS_ASSERT_EQUALS(g.get_debug_panel_bgcolor().a, 255);
        TS_ASSERT_EQUALS(g.chara_creation.race, RACE_HUMAN);
        TS_ASSERT_EQUALS(g.chara_creation.alignment, ALIGNMENT_NEUTRAL_NEUTRAL);
        TS_ASSERT_DELTA(g.cam2d.zoom, DEFAULT_ZOOM_LEVEL, 0.001f);
        TS_ASSERT_DELTA(g.cam2d.rotation, 0.0f, 0.001f);
        TS_ASSERT_EQUALS(g.cam2d.target.x, 0.0f);
        TS_ASSERT_EQUALS(g.cam2d.target.y, 0.0f);
        TS_ASSERT_EQUALS(g.cam2d.offset.x, 0.0f);
        TS_ASSERT_EQUALS(g.cam2d.offset.y, 0.0f);
        TS_ASSERT_EQUALS(g.msg_system.size(), 0U);
        TS_ASSERT_EQUALS(g.msg_history.size(), 0U);
        TS_ASSERT_EQUALS(g.d.floors.size(), 0U);
        TS_ASSERT(!g.d.is_initialized);
        TS_ASSERT(!g.music_file_paths.empty());
    }

    void testGamestateAddEntityTracksDirtyRange() {
        gamestate g;

        const entityid first = g.add_entity();
        const entityid second = g.add_entity();
        const entityid third = g.add_entity();

        TS_ASSERT_EQUALS(first, 1);
        TS_ASSERT_EQUALS(second, 2);
        TS_ASSERT_EQUALS(third, 3);
        TS_ASSERT_EQUALS(g.next_entityid, 4);
        TS_ASSERT(g.dirty_entities);
        TS_ASSERT_EQUALS(g.new_entityid_begin, 1);
        TS_ASSERT_EQUALS(g.new_entityid_end, 4);
    }

    void testGamestateSetHeroIdAndReset() {
        gamestate g;
        const entityid hero = g.add_entity();

        TS_ASSERT(g.set_hero_id(hero));
        TS_ASSERT_EQUALS(g.hero_id, hero);

        g.reset();

        TS_ASSERT_EQUALS(g.hero_id, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.next_entityid, 1);
        TS_ASSERT_EQUALS(g.new_entityid_begin, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.new_entityid_end, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.chara_creation.alignment, ALIGNMENT_NEUTRAL_NEUTRAL);
        TS_ASSERT(!g.dirty_entities);
        TS_ASSERT_EQUALS(g.d.floors.size(), 0U);
        TS_ASSERT_EQUALS(g.msg_system.size(), 0U);
        TS_ASSERT_EQUALS(g.msg_history.size(), 0U);
    }

    void testGamestateMessageQueuesAndReset() {
        gamestate g;

        TS_ASSERT(g.add_message("hello %d", 1));
        g.add_message_history("history %s", "entry");

        TS_ASSERT(g.msg_system_is_active);
        TS_ASSERT_EQUALS(g.msg_system.size(), 1U);
        TS_ASSERT_EQUALS(g.msg_system.front(), "hello 1");
        TS_ASSERT_EQUALS(g.msg_history.size(), 1U);
        TS_ASSERT_EQUALS(g.msg_history.front(), "history entry");

        g.reset();

        TS_ASSERT(!g.msg_system_is_active);
        TS_ASSERT_EQUALS(g.msg_system.size(), 0U);
        TS_ASSERT_EQUALS(g.msg_history.size(), 0U);
    }

    void testGamestateResetRestoresCameraAndUiState() {
        gamestate g;

        g.cam_lockon = false;
        g.cam_changed = true;
        g.display_inventory_menu = true;
        g.display_action_menu = true;
        g.display_help_menu = true;
        g.display_confirm_prompt = true;
        g.do_quit = true;
        g.confirm_action = CONFIRM_ACTION_QUIT;
        g.confirm_prompt_message = "quit?";
        g.current_scene = SCENE_GAMEPLAY;
        g.music_volume = 0.25f;
        g.cam2d.target = Vector2{12.0f, 34.0f};
        g.cam2d.offset = Vector2{56.0f, 78.0f};
        g.cam2d.zoom = 9.0f;
        g.cam2d.rotation = 45.0f;

        g.reset();

        TS_ASSERT(g.cam_lockon);
        TS_ASSERT(!g.cam_changed);
        TS_ASSERT(!g.display_inventory_menu);
        TS_ASSERT(!g.display_action_menu);
        TS_ASSERT(!g.display_help_menu);
        TS_ASSERT(!g.display_confirm_prompt);
        TS_ASSERT(!g.do_quit);
        TS_ASSERT_EQUALS(g.confirm_action, CONFIRM_ACTION_NONE);
        TS_ASSERT_EQUALS(g.confirm_prompt_message, "");
        TS_ASSERT_EQUALS(g.current_scene, SCENE_TITLE);
        TS_ASSERT_DELTA(g.music_volume, DEFAULT_MUSIC_VOLUME, 0.001f);
        TS_ASSERT_DELTA(g.cam2d.target.x, 0.0f, 0.001f);
        TS_ASSERT_DELTA(g.cam2d.target.y, 0.0f, 0.001f);
        TS_ASSERT_DELTA(g.cam2d.offset.x, 0.0f, 0.001f);
        TS_ASSERT_DELTA(g.cam2d.offset.y, 0.0f, 0.001f);
        TS_ASSERT_DELTA(g.cam2d.zoom, DEFAULT_ZOOM_LEVEL, 0.001f);
        TS_ASSERT_DELTA(g.cam2d.rotation, 0.0f, 0.001f);
    }

    void testGamestateMusicDefaultsAndResetBehavior() {
        gamestate g;

        TS_ASSERT_DELTA(g.music_volume, DEFAULT_MUSIC_VOLUME, 0.001f);
        TS_ASSERT(!g.music_volume_changed);
        TS_ASSERT_EQUALS(g.current_music_index, 0U);
        TS_ASSERT(!g.music_file_paths.empty());

        const std::vector<std::string> initialMusicPaths = g.music_file_paths;

        g.music_volume = 0.25f;
        g.music_volume_changed = true;
        g.current_music_index = 9;
        g.music_file_paths.clear();
        g.music_file_paths.push_back("broken/path.ogg");

        g.reset();

        TS_ASSERT_DELTA(g.music_volume, DEFAULT_MUSIC_VOLUME, 0.001f);
        TS_ASSERT(!g.music_volume_changed);
        TS_ASSERT_EQUALS(g.current_music_index, 0U);
        TS_ASSERT(!g.music_file_paths.empty());
        TS_ASSERT_DIFFERS(g.music_file_paths.front(), "broken/path.ogg");
        TS_ASSERT_EQUALS(g.music_file_paths, initialMusicPaths);
    }

    void testRestartGameReinitializesGameplayWithoutLeavingTitleScene() {
        gamestate g;
        g.test = true;
        g.mt.seed(12345);
        g.logic_init();
        g.targetwidth = DEFAULT_TARGET_WIDTH;
        g.targetheight = DEFAULT_TARGET_HEIGHT;
        g.windowwidth = 1234;
        g.windowheight = 777;

        g.current_scene = SCENE_GAMEPLAY;
        g.do_restart = true;
        g.do_quit = true;
        g.gameover = true;
        g.restart_count = 7;
        g.add_message("stale");

        g.restart_game();

        TS_ASSERT(g.d.is_initialized);
        TS_ASSERT_EQUALS(g.d.get_floor_count(), 4U);
        TS_ASSERT_EQUALS(g.current_scene, SCENE_TITLE);
        TS_ASSERT_EQUALS(g.restart_count, 8U);
        TS_ASSERT(!g.do_restart);
        TS_ASSERT(!g.do_quit);
        TS_ASSERT(!g.gameover);
        TS_ASSERT(g.frame_dirty);
        TS_ASSERT(g.msg_system.size() >= 2U);
        TS_ASSERT_EQUALS(g.msg_system.front(), "Welcome to the game! Press enter to cycle messages.");
        TS_ASSERT_EQUALS(g.targetwidth, DEFAULT_TARGET_WIDTH);
        TS_ASSERT_EQUALS(g.targetheight, DEFAULT_TARGET_HEIGHT);
        TS_ASSERT_EQUALS(g.windowwidth, 1234);
        TS_ASSERT_EQUALS(g.windowheight, 777);
        TS_ASSERT_DELTA(g.cam2d.offset.x, DEFAULT_TARGET_WIDTH / 4.0f, 0.001f);
        TS_ASSERT_DELTA(g.cam2d.offset.y, DEFAULT_TARGET_HEIGHT / 4.0f, 0.001f);
    }

    void testHeroOpensLevelUpModalAtTenXpAndCanApplySelection() {
        gamestate g;
        g.test = true;
        g.current_scene = SCENE_GAMEPLAY;

        auto df = g.d.create_floor(BIOME_STONE, 8, 8);
        df->df_set_all_tiles(TILE_FLOOR_STONE_00);
        g.d.add_floor(df);
        g.d.is_initialized = true;

        const entityid hero = g.create_player_at_with(vec3{1, 1, 0}, "hero", g.player_init(10));
        const entityid slime = g.create_npc_at_with(RACE_GREEN_SLIME, vec3{2, 1, 0}, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(slime, ENTITYID_INVALID);

        g.ct.set<xp>(hero, 9);
        const int old_str = g.ct.get<strength>(hero).value_or(0);
        const vec2 old_hp = g.ct.get<hp>(hero).value_or(vec2{0, 0});

        g.update_npc_xp(hero, slime);

        TS_ASSERT_EQUALS(g.ct.get<xp>(hero).value_or(0), 10);
        TS_ASSERT(g.display_level_up_modal);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_LEVEL_UP);

        inputstate is;
        inputstate_reset(is);
        press_key(is, KEY_ENTER);
        g.handle_input(is);

        TS_ASSERT(!g.display_level_up_modal);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_PLAYER);
        TS_ASSERT_EQUALS(g.ct.get<level>(hero).value_or(0), 2);
        TS_ASSERT_EQUALS(g.ct.get<strength>(hero).value_or(0), old_str + 1);
        const vec2 new_hp = g.ct.get<hp>(hero).value_or(vec2{0, 0});
        TS_ASSERT_EQUALS(new_hp.x, old_hp.x);
        TS_ASSERT(new_hp.y > old_hp.y);
    }

    void testApplyLevelUpRewardsAddsHitDieWorthOfMaxHp() {
        gamestate g;
        auto df = g.d.create_floor(BIOME_STONE, 8, 8);
        df->df_set_all_tiles(TILE_FLOOR_STONE_00);
        g.d.add_floor(df);
        g.d.is_initialized = true;

        const entityid hero = g.create_player_at_with(vec3{1, 1, 0}, "hero", g.player_init(10));
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);

        g.ct.set<hd>(hero, vec3{1, 1, 0});
        g.ct.set<hp>(hero, vec2{7, 10});

        g.apply_level_up_rewards(hero);

        TS_ASSERT_EQUALS(g.ct.get<level>(hero).value_or(0), 2);
        TS_ASSERT(vec2_equal(g.ct.get<hp>(hero).value_or(vec2{0, 0}), vec2{7, 11}));
    }

    void testConfirmPromptResolvesYesInputEvenIfControlmodeDrifted() {
        gamestate g;
        g.test = true;
        inputstate is;
        inputstate_reset(is);

        g.current_scene = SCENE_GAMEPLAY;
        g.open_confirm_prompt(CONFIRM_ACTION_QUIT, "quit?");
        g.controlmode = CONTROLMODE_PLAYER;
        press_key(is, KEY_Y);

        g.handle_input(is);

        TS_ASSERT(!g.display_confirm_prompt);
        TS_ASSERT_EQUALS(g.confirm_action, CONFIRM_ACTION_NONE);
        TS_ASSERT(g.do_quit);
    }

    void testConfirmPromptResolvesNoInput() {
        gamestate g;
        g.test = true;
        inputstate is;
        inputstate_reset(is);

        g.current_scene = SCENE_GAMEPLAY;
        g.open_confirm_prompt(CONFIRM_ACTION_QUIT, "quit?");
        press_key(is, KEY_N);

        g.handle_input(is);

        TS_ASSERT(!g.display_confirm_prompt);
        TS_ASSERT_EQUALS(g.confirm_action, CONFIRM_ACTION_NONE);
        TS_ASSERT(!g.do_quit);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_PLAYER);
    }

    void testCharacterCreationNameHelpersAppendAndBackspace() {
        gamestate g;

        g.chara_creation.name.clear();
        TS_ASSERT(g.try_append_character_creation_char('A'));
        TS_ASSERT(g.try_append_character_creation_char('b'));
        TS_ASSERT(g.try_append_character_creation_char('3'));
        TS_ASSERT_EQUALS(g.chara_creation.name, "Ab3");
        TS_ASSERT(g.backspace_character_creation_name());
        TS_ASSERT_EQUALS(g.chara_creation.name, "Ab");
    }

    void testCharacterCreationNameHelpersRejectControlCharsAndLengthOverflow() {
        gamestate g;

        g.chara_creation.name.clear();
        TS_ASSERT(!g.try_append_character_creation_char('\n'));
        TS_ASSERT(!g.try_append_character_creation_char(' '));
        TS_ASSERT_EQUALS(g.chara_creation.name, "");
        for (int i = 0; i < 16; i++) {
            TS_ASSERT(g.try_append_character_creation_char('a'));
        }
        TS_ASSERT_EQUALS(g.chara_creation.name.size(), 16U);
        TS_ASSERT(!g.try_append_character_creation_char('b'));
        TS_ASSERT_EQUALS(g.chara_creation.name, "aaaaaaaaaaaaaaaa");
    }

    void testCharacterCreationTransitionsIntoKeyboardProfilePrompt() {
        gamestate g;
        g.test = true;
        g.sfx.resize(71);
        add_floor(g);
        g.current_scene = SCENE_CHARACTER_CREATION;

        inputstate is = {};
        inputstate_reset(is);
        press_key(is, KEY_ENTER);

        g.handle_input_character_creation_scene(is);

        TS_ASSERT_EQUALS(g.current_scene, SCENE_GAMEPLAY);
        TS_ASSERT(g.display_keyboard_profile_prompt);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_KEYBOARD_PROFILE);
        TS_ASSERT_DIFFERS(g.hero_id, ENTITYID_INVALID);
    }

    void testKeyboardProfilePromptCanSelectLaptopProfile() {
        gamestate g;
        g.test = true;
        g.current_scene = SCENE_GAMEPLAY;
        g.open_keyboard_profile_prompt();

        inputstate is = {};
        inputstate_reset(is);
        press_key(is, KEY_RIGHT);
        g.handle_input_keyboard_profile_prompt(is);
        TS_ASSERT_EQUALS(g.keyboard_profile_selection, static_cast<unsigned int>(KEYBOARD_PROFILE_LAPTOP));

        inputstate_reset(is);
        press_key(is, KEY_ENTER);
        g.handle_input_keyboard_profile_prompt(is);

        TS_ASSERT_EQUALS(g.keyboard_profile, KEYBOARD_PROFILE_LAPTOP);
        TS_ASSERT(g.keyboard_profile_confirmed);
        TS_ASSERT(!g.display_keyboard_profile_prompt);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_PLAYER);
    }

    void testCharacterCreationSkipsKeyboardPromptAfterProfileAlreadyConfirmed() {
        gamestate g;
        g.test = true;
        g.sfx.resize(71);
        add_floor(g);
        g.current_scene = SCENE_CHARACTER_CREATION;
        g.keyboard_profile = KEYBOARD_PROFILE_LAPTOP;
        g.keyboard_profile_confirmed = true;

        inputstate is = {};
        inputstate_reset(is);
        press_key(is, KEY_ENTER);

        g.handle_input_character_creation_scene(is);

        TS_ASSERT_EQUALS(g.current_scene, SCENE_GAMEPLAY);
        TS_ASSERT(!g.display_keyboard_profile_prompt);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_PLAYER);
        TS_ASSERT_EQUALS(g.keyboard_profile, KEYBOARD_PROFILE_LAPTOP);
        TS_ASSERT_DIFFERS(g.hero_id, ENTITYID_INVALID);
    }

    void testLaptopProfileUsesHjklMovementKeys() {
        gamestate g;
        g.test = true;
        add_floor(g);
        const entityid hero = g.create_player_at_with(vec3{3, 3, 0}, "hero", g.player_init(10));
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);

        g.current_scene = SCENE_GAMEPLAY;
        g.hero_id = hero;
        g.keyboard_profile = KEYBOARD_PROFILE_LAPTOP;

        inputstate is = {};
        inputstate_reset(is);
        press_key(is, KEY_H);

        TS_ASSERT(g.handle_move_left(is, false));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{2, 3, 0}));
    }

    void testControlsMenuCanRebindAttackKeyForCurrentProfile() {
        gamestate g;
        g.test = true;
        g.current_scene = SCENE_GAMEPLAY;
        g.keyboard_profile = KEYBOARD_PROFILE_LAPTOP;
        g.open_controls_menu();
        g.controls_menu_selection = INPUT_ACTION_ATTACK + 2;

        inputstate is = {};
        inputstate_reset(is);
        press_key(is, KEY_ENTER);
        g.handle_input_controls_menu(is);
        TS_ASSERT(g.controls_menu_waiting_for_key);

        inputstate_reset(is);
        press_key(is, KEY_T);
        g.handle_input_controls_menu(is);

        TS_ASSERT(!g.controls_menu_waiting_for_key);
        TS_ASSERT_EQUALS(g.get_keybinding_primary(KEYBOARD_PROFILE_LAPTOP, INPUT_ACTION_ATTACK), KEY_T);
    }

    void testOptionMenuCanOpenSoundMenuAndAdjustVolumes() {
        gamestate g;
        g.test = true;
        g.current_scene = SCENE_GAMEPLAY;
        g.display_option_menu = true;
        g.controlmode = CONTROLMODE_OPTION_MENU;

        inputstate is = {};
        inputstate_reset(is);
        press_key(is, KEY_ENTER);
        g.handle_input_option_menu(is);

        TS_ASSERT(g.display_sound_menu);
        TS_ASSERT(!g.display_option_menu);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_SOUND_MENU);

        inputstate_reset(is);
        press_key(is, KEY_RIGHT);
        g.handle_input_sound_menu(is);
        TS_ASSERT_DELTA(master_volume, DEFAULT_MASTER_VOLUME, 0.001f);

        inputstate_reset(is);
        press_key(is, KEY_DOWN);
        g.handle_input_sound_menu(is);
        inputstate_reset(is);
        press_key(is, KEY_LEFT);
        g.handle_input_sound_menu(is);
        TS_ASSERT_DELTA(g.music_volume, DEFAULT_MUSIC_VOLUME - AUDIO_VOLUME_STEP, 0.001f);

        inputstate_reset(is);
        press_key(is, KEY_DOWN);
        g.handle_input_sound_menu(is);
        inputstate_reset(is);
        press_key(is, KEY_LEFT);
        g.handle_input_sound_menu(is);
        TS_ASSERT_DELTA(g.sfx_volume, DEFAULT_MASTER_VOLUME - AUDIO_VOLUME_STEP, 0.001f);

        inputstate_reset(is);
        press_key(is, KEY_ESCAPE);
        g.handle_input_sound_menu(is);
        TS_ASSERT(!g.display_sound_menu);
        TS_ASSERT(g.display_option_menu);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_OPTION_MENU);
    }

    void testOptionMenuCanOpenWindowColorMenuAndResetDefaults() {
        gamestate g;
        g.test = true;
        g.current_scene = SCENE_GAMEPLAY;
        g.display_option_menu = true;
        g.controlmode = CONTROLMODE_OPTION_MENU;
        g.options_menu.incr_selection();

        inputstate is = {};
        inputstate_reset(is);
        press_key(is, KEY_ENTER);
        g.handle_input_option_menu(is);

        TS_ASSERT(g.display_window_color_menu);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_WINDOW_COLOR_MENU);

        inputstate_reset(is);
        hold_key(is, KEY_RIGHT);
        g.handle_input_window_color_menu(is);
        TS_ASSERT_EQUALS(g.window_box_bgcolor.r, 1);

        for (int i = 0; i < 8; i++) {
            inputstate_reset(is);
            press_key(is, KEY_DOWN);
            g.handle_input_window_color_menu(is);
        }
        inputstate_reset(is);
        press_key(is, KEY_ENTER);
        g.handle_input_window_color_menu(is);

        TS_ASSERT_EQUALS(g.window_box_bgcolor.r, 0);
        TS_ASSERT_EQUALS(g.window_box_bgcolor.g, 0);
        TS_ASSERT_EQUALS(g.window_box_bgcolor.b, 255);
        TS_ASSERT_EQUALS(g.window_box_bgcolor.a, 128);
        TS_ASSERT_EQUALS(g.window_box_fgcolor.r, 255);
        TS_ASSERT_EQUALS(g.window_box_fgcolor.g, 255);
        TS_ASSERT_EQUALS(g.window_box_fgcolor.b, 255);
        TS_ASSERT_EQUALS(g.window_box_fgcolor.a, 255);
    }

    void testOptionMenuCanToggleMiniInventoryPreference() {
        gamestate g;
        g.test = true;
        g.current_scene = SCENE_GAMEPLAY;
        g.display_option_menu = true;
        g.controlmode = CONTROLMODE_OPTION_MENU;
        g.options_menu.incr_selection();
        g.options_menu.incr_selection();
        g.options_menu.incr_selection();

        inputstate is = {};
        inputstate_reset(is);
        press_key(is, KEY_ENTER);
        g.handle_input_option_menu(is);

        TS_ASSERT(g.prefer_mini_inventory_menu);
        TS_ASSERT(g.display_option_menu);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_OPTION_MENU);
    }

    void testFaceDirectionAttackUsesBoundActionInsteadOfHardCodedKey() {
        gamestate g;
        g.test = true;
        add_floor(g);
        const entityid hero = g.create_player_at_with(vec3{3, 3, 0}, "hero", g.player_init(10));
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);

        g.current_scene = SCENE_GAMEPLAY;
        g.hero_id = hero;
        g.player_changing_dir = true;
        g.keyboard_profile = KEYBOARD_PROFILE_FULL;

        inputstate is = {};
        inputstate_reset(is);
        press_key(is, KEY_APOSTROPHE);
        TS_ASSERT(g.handle_change_dir(is));
        TS_ASSERT(g.ct.get<attacking>(hero).value_or(false));
        TS_ASSERT(!g.player_changing_dir);

        g.ct.set<attacking>(hero, false);
        g.player_changing_dir = true;
        g.set_keybinding_primary(KEYBOARD_PROFILE_FULL, INPUT_ACTION_FACE_ATTACK, KEY_T);
        inputstate_reset(is);
        press_key(is, KEY_APOSTROPHE);
        TS_ASSERT(g.handle_change_dir(is));
        TS_ASSERT(!g.ct.get<attacking>(hero).value_or(false));
        TS_ASSERT(g.player_changing_dir);

        inputstate_reset(is);
        press_key(is, KEY_T);
        TS_ASSERT(g.handle_change_dir(is));
        TS_ASSERT(g.ct.get<attacking>(hero).value_or(false));
        TS_ASSERT(!g.player_changing_dir);
    }

    void testDefaultStairsBindingUsesKeyFForBothProfiles() {
        gamestate g;

        TS_ASSERT_EQUALS(g.get_keybinding_primary(KEYBOARD_PROFILE_FULL, INPUT_ACTION_STAIRS), KEY_F);
        TS_ASSERT_EQUALS(g.get_keybinding_primary(KEYBOARD_PROFILE_LAPTOP, INPUT_ACTION_STAIRS), KEY_F);
    }

    void testLaptopDirectionModeDefaultMovesOffKeyF() {
        gamestate g;

        TS_ASSERT_EQUALS(g.get_keybinding_primary(KEYBOARD_PROFILE_LAPTOP, INPUT_ACTION_DIRECTION_MODE), KEY_G);
        TS_ASSERT_DIFFERS(g.get_keybinding_primary(KEYBOARD_PROFILE_LAPTOP, INPUT_ACTION_DIRECTION_MODE), KEY_F);
    }

    void testRestartGamePreservesConfirmedKeyboardProfileChoice() {
        gamestate g;
        g.test = true;
        g.mt.seed(12345);
        g.keyboard_profile = KEYBOARD_PROFILE_LAPTOP;
        g.keyboard_profile_confirmed = true;

        g.logic_init();
        g.current_scene = SCENE_GAMEPLAY;

        g.restart_game();

        TS_ASSERT_EQUALS(g.current_scene, SCENE_TITLE);
        TS_ASSERT_EQUALS(g.keyboard_profile, KEYBOARD_PROFILE_LAPTOP);
        TS_ASSERT(g.keyboard_profile_confirmed);
    }

    void testLogicInitAddsFloorFourPressurePlateTutorialSetup() {
        gamestate g;
        g.test = true;
        g.mt.seed(12345);

        g.logic_init();

        TS_ASSERT_EQUALS(g.d.get_floor_count(), 4U);
        TS_ASSERT_EQUALS(g.floor_pressure_plates.size(), 1U);
        TS_ASSERT_EQUALS(g.floor_pressure_plates.front().loc.z, 3);
        TS_ASSERT(!g.floor_pressure_plates.front().destroyed);
        TS_ASSERT_EQUALS(g.floor_pressure_plates.front().txkey_up, TX_SWITCHES_PRESSURE_PLATE_UP_00);
        TS_ASSERT_EQUALS(g.floor_pressure_plates.front().txkey_down, TX_SWITCHES_PRESSURE_PLATE_DOWN_00);
        TS_ASSERT_DIFFERS(g.floor_pressure_plates.front().linked_door_id, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.ct.get<entitytype>(g.floor_pressure_plates.front().linked_door_id).value_or(ENTITY_NONE), ENTITY_DOOR);
    }
};
