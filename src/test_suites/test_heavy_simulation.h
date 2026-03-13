#pragma once

#include "../gamestate.h"
#include "../inputstate.h"
#include <cxxtest/TestSuite.h>

class HeavySimulationTestSuite : public CxxTest::TestSuite {
private:
    void add_open_floor(gamestate& g, int width = 8, int height = 8) {
        auto df = g.d.create_floor(BIOME_STONE, width, height);
        df->df_set_all_tiles(TILE_FLOOR_STONE_00);
        g.d.add_floor(df);
        g.d.is_initialized = true;
    }

    entityid find_live_npc_on_floor(gamestate& g, int floor, race_t race_value = RACE_NONE) {
        for (entityid id = 1; id < g.next_entityid; id++) {
            if (g.ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_NPC) {
                continue;
            }
            if (g.ct.get<dead>(id).value_or(true)) {
                continue;
            }
            if (race_value != RACE_NONE && g.ct.get<race>(id).value_or(RACE_NONE) != race_value) {
                continue;
            }
            const vec3 loc = g.ct.get<location>(id).value_or(vec3{-1, -1, -1});
            if (loc.z == floor) {
                return id;
            }
        }
        return ENTITYID_INVALID;
    }

    vec3 find_open_adjacent_tile(gamestate& g, vec3 origin) {
        auto df = g.d.get_floor(origin.z);
        static constexpr vec3 offsets[] = {
            vec3{0, -1, 0},
            vec3{-1, 0, 0},
            vec3{1, 0, 0},
            vec3{0, 1, 0},
        };

        for (const vec3 offset : offsets) {
            const vec3 loc{origin.x + offset.x, origin.y + offset.y, origin.z};
            if (loc.x < 0 || loc.x >= df->get_width() || loc.y < 0 || loc.y >= df->get_height()) {
                continue;
            }
            tile_t& tile = df->tile_at(loc);
            if (!tile_is_walkable(tile.get_type())) {
                continue;
            }
            if (tile.entity_count() != 0) {
                continue;
            }
            if (tile.get_type() == TILE_UPSTAIRS || tile.get_type() == TILE_DOWNSTAIRS) {
                continue;
            }
            return loc;
        }

        return vec3{-1, -1, -1};
    }

public:
    void testPathfindingSoakMovesHostileOrcThroughSingleGapTowardHero() {
        gamestate g;
        g.test = true;
        add_open_floor(g, 12, 12);
        g.d.current_floor = 0;

        auto df = g.d.get_floor(0);
        for (int y = 0; y < df->get_height(); ++y) {
            if (y == 6) {
                continue;
            }
            df->df_set_tile(TILE_STONE_WALL_00, 5, y);
        }

        const entityid hero = g.create_player_at_with(vec3{9, 6, 0}, "hero", g.player_init(18));
        const entityid orc = g.create_orc_at_with(vec3{1, 6, 0}, [](CT&, const entityid) {});

        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(orc, ENTITYID_INVALID);

        bool crossed_gap_column = false;
        bool became_adjacent = false;
        vec3 previous_loc = g.ct.get<location>(orc).value_or(vec3{-1, -1, -1});

        for (int step = 0; step < 32 && !became_adjacent; ++step) {
            g.update_npcs_state();

            const auto path = g.ct.get<target_path>(orc).value_or(nullptr);
            if (path) {
                path->clear();
            }
            g.update_path_to_target(orc);

            const auto refreshed_path = g.ct.get<target_path>(orc).value_or(nullptr);
            TS_ASSERT(refreshed_path);
            TS_ASSERT(!refreshed_path->empty());

            const vec3 next_step = refreshed_path->front();
            TS_ASSERT(next_step.x >= 0 && next_step.x < df->get_width());
            TS_ASSERT(next_step.y >= 0 && next_step.y < df->get_height());
            TS_ASSERT(tile_is_walkable(df->tile_at(next_step).get_type()));

            TS_ASSERT(g.handle_npc(orc));

            const vec3 orc_loc = g.ct.get<location>(orc).value_or(vec3{-1, -1, -1});
            TS_ASSERT(vec3_valid(orc_loc));
            TS_ASSERT(tile_is_walkable(df->tile_at(orc_loc).get_type()));
            TS_ASSERT(!vec3_equal(orc_loc, previous_loc));
            TS_ASSERT(orc_loc.x != 5 || orc_loc.y == 6);

            if (orc_loc.x > 5) {
                crossed_gap_column = true;
            }
            became_adjacent = g.is_entity_adjacent(orc, hero);
            previous_loc = orc_loc;
        }

        TS_ASSERT(crossed_gap_column);
        TS_ASSERT(became_adjacent);
        TS_ASSERT(g.ct.get<steps_taken>(orc).value_or(0) > 0);
    }

    void testTickHeavySimulationResolvesAdjacentOrcFightWithoutBreakingTileState() {
        gamestate g;
        g.test = true;
        g.sfx.resize(71);
        g.mt.seed(12345);
        g.logic_init();

        g.d.current_floor = 3;
        const entityid orc = find_live_npc_on_floor(g, 3, RACE_ORC);
        TS_ASSERT_DIFFERS(orc, ENTITYID_INVALID);

        const vec3 orc_loc = g.ct.get<location>(orc).value_or(vec3{-1, -1, -1});
        const vec3 hero_loc = find_open_adjacent_tile(g, orc_loc);
        TS_ASSERT(vec3_valid(hero_loc));

        g.entity_turn = g.create_player_at_with(hero_loc, "heavy_hero", g.player_init(18));
        TS_ASSERT_DIFFERS(g.hero_id, ENTITYID_INVALID);

        const entityid hero_weapon = g.create_weapon_with(g.sword_init());
        TS_ASSERT_DIFFERS(hero_weapon, ENTITYID_INVALID);
        g.add_to_inventory(g.hero_id, hero_weapon);
        g.ct.set<equipped_weapon>(g.hero_id, hero_weapon);
        g.ct.set<strength>(g.hero_id, 18);
        g.ct.set<dexterity>(g.hero_id, 18);
        g.ct.set<hp>(g.hero_id, vec2{20, 20});

        g.make_all_npcs_target_player();
        g.current_scene = SCENE_GAMEPLAY;

        inputstate is;
        inputstate_reset(is);

        bool resolved = false;
        for (int step = 0; step < 256 && !resolved; ++step) {
            g.entity_turn = orc;
            g.flag = GAMESTATE_FLAG_NPC_TURN;
            g.tick(is);

            const bool hero_dead = g.ct.get<dead>(g.hero_id).value_or(true);
            const bool orc_dead = g.ct.get<dead>(orc).value_or(true);
            resolved = hero_dead || orc_dead;
        }

        TS_ASSERT(resolved);
        TS_ASSERT_DIFFERS(g.ct.get<dead>(g.hero_id).value_or(false), g.ct.get<dead>(orc).value_or(false));

        tile_t& hero_tile = g.d.get_floor(3)->tile_at(hero_loc);
        tile_t& orc_tile = g.d.get_floor(3)->tile_at(orc_loc);
        if (g.ct.get<dead>(orc).value_or(false)) {
            TS_ASSERT_EQUALS(orc_tile.get_cached_live_npc(), ENTITYID_INVALID);
            TS_ASSERT_EQUALS(orc_tile.get_dead_npc_count(), 1U);
            TS_ASSERT_EQUALS(g.ct.get<xp>(g.hero_id).value_or(0), 1);
        } else {
            TS_ASSERT_EQUALS(hero_tile.get_cached_live_npc(), ENTITYID_INVALID);
            TS_ASSERT_EQUALS(hero_tile.get_dead_npc_count(), 1U);
            TS_ASSERT_EQUALS(g.ct.get<xp>(orc).value_or(0), 0);
        }
    }
};
