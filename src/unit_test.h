#include "ComponentTraits.h"
#include "entityid.h"
#include "entitytype.h"
#include "gamestate.h"
#include "mprint.h"
#include "potion.h"
#include "scene.h"
#include "tactics.h"
#include "weapon.h"
#include <chrono>
#include <cxxtest/TestSuite.h>
#include <raylib.h>
#include <set>




using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::system_clock;
using std::chrono::time_point;




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

        g.init_dungeon(BIOME_STONE, 1, 32, 32);
        TS_ASSERT(g.d.floors.size() == 1);
        g.init_dungeon(BIOME_STONE, 2, 32, 32);
        TS_ASSERT(g.d.floors.size() == 1);
        g.reset();
        TS_ASSERT(g.d.floors.size() == 0);

        g.init_dungeon(BIOME_STONE, 2, 32, 32);
        TS_ASSERT(g.d.floors.size() == 2);

        g.reset();
    }


    void testPlaceDoors() {
        gamestate g;
        const size_t placed_doors_0 = g.place_doors();
        TS_ASSERT(placed_doors_0 == 0);
        g.init_dungeon(BIOME_STONE, 1, 32, 32);
        TS_ASSERT(g.d.floors.size() == 1);
        const size_t placed_doors_1 = g.place_doors();
        TS_ASSERT(placed_doors_1 > 0);
    }


    void testPlaceProps() {
        gamestate g;
        const size_t placed_props_0 = g.place_props();
        TS_ASSERT(placed_props_0 == 0);
        g.init_dungeon(BIOME_STONE, 1, 32, 32);
        const size_t placed_props_1 = g.place_props();
        TS_ASSERT(placed_props_1 > 0);
    }


    void testPlaceDagger() {
        gamestate g;

        TS_ASSERT(g.d.floors.size() == 0);

        //g.init_dungeon(1);
        g.init_dungeon(BIOME_STONE, 1, 32, 32);

        TS_ASSERT(g.d.floors.size() == 1);

        const vec3 loc = g.d.floors[0].df_get_random_loc();

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


    void testInitShield() {
        gamestate g;
        //g.init_dungeon(1);
        g.init_dungeon(BIOME_STONE, 1, 32, 32);
        TS_ASSERT(g.d.floors.size() > 0);
        const vec3 loc = g.d.floors[0].df_get_random_loc();
        TS_ASSERT(!vec3_equal(loc, (vec3){-1, -1, -1}));
        const entityid id = g.create_shield_at_with(loc, g.shield_init());
        TS_ASSERT(id != ENTITYID_INVALID);

        TS_ASSERT(g.ct.has<entitytype>(id));
        TS_ASSERT(g.ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_ITEM);
        TS_ASSERT(g.ct.has<itemtype>(id));
        TS_ASSERT(g.ct.get<itemtype>(id).value_or(ITEM_NONE) == ITEM_SHIELD);
        TS_ASSERT(g.ct.has<shieldtype>(id));
        TS_ASSERT(g.ct.get<shieldtype>(id).value_or(SHIELD_NONE) == SHIELD_KITE);
        TS_ASSERT(g.ct.has<name>(id));
        TS_ASSERT(g.ct.get<name>(id).value_or("no-name") == "kite shield");
    }


    void testInitPotion() {
        gamestate g;
        //g.init_dungeon(1);

        g.init_dungeon(BIOME_STONE, 1, 32, 32);
        TS_ASSERT(g.d.floors.size() > 0);
        const vec3 loc = g.d.floors[0].df_get_random_loc();
        TS_ASSERT(!vec3_equal(loc, (vec3){-1, -1, -1}));
        const entityid id = g.create_potion_at_with(loc, g.potion_init(POTION_HP_SMALL));
        TS_ASSERT(id != ENTITYID_INVALID);

        TS_ASSERT(g.ct.has<entitytype>(id));
        TS_ASSERT(g.ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_ITEM);
        TS_ASSERT(g.ct.has<itemtype>(id));
        TS_ASSERT(g.ct.get<itemtype>(id).value_or(ITEM_NONE) == ITEM_POTION);
        TS_ASSERT(g.ct.has<potiontype>(id));
        TS_ASSERT(g.ct.get<potiontype>(id).value_or(POTION_NONE) == POTION_HP_SMALL);
        TS_ASSERT(g.ct.has<name>(id));
        TS_ASSERT(g.ct.get<name>(id).value_or("no-name") == "small healing potion");
    }



    void testMonsterInitSingle() {
        gamestate g;
        //g.init_dungeon(1);
        g.init_dungeon(BIOME_STONE, 1, 32, 32);
        TS_ASSERT(g.d.floors.size() > 0);
        const vec3 loc = g.d.floors[0].df_get_random_loc();
        TS_ASSERT(!vec3_equal(loc, (vec3){-1, -1, -1}));

        const entityid id = g.create_random_monster_at_with(loc, [](CT& ct, const entityid id) {});
        TS_ASSERT(id != ENTITYID_INVALID);

        TS_ASSERT(g.ct.has<entitytype>(id));
        TS_ASSERT(g.ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_NPC);
        //TS_ASSERT(g.ct.has<itemtype>(id));
        //TS_ASSERT(g.ct.get<itemtype>(id).value_or(ITEM_NONE) == ITEM_POTION);
        TS_ASSERT(g.ct.has<race>(id));
        TS_ASSERT(g.ct.get<race>(id).value_or(RACE_NONE) == RACE_ORC);
        TS_ASSERT(g.ct.has<name>(id));
        TS_ASSERT(g.ct.has<inventory>(id));
        TS_ASSERT(g.ct.get<inventory>(id).value()->size() > 0);
        TS_ASSERT(g.ct.get<inventory>(id).value()->size() == 2);

        auto df = g.d.floors[0];
        auto t = df.df_tile_at(loc);

        TS_ASSERT(t.entities->size() > 0);
        const entityid id2 = t.entities->at(0);
        TS_ASSERT_EQUALS(id, id2);
    }



    void testMonsterMulti() {
        gamestate g;
        //g.init_dungeon(1);
        g.init_dungeon(BIOME_STONE, 1, 32, 32);
        constexpr int monster_count = 4;
        std::set<entityid> monster_set;
        for (int i = 0; i < monster_count; i++) {
            TS_ASSERT(g.d.floors.size() > 0);
            //    const vec3 loc = df_get_random_loc(g.d.floors[0]);
            const vec3 loc = g.d.floors[0].df_get_random_loc();
            TS_ASSERT(!vec3_equal(loc, (vec3){-1, -1, -1}));
            const entityid id = g.create_random_monster_at_with(loc, [](CT& ct, const entityid id) {});
            TS_ASSERT(id != ENTITYID_INVALID);
            monster_set.emplace(id);
        }
        TS_ASSERT_EQUALS(monster_set.size(), monster_count);
    }


    void testMonsterMax() {
        gamestate g;
        //g.init_dungeon(1);
        g.init_dungeon(BIOME_STONE, 1, 32, 32);
        constexpr int monster_count = 31;
        std::set<entityid> monster_set;
        for (int i = 0; i < monster_count; i++) {
            TS_ASSERT(g.d.floors.size() > 0);
            //    const vec3 loc = df_get_random_loc(g.d.floors[0]);
            const vec3 loc = g.d.floors[0].df_get_random_loc();
            TS_ASSERT(!vec3_equal(loc, (vec3){-1, -1, -1}));
            const entityid id = g.create_random_monster_at_with(loc, [](CT& ct, const entityid id) {});
            TS_ASSERT(id != ENTITYID_INVALID);
            monster_set.emplace(id);
        }
        TS_ASSERT_EQUALS(monster_set.size(), monster_count);
    }


    void testMonsterTooMany() {
        gamestate g;
        g.init_dungeon(BIOME_STONE, 1, 32, 32);
        TS_ASSERT(g.d.floors.size() > 0);
        constexpr int monster_count = 32;
        std::set<entityid> monster_set;
        for (int i = 0; i < monster_count; i++) {
            //    const vec3 loc = df_get_random_loc(g.d.floors[0]);
            const vec3 loc = g.d.floors[0].df_get_random_loc();
            const entityid id = g.create_random_monster_at_with(loc, [](CT& ct, const entityid id) {});
            if (id == ENTITYID_INVALID) {
                continue;
            }
            TS_ASSERT(id != ENTITYID_INVALID);
            monster_set.emplace(id);
        }
        TS_ASSERT_EQUALS(monster_set.size(), monster_count - 1);
    }



    void testLogicInit() {
        gamestate g;
        g.test = true;
        g.init_dungeon(BIOME_STONE, 1, 32, 32);
        TS_ASSERT(g.d.floors.size() > 0);
        // place doors
        const size_t placed_doors_0 = g.place_doors();
        TS_ASSERT(placed_doors_0 > 0);
        // place props
        const size_t placed_props_0 = g.place_props();
        TS_ASSERT(placed_props_0 > 0);
        // create dagger
        const entityid id = g.create_weapon_at_random_loc_with(g.ct, g.dagger_init());
        TS_ASSERT(id != ENTITYID_INVALID);
        // create shield
        const vec3 loc2 = g.d.floors[0].df_get_random_loc();
        TS_ASSERT(!vec3_equal(loc2, (vec3){-1, -1, -1}));
        const entityid id2 = g.create_shield_at_with(loc2, g.shield_init());
        TS_ASSERT(id2 != ENTITYID_INVALID);
        // create potion
        const vec3 loc3 = g.d.floors[0].df_get_random_loc();
        TS_ASSERT(!vec3_equal(loc3, (vec3){-1, -1, -1}));
        const entityid id3 = g.create_potion_at_with(loc3, g.potion_init(POTION_HP_SMALL));
        TS_ASSERT(id3 != ENTITYID_INVALID);
        // create NPCs (orcs)
        constexpr int monster_count = 4;
        std::set<entityid> monster_set;
        for (int i = 0; i < monster_count; i++) {
            TS_ASSERT(g.d.floors.size() > 0);
            const vec3 loc = g.d.floors[0].df_get_random_loc();
            TS_ASSERT(!vec3_equal(loc, (vec3){-1, -1, -1}));
            const entityid id = g.create_random_monster_at_with(loc, [](CT& ct, const entityid id) {});
            TS_ASSERT(id != ENTITYID_INVALID);
            monster_set.emplace(id);
        }
        TS_ASSERT_EQUALS(monster_set.size(), monster_count);
        const vec3 loc4 = g.d.floors[0].df_get_random_loc();
        TS_ASSERT(!vec3_equal(loc4, (vec3){-1, -1, -1}));
        const int maxhp_roll = 10;
        g.entity_turn = g.create_player_with(loc4, "darkmage", g.player_init(maxhp_roll));
        TS_ASSERT(g.hero_id != ENTITYID_INVALID);
        inputstate is;
        inputstate_update(is);
        g.current_scene = SCENE_GAMEPLAY;
        TS_ASSERT(g.test);
        //constexpr int num_ticks = 1000;
        //for (int i = 0; i < num_ticks; i++) {
        //    g.tick(is);
        //}
        //TS_ASSERT(g.turn_count > 0);
        //TS_ASSERT(g.turn_count == num_ticks / 2);
        TS_ASSERT(g.ct.has<dead>(g.hero_id));
        TS_ASSERT(!g.ct.get<dead>(g.hero_id).value());
        const int npc_count = g.count_live_npcs_on_floor(0);
        TS_ASSERT(npc_count > 0);
        // No more than 1 NPC per tile
    }



    void testCombat1v1() {
        minfo("----------------------------------------");
        minfo("test combat 1v1");
        minfo("----------------------------------------");

        unsigned int hero_death_count = 0;

        constexpr unsigned int sim_count = 10000;

        //vector<string> seeds;


        auto now = system_clock::now();
        auto now_dur = duration_cast<nanoseconds>(now.time_since_epoch());
        auto begin_time = now_dur.count();

        for (unsigned int i = 0; i < sim_count; i++) {
            gamestate g;
            g.test = true;
            g.init_dungeon(BIOME_STONE, 1, 32, 32);
            TS_ASSERT(g.d.floors.size() > 0);

            const vec3 player_loc = g.d.floors[0].df_get_random_loc();
            TS_ASSERT(!vec3_equal(player_loc, (vec3){-1, -1, -1}));

            const int maxhp_roll = 10;

            g.entity_turn = g.create_player_with(player_loc, "darkmage", g.player_init(maxhp_roll));
            TS_ASSERT(g.hero_id != ENTITYID_INVALID);
            TS_ASSERT(g.entity_turn != ENTITYID_INVALID);

            constexpr int monster_count = 1;
            TS_ASSERT(g.d.floors.size() > 0);

            const vec3 loc = g.d.floors[0].df_get_random_loc();
            TS_ASSERT(!vec3_equal(loc, (vec3){-1, -1, -1}));

            const entityid id = g.create_random_monster_at_with(loc, [](CT& ct, const entityid id) {});
            TS_ASSERT(id != ENTITYID_INVALID);
            TS_ASSERT(g.ct.get<hp>(id).value_or(0) >= 1);
            TS_ASSERT(g.ct.get<maxhp>(id).value_or(0) >= 1);

            g.ct.set<tactics>(
                id,
                (vector<tactic>){
                    {tactic_target::enemy, tactic_condition::adjacent, tactic_action::attack},
                    {tactic_target::nil, tactic_condition::any, tactic_action::move},
                });

            inputstate is;
            inputstate_update(is);
            g.current_scene = SCENE_GAMEPLAY;
            TS_ASSERT(g.test);
            TS_ASSERT(g.current_scene == SCENE_GAMEPLAY);

            constexpr int num_ticks = 100;
            minfo("simulating game for %d ticks...", num_ticks);
            for (int i = 0; i < num_ticks; i++) {
                g.tick(is);
            }
            minfo("simulation complete");
            minfo("turn count: %d", g.turn_count);
            TS_ASSERT(g.turn_count > 0);
            TS_ASSERT(g.turn_count == num_ticks / 2);

            //TS_ASSERT(g.ct.has<dead>(g.hero_id));
            //TS_ASSERT(!g.ct.get<dead>(g.hero_id).value());
            const unsigned int live_npc_count = g.count_live_npcs_on_floor(0);
            minfo("live npc_count: %u", live_npc_count);

            minfo("id %d race: %d", g.hero_id, g.ct.get<race>(g.hero_id).value_or(RACE_NONE));
            minfo("id %d hp: %d", g.hero_id, g.ct.get<hp>(g.hero_id).value_or(-666));
            minfo("id %d maxhp: %d", g.hero_id, g.ct.get<maxhp>(g.hero_id).value_or(-666));
            minfo("id %d dead: %d", g.hero_id, g.ct.get<dead>(g.hero_id).value_or(true));

            minfo("id %d race: %d", id, g.ct.get<race>(id).value_or(RACE_NONE));
            minfo("id %d dead: %d", id, g.ct.get<dead>(id).value_or(true));
            minfo("id %d hp: %d", id, g.ct.get<hp>(id).value_or(-666));
            minfo("id %d maxhp: %d", id, g.ct.get<maxhp>(id).value_or(-666));

            if (g.ct.get<hp>(g.hero_id).value_or(-666) <= 0) {
                TS_ASSERT(g.ct.get<dead>(g.hero_id).value_or(true));
                hero_death_count++;
            } else {
                TS_ASSERT(!g.ct.get<dead>(g.hero_id).value_or(true));
            }

            //sleep(1);
        }

        now = system_clock::now();
        now_dur = duration_cast<nanoseconds>(now.time_since_epoch());
        auto end_time = now_dur.count();

        auto diff_time_ns = end_time - begin_time;
        long double diff_time_us = diff_time_ns / 1000.0;
        auto diff_time_ms = diff_time_us / 1000.0;
        auto diff_time_s = diff_time_ms / 1000.0;

        minfo("Hero died %d times across %d simulations", hero_death_count, sim_count);
        //minfo("%d simulations ran in %lld nanoseconds", sim_count, diff_time_ns);
        //minfo("%d simulations ran in %0.4llf microseconds", sim_count, diff_time_us);

        minfo("%d simulations ran in %0.4llf ms", sim_count, diff_time_ms);

        //minfo("%d simulations ran in %0.4llf seconds", sim_count, diff_time_s);


        //TS_ASSERT(npc_count > 0);
        // No more than 1 NPC per tile
    }
};
