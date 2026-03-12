#pragma once

#include "../gamestate.h"
#include <cxxtest/TestSuite.h>

class WorldInteractionTestSuite : public CxxTest::TestSuite {
private:
    void add_floor(gamestate& g, int width = 6, int height = 6) {
        auto df = g.d.create_floor(BIOME_STONE, width, height);
        df->df_set_all_tiles(TILE_FLOOR_STONE_00);
        g.d.add_floor(df);
        g.d.is_initialized = true;
    }

    entityid create_hero(gamestate& g, vec3 loc) {
        return g.create_player_at_with(loc, "hero", [](CT&, const entityid) {});
    }

public:
    void testTryEntityMoveBlockedByWall() {
        gamestate g;
        add_floor(g);
        const vec3 hero_loc{1, 1, 0};
        const entityid hero = create_hero(g, hero_loc);
        g.d.get_floor(0)->df_set_tile(TILE_STONE_WALL_00, 2, 1);

        TS_ASSERT(!g.try_entity_move(hero, vec3{1, 0, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), hero_loc));
    }

    void testTryEntityMoveBlockedBySolidPropAndClosedDoor() {
        gamestate g;
        add_floor(g);
        const vec3 hero_loc{1, 1, 0};
        const entityid hero = create_hero(g, hero_loc);

        const entityid prop_id = g.create_prop_at_with(PROP_DUNGEON_STATUE_00, vec3{2, 1, 0}, dungeon_prop_init(PROP_DUNGEON_STATUE_00));
        TS_ASSERT_DIFFERS(prop_id, ENTITYID_INVALID);
        TS_ASSERT(!g.try_entity_move(hero, vec3{1, 0, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), hero_loc));

        g.d.get_floor(0)->tile_at(vec3{2, 1, 0}).tile_remove(prop_id);
        const entityid door_id = g.create_door_at_with(vec3{2, 1, 0}, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(door_id, ENTITYID_INVALID);
        TS_ASSERT(!g.try_entity_move(hero, vec3{1, 0, 0}));

        g.ct.set<door_open>(door_id, true);
        TS_ASSERT(g.try_entity_move(hero, vec3{1, 0, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{2, 1, 0}));
    }

    void testTryEntityMovePushesBox() {
        gamestate g;
        add_floor(g);
        const entityid hero = create_hero(g, vec3{2, 2, 0});
        const entityid box = g.create_box_at_with(vec3{3, 2, 0});

        TS_ASSERT_DIFFERS(box, ENTITYID_INVALID);
        TS_ASSERT(g.try_entity_move(hero, vec3{1, 0, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{2, 2, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(box).value_or(vec3{-1, -1, -1}), vec3{4, 2, 0}));
        TS_ASSERT_EQUALS(g.d.get_floor(0)->tile_at(vec3{4, 2, 0}).get_cached_box(), box);
    }

    void testTryEntityPullMovesHeroAndBoxBackward() {
        gamestate g;
        add_floor(g);
        const entityid hero = create_hero(g, vec3{2, 2, 0});
        g.ct.set<direction>(hero, DIR_RIGHT);
        const entityid box = g.create_box_at_with(vec3{3, 2, 0});

        TS_ASSERT_DIFFERS(box, ENTITYID_INVALID);
        TS_ASSERT(g.try_entity_pull(hero));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{1, 2, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(box).value_or(vec3{-1, -1, -1}), vec3{2, 2, 0}));
        TS_ASSERT_EQUALS(g.d.get_floor(0)->tile_at(vec3{2, 2, 0}).get_cached_box(), box);
    }

    void testTryEntityPullDeadNpcKeepsDeadNpcCachePath() {
        gamestate g;
        add_floor(g);
        const entityid hero = create_hero(g, vec3{2, 2, 0});
        g.ct.set<direction>(hero, DIR_RIGHT);

        const vec3 corpse_loc{3, 2, 0};
        const entityid corpse = g.create_orc_at_with(corpse_loc, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(corpse, ENTITYID_INVALID);

        tile_t& corpse_tile = g.d.get_floor(0)->tile_at(corpse_loc);
        TS_ASSERT_EQUALS(corpse_tile.tile_remove(corpse), corpse);
        corpse_tile.add_dead_npc(corpse);
        g.ct.set<dead>(corpse, true);
        g.ct.set<pullable>(corpse, true);

        TS_ASSERT(g.try_entity_pull(hero));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{1, 2, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(corpse).value_or(vec3{-1, -1, -1}), vec3{2, 2, 0}));

        tile_t& new_tile = g.d.get_floor(0)->tile_at(vec3{2, 2, 0});
        TS_ASSERT_EQUALS(new_tile.get_cached_dead_npc(), corpse);
        TS_ASSERT_EQUALS(new_tile.get_cached_live_npc(), ENTITYID_INVALID);
    }

    void testTryEntityMoveRespectsDoorOnEntityFloorNotCurrentFloor() {
        gamestate g;
        add_floor(g);
        add_floor(g);
        g.d.current_floor = 0;

        const entityid npc = g.create_orc_at_with(vec3{1, 1, 1}, [](CT&, const entityid) {});
        const entityid door = g.create_door_at_with(vec3{2, 1, 1}, [](CT&, const entityid) {});

        TS_ASSERT_DIFFERS(npc, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);
        TS_ASSERT(!g.ct.get<door_open>(door).value_or(true));

        TS_ASSERT(!g.try_entity_move(npc, vec3{1, 0, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(npc).value_or(vec3{-1, -1, -1}), vec3{1, 1, 1}));
    }
};
