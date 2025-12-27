#include "ComponentTraits.h"
#include "entitytype.h"
#include "gamestate.h"
#include "mprint.h"
#include "weapon.h"
#include <cxxtest/TestSuite.h>
#include <raylib.h>


class MyTestSuite : public CxxTest::TestSuite {
public:
    void testGamestateBasic() {
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
        for (int i = 0; i < 10; i++) {
            gamestate g;
        }
    }

    void testEntityManagement() {
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
        gamestate g;

        // Verify initial state
        TS_ASSERT(g.msg_system.size() == 0);
        TS_ASSERT(g.msg_history.size() == 0);
        TS_ASSERT(!g.msg_system_is_active);

        // Add messages
        g.msg_system.push_back("Test message 1");
        g.msg_system.push_back("Test message 2");
        TS_ASSERT(g.msg_system.size() == 2);
        TS_ASSERT(g.msg_system[0] == "Test message 1");
        TS_ASSERT(g.msg_system[1] == "Test message 2");

        // Verify history
        g.msg_history = g.msg_system;
        TS_ASSERT(g.msg_history.size() == 2);
        TS_ASSERT(g.msg_history[0] == "Test message 1");
        TS_ASSERT(g.msg_history[1] == "Test message 2");

        // Test reset
        g.reset();
        TS_ASSERT(g.msg_system.size() == 0);
        TS_ASSERT(g.msg_history.size() == 0);
        TS_ASSERT(!g.msg_system_is_active);
    }

    void testCameraSystem() {
        gamestate g;

        // Verify initial state
        TS_ASSERT(g.cam2d.zoom == 4.0f);
        TS_ASSERT(g.cam2d.rotation == 0.0);
        TS_ASSERT(g.cam_lockon == true);
        TS_ASSERT(g.cam_changed == false);

        // Test camera movement
        g.cam2d.target = (Vector2){100, 100};
        TS_ASSERT(g.cam2d.target.x == 100);
        TS_ASSERT(g.cam2d.target.y == 100);

        // Test zoom
        g.cam2d.zoom = 2.0f;
        TS_ASSERT(g.cam2d.zoom == 2.0f);

        // Test reset
        g.reset();
        TS_ASSERT(g.cam2d.zoom == 4.0f);
        TS_ASSERT(g.cam2d.rotation == 0.0);
        TS_ASSERT(g.cam_lockon == true);
        TS_ASSERT(g.cam_changed == false);
    }

    void testMusicSystem() {
        gamestate g;

        // Verify initial state
        TS_ASSERT(g.music_volume == DEFAULT_MUSIC_VOLUME);
        TS_ASSERT(!g.music_volume_changed);
        TS_ASSERT(g.current_music_index == 0);

        // Verify music paths initialized
        TS_ASSERT(!g.music_file_paths.empty());

        // Test volume change
        const float test_volume = 0.5f;
        g.music_volume = test_volume;
        TS_ASSERT(g.music_volume == test_volume);

        // Test reset
        g.reset();
        TS_ASSERT(g.music_volume == DEFAULT_MUSIC_VOLUME);
        TS_ASSERT(!g.music_volume_changed);
        TS_ASSERT(g.current_music_index == 0);
    }


    void testGamestateInitDungeon() {
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
    }


    void testPlaceDoors() {
        gamestate g;
        const size_t placed_doors_0 = g.place_doors();
        TS_ASSERT(placed_doors_0 == 0);
        g.init_dungeon(1);
        TS_ASSERT(g.dungeon.floors.size() == 1);
        const size_t placed_doors_1 = g.place_doors();
        TS_ASSERT(placed_doors_1 > 0);
    }


    void testPlaceProps() {
        gamestate g;
        const size_t placed_props_0 = g.place_props();
        TS_ASSERT(placed_props_0 == 0);
        g.init_dungeon(1);
        const size_t placed_props_1 = g.place_props();
        TS_ASSERT(placed_props_1 > 0);
    }


    void testPlaceDagger() {
        gamestate g;

        TS_ASSERT(g.dungeon.floors.size() == 0);

        g.init_dungeon(1);

        TS_ASSERT(g.dungeon.floors.size() == 1);

        const vec3 loc = df_get_random_loc(g.dungeon.floors[0]);

        TS_ASSERT(!vec3_equal(loc, (vec3){-1, -1, -1}));

        const entityid id = g.create_weapon_at_with(g.ct, loc, g.dagger_init());

        TS_ASSERT(id != ENTITYID_INVALID);
        TS_ASSERT(g.ct.has<entitytype>(id));
        TS_ASSERT(g.ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_ITEM);
        TS_ASSERT(g.ct.has<itemtype>(id));
        TS_ASSERT(g.ct.get<itemtype>(id).value_or(ITEM_NONE) == ITEM_WEAPON);
        TS_ASSERT(g.ct.has<weapontype>(id));
        TS_ASSERT(g.ct.get<weapontype>(id).value_or(WEAPON_NONE) == WEAPON_DAGGER);
        TS_ASSERT(g.ct.has<name>(id));
        TS_ASSERT(g.ct.get<name>(id).value_or("no-name") == "dagger");
    }
};
