#pragma once

#include "../gamestate.h"
#include <cxxtest/TestSuite.h>

class GamestateLifecycleTestSuite : public CxxTest::TestSuite {
public:
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
};
