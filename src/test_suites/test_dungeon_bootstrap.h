#pragma once

#include "../gamestate.h"
#include "../vec3.h"
#include <cxxtest/TestSuite.h>
#include <queue>
#include <set>

class DungeonBootstrapTestSuite : public CxxTest::TestSuite {
private:
    std::set<vec3> collect_walkable_region(shared_ptr<dungeon_floor> df, vec3 start) {
        std::set<vec3> visited;
        if (!df || !vec3_valid(start)) {
            return visited;
        }

        std::queue<vec3> pending;
        pending.push(start);
        visited.insert(start);

        static constexpr vec3 offsets[] = {
            vec3{0, -1, 0},
            vec3{-1, 0, 0},
            vec3{1, 0, 0},
            vec3{0, 1, 0},
        };

        while (!pending.empty()) {
            const vec3 current = pending.front();
            pending.pop();

            for (const vec3 offset : offsets) {
                const vec3 next{current.x + offset.x, current.y + offset.y, current.z};
                if (next.x < 0 || next.x >= df->get_width() || next.y < 0 || next.y >= df->get_height()) {
                    continue;
                }
                if (visited.find(next) != visited.end()) {
                    continue;
                }
                if (!tile_is_walkable(df->tile_at(next).get_type())) {
                    continue;
                }

                visited.insert(next);
                pending.push(next);
            }
        }

        return visited;
    }

    size_t count_walkable_tiles(shared_ptr<dungeon_floor> df) {
        size_t count = 0;
        for (int y = 0; y < df->get_height(); ++y) {
            for (int x = 0; x < df->get_width(); ++x) {
                if (tile_is_walkable(df->tile_at(vec3{x, y, df->get_floor()}).get_type())) {
                    count++;
                }
            }
        }
        return count;
    }

public:
    void testGamestateInitDungeonCreatesRequestedFloors() {
        gamestate g;

        g.init_dungeon(BIOME_STONE, 2, 4.0f, 16, 16);

        TS_ASSERT(g.d.is_initialized);
        TS_ASSERT_EQUALS(g.d.get_floor_count(), 2U);

        for (size_t i = 0; i < g.d.get_floor_count(); i++) {
            auto df = g.d.get_floor(i);
            TS_ASSERT(df);
            TS_ASSERT_EQUALS(df->get_floor(), static_cast<int>(i));
            TS_ASSERT_EQUALS(df->get_width(), 16);
            TS_ASSERT_EQUALS(df->get_height(), 16);
            TS_ASSERT_EQUALS(df->get_biome(), BIOME_STONE);
        }
    }

    void testGamestateInitDungeonDoesNotReinitializeExistingDungeon() {
        gamestate g;

        g.init_dungeon(BIOME_STONE, 1, 4.0f, 16, 16);
        const size_t floor_count = g.d.get_floor_count();
        auto df0 = g.d.get_floor(0);
        const int width = df0->get_width();
        const int height = df0->get_height();

        g.init_dungeon(BIOME_STONE, 3, 4.0f, 32, 32);

        TS_ASSERT_EQUALS(g.d.get_floor_count(), floor_count);
        TS_ASSERT_EQUALS(g.d.get_floor(0)->get_width(), width);
        TS_ASSERT_EQUALS(g.d.get_floor(0)->get_height(), height);
    }

    void testGamestateInitDungeonAssignsDistinctValidStairsPerFloor() {
        gamestate g;
        g.init_dungeon(BIOME_STONE, 2, 4.0f, 16, 16);

        for (size_t i = 0; i < g.d.get_floor_count(); i++) {
            auto df = g.d.get_floor(i);
            const vec3 upstairs = df->get_upstairs_loc();
            const vec3 downstairs = df->get_downstairs_loc();

            TS_ASSERT(vec3_valid(upstairs));
            TS_ASSERT(vec3_valid(downstairs));
            TS_ASSERT_EQUALS(upstairs.z, static_cast<int>(i));
            TS_ASSERT_EQUALS(downstairs.z, static_cast<int>(i));
            TS_ASSERT(!vec3_equal(upstairs, downstairs));

            tile_t& upstairs_tile = df->tile_at(upstairs);
            tile_t& downstairs_tile = df->tile_at(downstairs);
            TS_ASSERT_EQUALS(upstairs_tile.get_type(), TILE_UPSTAIRS);
            TS_ASSERT_EQUALS(downstairs_tile.get_type(), TILE_DOWNSTAIRS);
            TS_ASSERT(!upstairs_tile.get_can_have_door());
            TS_ASSERT(!downstairs_tile.get_can_have_door());
        }
    }

    void testGamestateInitDungeonCompactMapHasValidSpawnableLocation() {
        gamestate g;
        g.init_dungeon(BIOME_STONE, 1, 4.0f, 8, 8);

        TS_ASSERT(g.d.is_initialized);
        TS_ASSERT_EQUALS(g.d.get_floor_count(), 1U);

        auto df = g.d.get_floor(0);
        TS_ASSERT_EQUALS(df->get_width(), 8);
        TS_ASSERT_EQUALS(df->get_height(), 8);

        const vec3 loc = df->get_random_loc();
        TS_ASSERT(vec3_valid(loc));
        TS_ASSERT_EQUALS(loc.z, 0);

        tile_t& tile = df->tile_at(loc);
        TS_ASSERT(tile_is_walkable(tile.get_type()));
        TS_ASSERT(tile.get_type() != TILE_UPSTAIRS);
        TS_ASSERT(tile.get_type() != TILE_DOWNSTAIRS);
        TS_ASSERT_EQUALS(tile.entity_count(), 0U);
    }

    void testGamestateInitDungeonMakesEachFloorSingleConnectedWalkableRegion() {
        gamestate g;
        g.mt.seed(12345);

        g.init_dungeon(BIOME_STONE, 4, 4.0f, 16, 16);

        for (size_t i = 0; i < g.d.get_floor_count(); i++) {
            auto df = g.d.get_floor(i);
            const vec3 upstairs = df->get_upstairs_loc();
            const vec3 downstairs = df->get_downstairs_loc();
            const std::set<vec3> connected = collect_walkable_region(df, upstairs);

            TS_ASSERT(vec3_valid(upstairs));
            TS_ASSERT(vec3_valid(downstairs));
            TS_ASSERT_EQUALS(connected.size(), count_walkable_tiles(df));
            TS_ASSERT(connected.find(downstairs) != connected.end());
        }
    }

    void testGamestateInitDungeonCompactFloorsKeepStairsConnected() {
        gamestate g;
        g.mt.seed(54321);

        g.init_dungeon(BIOME_STONE, 3, 4.0f, 8, 8);

        for (size_t i = 0; i < g.d.get_floor_count(); i++) {
            auto df = g.d.get_floor(i);
            const vec3 upstairs = df->get_upstairs_loc();
            const vec3 downstairs = df->get_downstairs_loc();
            const std::set<vec3> connected = collect_walkable_region(df, upstairs);

            TS_ASSERT_EQUALS(df->get_width(), 8);
            TS_ASSERT_EQUALS(df->get_height(), 8);
            TS_ASSERT(connected.find(downstairs) != connected.end());
            TS_ASSERT_EQUALS(connected.size(), count_walkable_tiles(df));
        }
    }

    void testPossibleStairsLocationsExcludeDoorCandidates() {
        gamestate g;
        auto df = g.d.create_floor(BIOME_STONE, 8, 8);
        df->df_set_all_tiles(TILE_FLOOR_STONE_00);
        const vec3 blocked_loc{3, 3, 0};
        df->df_set_can_have_door(blocked_loc);

        auto upstairs_locs = df->df_get_possible_upstairs_locs();
        auto downstairs_locs = df->df_get_possible_downstairs_locs();
        const bool upstairs_contains_blocked = std::any_of(
            upstairs_locs->begin(), upstairs_locs->end(), [&](const vec3& loc) { return vec3_equal(loc, blocked_loc); });
        const bool downstairs_contains_blocked = std::any_of(
            downstairs_locs->begin(), downstairs_locs->end(), [&](const vec3& loc) { return vec3_equal(loc, blocked_loc); });

        TS_ASSERT(upstairs_locs);
        TS_ASSERT(downstairs_locs);
        TS_ASSERT(!upstairs_contains_blocked);
        TS_ASSERT(!downstairs_contains_blocked);
    }

    void testStairsAssignmentRejectsDoorCandidateTile() {
        gamestate g;
        auto df = g.d.create_floor(BIOME_STONE, 8, 8);
        df->df_set_all_tiles(TILE_FLOOR_STONE_00);
        const vec3 blocked_loc{4, 4, 0};
        df->df_set_can_have_door(blocked_loc);

        TS_ASSERT(!df->df_set_upstairs_loc(blocked_loc));
        TS_ASSERT(!df->df_set_downstairs_loc(blocked_loc));
        TS_ASSERT_EQUALS(df->tile_at(blocked_loc).get_type(), TILE_FLOOR_STONE_00);
        TS_ASSERT(df->tile_at(blocked_loc).get_can_have_door());
    }
};
