#include "gamestate.h"
#include "mprint.h"
#include <cxxtest/TestSuite.h>
#include <raylib.h>


class MyTestSuite : public CxxTest::TestSuite {
public:
    void testGamestateBasic() {
        //minfo("--- start test gamestate basic ---");
        gamestate g;

        TS_ASSERT(g.cam_lockon == true);
        TS_ASSERT(g.msg_system.size() == 0);
        TS_ASSERT(g.msg_history.size() == 0);

        g.set_hero_id(1);
        TS_ASSERT(g.hero_id == 1);

        g.reset();

        TS_ASSERT(g.hero_id == ENTITYID_INVALID);
        TS_ASSERT(g.cam_lockon == true);
        TS_ASSERT(g.msg_system.size() == 0);
        TS_ASSERT(g.msg_history.size() == 0);

        g.reset();
    }

    void testGamestateThrowaway() {
        //minfo("--- start test gamestate throwaway ---");
        for (int i = 0; i < 10; i++) {
            gamestate g;
        }
    }

    void testEntityManagement() {
        //minfo("--- start test entity management ---");
        gamestate g;

        // Verify initial state
        TS_ASSERT(g.next_entityid == 1);
        TS_ASSERT(g.new_entityid_begin == ENTITYID_INVALID);
        TS_ASSERT(g.new_entityid_end == ENTITYID_INVALID);
        TS_ASSERT(!g.dirty_entities);

        // Add first entity
        entityid first = g.add_entity();
        TS_ASSERT(first == 1);
        TS_ASSERT(g.next_entityid == 2);
        TS_ASSERT(g.new_entityid_begin == 1);
        TS_ASSERT(g.new_entityid_end == 2);
        TS_ASSERT(g.dirty_entities);

        // Add second entity
        entityid second = g.add_entity();
        TS_ASSERT(second == 2);
        TS_ASSERT(g.next_entityid == 3);
        TS_ASSERT(g.new_entityid_begin == 1); // Should stay at first ID
        TS_ASSERT(g.new_entityid_end == 3); // Should update to new end
        TS_ASSERT(g.dirty_entities);

        // Reset and verify clean state
        g.reset();
        TS_ASSERT(g.next_entityid == 1);
        TS_ASSERT(g.new_entityid_begin == ENTITYID_INVALID);
        TS_ASSERT(g.new_entityid_end == ENTITYID_INVALID);
        TS_ASSERT(!g.dirty_entities);
    }

    void testMessageSystem() {
        //minfo("--- start test message system ---");
        gamestate g;

        // Verify initial state
        //minfo("Checking initial state");
        TS_ASSERT(g.msg_system.size() == 0);
        TS_ASSERT(g.msg_history.size() == 0);
        TS_ASSERT(!g.msg_system_is_active);

        // Add messages
        //minfo("Adding test messages");
        g.msg_system.push_back("Test message 1");
        g.msg_system.push_back("Test message 2");
        TS_ASSERT(g.msg_system.size() == 2);
        //minfo("Message system size: %zu", g.msg_system.size());
        TS_ASSERT(g.msg_system[0] == "Test message 1");
        TS_ASSERT(g.msg_system[1] == "Test message 2");

        // Verify history
        //minfo("Checking message history");
        g.msg_history = g.msg_system;
        TS_ASSERT(g.msg_history.size() == 2);
        //minfo("History size: %zu", g.msg_history.size());
        TS_ASSERT(g.msg_history[0] == "Test message 1");
        TS_ASSERT(g.msg_history[1] == "Test message 2");

        // Test reset
        //minfo("Testing reset behavior");
        g.reset();
        TS_ASSERT(g.msg_system.size() == 0);
        TS_ASSERT(g.msg_history.size() == 0);
        TS_ASSERT(!g.msg_system_is_active);
        //minfo("Reset complete");
    }

    void testCameraSystem() {
        //minfo("--- start test camera system ---");
        gamestate g;

        // Verify initial state
        //minfo("Checking initial camera state");
        TS_ASSERT(g.cam2d.zoom == 4.0f);
        TS_ASSERT(g.cam2d.rotation == 0.0);
        TS_ASSERT(g.cam_lockon == true);
        TS_ASSERT(g.cam_changed == false);

        // Test camera movement
        //minfo("Testing camera movement");
        g.cam2d.target = (Vector2){100, 100};
        TS_ASSERT(g.cam2d.target.x == 100);
        TS_ASSERT(g.cam2d.target.y == 100);
        //minfo("Camera target set to (%.1f, %.1f)", g.cam2d.target.x, g.cam2d.target.y);

        // Test zoom
        //minfo("Testing camera zoom");
        g.cam2d.zoom = 2.0f;
        TS_ASSERT(g.cam2d.zoom == 2.0f);
        //minfo("Camera zoom set to %.1f", g.cam2d.zoom);

        // Test reset
        //minfo("Testing camera reset");
        g.reset();
        TS_ASSERT(g.cam2d.zoom == 4.0f);
        TS_ASSERT(g.cam2d.rotation == 0.0);
        TS_ASSERT(g.cam_lockon == true);
        TS_ASSERT(g.cam_changed == false);
        //minfo("Reset complete - zoom: %.1f, changed: %d", g.cam2d.zoom, g.cam_changed);
    }

    void testMusicSystem() {
        //minfo("--- Starting test music system ---");
        gamestate g;

        // Verify initial state
        //minfo("Checking initial music state...");
        //minfo("Expected music_volume: %.1f, Actual: %.1f", DEFAULT_MUSIC_VOLUME, g.music_volume);
        TS_ASSERT(g.music_volume == DEFAULT_MUSIC_VOLUME);
        //minfo("Expected music_volume_changed: 0, Actual: %d", g.music_volume_changed);
        TS_ASSERT(!g.music_volume_changed);
        //minfo("Expected current_music_index: 0, Actual: %d", g.current_music_index);
        TS_ASSERT(g.current_music_index == 0);

        // Verify music paths initialized
        //minfo("Checking music paths...");
        //minfo("Music paths count: %zu", g.music_file_paths.size());
        //if (!g.music_file_paths.empty()) {
        //    minfo("First music path: %s", g.music_file_paths[0].c_str());
        //}
        TS_ASSERT(!g.music_file_paths.empty());

        // Test volume change
        //minfo("Testing volume change...");
        const float test_volume = 0.5f;
        g.music_volume = test_volume;
        //minfo("Set volume to %.1f, checking...", test_volume);
        //minfo("Expected music_volume: %.1f, Actual: %.1f", test_volume, g.music_volume);
        TS_ASSERT(g.music_volume == test_volume);
        //minfo("Note: music_volume_changed is not automatically set by direct TS_ASSERTsignment");

        // Test reset
        //minfo("Testing reset...");
        g.reset();
        //minfo("After reset - Expected music_volume: %.1f, Actual: %.1f", DEFAULT_MUSIC_VOLUME, g.music_volume);
        TS_ASSERT(g.music_volume == DEFAULT_MUSIC_VOLUME);
        //minfo("After reset - Expected music_volume_changed: 0, Actual: %d", g.music_volume_changed);
        TS_ASSERT(!g.music_volume_changed);
        //minfo("After reset - Expected current_music_index: 0, Actual: %d", g.current_music_index);
        TS_ASSERT(g.current_music_index == 0);
        //minfo("--- Reset complete ---");
    }


    void testGamestateInitDungeon() {
        //minfo("--- start test init dungeon ---");
        gamestate g;

        g.init_dungeon(1);
        TS_ASSERT(g.dungeon.floors.size() == 1);
        g.init_dungeon(2);
        TS_ASSERT(g.dungeon.floors.size() == 1);
        g.reset();
        TS_ASSERT(g.dungeon.floors.size() == 0);

        g.init_dungeon(2);
        TS_ASSERT(g.dungeon.floors.size() == 2);

        g.reset();

        //minfo("--- end test init dungeon ---");
    }


    void testPlaceDoors() {
        //minfo("--- start test place doors ---");
        gamestate g;
        const size_t placed_doors_0 = g.place_doors();

        TS_ASSERT(placed_doors_0 == 0);

        g.init_dungeon(1);

        TS_ASSERT(g.dungeon.floors.size() == 1);

        const size_t placed_doors_1 = g.place_doors();

        TS_ASSERT(placed_doors_1 > 0);
    }
};
