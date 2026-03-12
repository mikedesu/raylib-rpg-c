#pragma once

#include "../gamestate.h"
#include "../vec3.h"
#include <cxxtest/TestSuite.h>

class DungeonBootstrapTestSuite : public CxxTest::TestSuite {
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
};
