#pragma once

#include "../gamestate.h"
#include "../inputstate.h"
#include <cxxtest/TestSuite.h>
#include <set>

class CombatBootstrapTestSuite : public CxxTest::TestSuite {
private:
    size_t count_entities_of_type(gamestate& g, entitytype_t type) {
        size_t count = 0;
        for (entityid id = 1; id < g.next_entityid; id++) {
            if (g.ct.get<entitytype>(id).value_or(ENTITY_NONE) == type) {
                count++;
            }
        }
        return count;
    }

    size_t count_live_npcs_on_floor(gamestate& g, int floor) {
        size_t count = 0;
        for (entityid id = 1; id < g.next_entityid; id++) {
            if (g.ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_NPC) {
                continue;
            }
            if (g.ct.get<dead>(id).value_or(true)) {
                continue;
            }
            const vec3 loc = g.ct.get<location>(id).value_or(vec3{-1, -1, -1});
            if (loc.z == floor) {
                count++;
            }
        }
        return count;
    }

    size_t count_live_npcs_of_race_on_floor(gamestate& g, race_t race_value, int floor) {
        size_t count = 0;
        for (entityid id = 1; id < g.next_entityid; id++) {
            if (g.ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_NPC) {
                continue;
            }
            if (g.ct.get<dead>(id).value_or(true)) {
                continue;
            }
            if (g.ct.get<race>(id).value_or(RACE_NONE) != race_value) {
                continue;
            }
            const vec3 loc = g.ct.get<location>(id).value_or(vec3{-1, -1, -1});
            if (loc.z == floor) {
                count++;
            }
        }
        return count;
    }

    entityid find_live_npc_on_floor(gamestate& g, int floor) {
        for (entityid id = 1; id < g.next_entityid; id++) {
            if (g.ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_NPC) {
                continue;
            }
            if (g.ct.get<dead>(id).value_or(true)) {
                continue;
            }
            const vec3 loc = g.ct.get<location>(id).value_or(vec3{-1, -1, -1});
            if (loc.z == floor) {
                return id;
            }
        }
        return ENTITYID_INVALID;
    }

    void add_floor(gamestate& g, int width = 8, int height = 8) {
        auto df = g.d.create_floor(BIOME_STONE, width, height);
        df->df_set_all_tiles(TILE_FLOOR_STONE_00);
        g.d.add_floor(df);
        g.d.is_initialized = true;
    }

public:
    void testCreateMultipleOrcsProducesUniqueIdsAndNpcState() {
        gamestate g;
        add_floor(g, 10, 10);

        std::set<entityid> ids;
        const vec3 locs[] = {{1, 1, 0}, {2, 1, 0}, {3, 1, 0}, {4, 1, 0}};
        for (const vec3 loc : locs) {
            const entityid id = g.create_orc_at_with(loc, [](CT&, const entityid) {});
            TS_ASSERT_DIFFERS(id, ENTITYID_INVALID);
            ids.insert(id);
            TS_ASSERT_EQUALS(g.ct.get<entitytype>(id).value_or(ENTITY_NONE), ENTITY_NPC);
            TS_ASSERT_EQUALS(g.ct.get<race>(id).value_or(RACE_NONE), RACE_ORC);
            TS_ASSERT(!g.ct.get<dead>(id).value_or(true));
        }

        TS_ASSERT_EQUALS(ids.size(), 4U);
        TS_ASSERT_EQUALS(count_live_npcs_on_floor(g, 0), 4U);
    }

    void testCreateOrcRejectsOccupiedTile() {
        gamestate g;
        add_floor(g, 8, 8);

        const vec3 loc{1, 1, 0};
        const entityid first = g.create_orc_at_with(loc, [](CT&, const entityid) {});
        const entityid second = g.create_orc_at_with(loc, [](CT&, const entityid) {});

        TS_ASSERT_DIFFERS(first, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(second, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(count_live_npcs_on_floor(g, 0), 1U);
    }

    void testCreateOrcStopsAtWalkableTileCapacityWithoutLeakingIds() {
        gamestate g;
        add_floor(g, 3, 3);

        std::set<entityid> ids;
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                const entityid id = g.create_orc_at_with(vec3{x, y, 0}, [](CT&, const entityid) {});
                TS_ASSERT_DIFFERS(id, ENTITYID_INVALID);
                ids.insert(id);
            }
        }

        TS_ASSERT_EQUALS(ids.size(), 9U);
        TS_ASSERT_EQUALS(count_live_npcs_on_floor(g, 0), 9U);

        const entityid before_next_entity = g.next_entityid;
        const entityid extra = g.create_orc_at_with(vec3{1, 1, 0}, [](CT&, const entityid) {});

        TS_ASSERT_EQUALS(extra, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.next_entityid, before_next_entity);
        TS_ASSERT_EQUALS(count_live_npcs_on_floor(g, 0), 9U);
    }

    void testLogicInitBuildsGameplayBootstrap() {
        gamestate g;
        g.test = true;
        g.mt.seed(12345);

        g.logic_init();

        TS_ASSERT(g.d.is_initialized);
        TS_ASSERT_EQUALS(g.d.get_floor_count(), 3U);
        TS_ASSERT_EQUALS(g.d.current_floor, 0);
        TS_ASSERT_EQUALS(g.d.get_floor(0)->get_width(), 8);
        TS_ASSERT_EQUALS(g.d.get_floor(0)->get_height(), 8);
        TS_ASSERT_EQUALS(g.d.get_floor(1)->get_width(), 16);
        TS_ASSERT_EQUALS(g.d.get_floor(1)->get_height(), 16);
        TS_ASSERT_EQUALS(g.d.get_floor(2)->get_width(), 24);
        TS_ASSERT_EQUALS(g.d.get_floor(2)->get_height(), 24);
        TS_ASSERT(vec3_valid(g.d.get_floor(0)->get_upstairs_loc()));
        TS_ASSERT(vec3_valid(g.d.get_floor(0)->get_downstairs_loc()));
        TS_ASSERT(count_entities_of_type(g, ENTITY_DOOR) >= 1U);
        TS_ASSERT(count_entities_of_type(g, ENTITY_BOX) >= 1U);
        TS_ASSERT(count_entities_of_type(g, ENTITY_ITEM) >= 2U);
        TS_ASSERT(count_live_npcs_on_floor(g, 0) >= 1U);
        TS_ASSERT(count_live_npcs_on_floor(g, 1) >= 1U);
        TS_ASSERT(count_live_npcs_on_floor(g, 2) >= 9U);
        TS_ASSERT(g.msg_system.size() >= 2U);
    }

    void testLogicInitPlacesFriendlyNpcAndGreenSlimesOnLaterFloors() {
        gamestate g;
        g.test = true;
        g.mt.seed(12345);

        g.logic_init();

        const entityid floor_zero_npc = find_live_npc_on_floor(g, 0);
        const entityid floor_one_npc = find_live_npc_on_floor(g, 1);

        TS_ASSERT_DIFFERS(floor_zero_npc, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(floor_one_npc, ENTITYID_INVALID);
        TS_ASSERT(!g.ct.get<aggro>(floor_zero_npc).value_or(true));
        TS_ASSERT_EQUALS(g.ct.get<race>(floor_one_npc).value_or(RACE_NONE), RACE_GREEN_SLIME);
        TS_ASSERT(!g.ct.get<aggro>(floor_one_npc).value_or(true));
        TS_ASSERT_EQUALS(count_live_npcs_of_race_on_floor(g, RACE_GREEN_SLIME, 2), 9U);
    }

    void testUpdateNpcsStateSetsFriendlyAndHostileDefaultActions() {
        gamestate g;
        add_floor(g, 8, 8);
        add_floor(g, 8, 8);

        const entityid friendly = g.create_npc_at_with(RACE_DWARF, vec3{1, 1, 0}, [](CT&, const entityid) {});
        const entityid hostile = g.create_orc_at_with(vec3{2, 2, 1}, [](CT&, const entityid) {});

        TS_ASSERT_DIFFERS(friendly, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(hostile, ENTITYID_INVALID);

        g.update_npcs_state();

        TS_ASSERT_EQUALS(g.ct.get<entity_default_action>(friendly).value_or(ENTITY_DEFAULT_ACTION_NONE), ENTITY_DEFAULT_ACTION_RANDOM_MOVE);
        TS_ASSERT_EQUALS(g.ct.get<entity_default_action>(hostile).value_or(ENTITY_DEFAULT_ACTION_NONE), ENTITY_DEFAULT_ACTION_RANDOM_MOVE);
        TS_ASSERT_EQUALS(g.ct.get<target_id>(hostile).value_or(ENTITYID_INVALID), ENTITYID_INVALID);

        g.d.current_floor = 1;
        g.hero_id = g.create_player_at_with(vec3{4, 4, 1}, "hero", g.player_init(10));
        g.update_npcs_state();

        TS_ASSERT_EQUALS(g.ct.get<target_id>(hostile).value_or(ENTITYID_INVALID), g.hero_id);
        TS_ASSERT_EQUALS(g.ct.get<entity_default_action>(hostile).value_or(ENTITY_DEFAULT_ACTION_NONE),
                         ENTITY_DEFAULT_ACTION_MOVE_TO_TARGET_AND_ATTACK_TARGET_IF_ADJACENT);

        g.hero_id = g.create_player_at_with(vec3{3, 2, 1}, "hero_adjacent", g.player_init(10));
        g.update_npcs_state();

        TS_ASSERT_EQUALS(g.ct.get<entity_default_action>(hostile).value_or(ENTITY_DEFAULT_ACTION_NONE),
                         ENTITY_DEFAULT_ACTION_ATTACK_TARGET_IF_ADJACENT);
    }

    void testProvokeNpcTurnsFriendlyNpcHostile() {
        gamestate g;
        add_floor(g, 8, 8);

        const entityid friendly = g.create_npc_at_with(RACE_DWARF, vec3{2, 1, 0}, [](CT&, const entityid) {});
        const entityid hero = g.create_player_at_with(vec3{1, 1, 0}, "hero", g.player_init(10));

        TS_ASSERT_DIFFERS(friendly, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT(!g.ct.get<aggro>(friendly).value_or(true));

        g.provoke_npc(friendly, hero);

        TS_ASSERT(g.ct.get<aggro>(friendly).value_or(false));
        TS_ASSERT_EQUALS(g.ct.get<target_id>(friendly).value_or(ENTITYID_INVALID), hero);
        TS_ASSERT_EQUALS(g.ct.get<entity_default_action>(friendly).value_or(ENTITY_DEFAULT_ACTION_NONE),
                         ENTITY_DEFAULT_ACTION_ATTACK_TARGET_IF_ADJACENT);
    }

    void testAttackingFriendlyNpcSetsAggro() {
        gamestate g;
        add_floor(g, 8, 8);

        const entityid friendly = g.create_npc_at_with(RACE_DWARF, vec3{2, 1, 0}, [](CT&, const entityid) {});
        const entityid hero = g.create_player_at_with(vec3{1, 1, 0}, "hero", g.player_init(10));
        tile_t& target_tile = g.d.get_floor(0)->tile_at(vec3{2, 1, 0});

        TS_ASSERT_DIFFERS(friendly, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT(!g.ct.get<aggro>(friendly).value_or(true));

        g.process_attack_entity(target_tile, hero, friendly);

        TS_ASSERT(g.ct.get<aggro>(friendly).value_or(false));
        TS_ASSERT_EQUALS(g.ct.get<target_id>(friendly).value_or(ENTITYID_INVALID), hero);
    }

    void testTickInTestModeAdvancesTicksAndTurnsWithHeroPresent() {
        gamestate g;
        g.test = true;
        g.sfx.resize(71);
        g.logic_init();

        const vec3 hero_loc = g.d.get_floor(0)->get_random_loc();
        TS_ASSERT(vec3_valid(hero_loc));

        const int maxhp_roll = 10;
        g.entity_turn = g.create_player_at_with(hero_loc, "darkmage", g.player_init(maxhp_roll));
        TS_ASSERT_DIFFERS(g.hero_id, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.entity_turn, g.hero_id);

        g.make_all_npcs_target_player();
        g.current_scene = SCENE_GAMEPLAY;

        inputstate is;
        inputstate_reset(is);

        const unsigned int before_turns = g.turn_count;
        const unsigned long before_ticks = g.ticks;
        const int num_ticks = static_cast<int>(g.next_entityid) * 2;
        for (int i = 0; i < num_ticks; i++) {
            g.tick(is);
        }

        TS_ASSERT(g.ticks >= before_ticks + static_cast<unsigned long>(num_ticks));
        TS_ASSERT(g.turn_count > before_turns);
        TS_ASSERT(g.ct.has<dead>(g.hero_id));
        TS_ASSERT(!g.ct.get<dead>(g.hero_id).value_or(true));
    }
};
