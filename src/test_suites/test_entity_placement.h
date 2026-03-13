#pragma once

#include "../gamestate.h"
#include <cxxtest/TestSuite.h>

class EntityPlacementTestSuite : public CxxTest::TestSuite {
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

    void add_simple_floor(gamestate& g, int width = 5, int height = 5) {
        auto df = g.d.create_floor(BIOME_STONE, width, height);
        df->df_set_all_tiles(TILE_FLOOR_STONE_00);
        g.d.add_floor(df);
    }

    void add_chokepoint_floor(gamestate& g) {
        auto df = g.d.create_floor(BIOME_STONE, 7, 7);
        df->df_set_all_tiles(TILE_NONE);
        df->df_set_tile(TILE_FLOOR_STONE_00, 1, 3);
        df->df_set_tile(TILE_FLOOR_STONE_00, 2, 3);
        df->df_set_tile(TILE_FLOOR_STONE_00, 3, 3);
        df->df_set_tile(TILE_FLOOR_STONE_00, 4, 2);
        df->df_set_tile(TILE_FLOOR_STONE_00, 4, 3);
        df->df_set_tile(TILE_FLOOR_STONE_00, 4, 4);
        df->df_set_tile(TILE_FLOOR_STONE_00, 5, 2);
        df->df_set_tile(TILE_FLOOR_STONE_00, 5, 3);
        df->df_set_tile(TILE_FLOOR_STONE_00, 5, 4);
        g.d.add_floor(df);
    }

public:
    void testPlaceDoorsReturnsZeroOnEmptyDungeon() {
        gamestate g;
        TS_ASSERT_EQUALS(g.place_doors(), 0U);
        TS_ASSERT_EQUALS(count_entities_of_type(g, ENTITY_DOOR), 0U);
    }

    void testPlaceDoorsCreatesDoorEntitiesWithTileAndComponentState() {
        gamestate g;
        auto floor = g.d.create_floor(BIOME_STONE, 7, 7);
        floor->df_set_all_tiles(TILE_NONE);
        floor->df_set_tile(TILE_FLOOR_STONE_00, 1, 3);
        floor->df_set_tile(TILE_FLOOR_STONE_00, 2, 3);
        floor->df_set_tile(TILE_FLOOR_STONE_00, 3, 3);
        floor->df_set_tile(TILE_FLOOR_STONE_00, 4, 2);
        floor->df_set_tile(TILE_FLOOR_STONE_00, 4, 3);
        floor->df_set_tile(TILE_FLOOR_STONE_00, 4, 4);
        floor->df_set_tile(TILE_FLOOR_STONE_00, 5, 2);
        floor->df_set_tile(TILE_FLOOR_STONE_00, 5, 3);
        floor->df_set_tile(TILE_FLOOR_STONE_00, 5, 4);
        g.d.add_floor(floor);
        floor = g.d.get_floor(0);
        floor->df_refresh_door_candidates();

        const size_t placed = g.place_doors();
        TS_ASSERT(placed > 0);
        TS_ASSERT_EQUALS(count_entities_of_type(g, ENTITY_DOOR), placed);

        size_t verified = 0;
        for (entityid id = 1; id < g.next_entityid; id++) {
            if (g.ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_DOOR) {
                continue;
            }

            auto maybe_loc = g.ct.get<location>(id);
            TS_ASSERT(maybe_loc.has_value());
            const vec3 loc = maybe_loc.value_or(vec3{-1, -1, -1});
            TS_ASSERT(vec3_valid(loc));

            auto df = g.d.get_floor(static_cast<size_t>(loc.z));
            tile_t& tile = df->tile_at(loc);
            TS_ASSERT_EQUALS(tile.get_cached_door(), id);
            TS_ASSERT(tile_is_walkable(tile.get_type()));
            TS_ASSERT(g.ct.get<door_open>(id).has_value());
            TS_ASSERT(!g.ct.get<door_open>(id).value_or(true));
            TS_ASSERT(g.ct.get<update>(id).value_or(false));
            verified++;
        }

        TS_ASSERT_EQUALS(verified, placed);
    }

    void testPlacePropsReturnsZeroOnEmptyDungeon() {
        gamestate g;
        TS_ASSERT_EQUALS(g.place_props(), 0);
        TS_ASSERT_EQUALS(count_entities_of_type(g, ENTITY_PROP), 0U);
    }

    void testPlacePropsCreatesPropEntitiesWithTileAndComponentState() {
        gamestate g;
        add_simple_floor(g, 6, 6);

        const int placed = g.place_props();
        TS_ASSERT(placed > 0);
        TS_ASSERT_EQUALS(count_entities_of_type(g, ENTITY_PROP), static_cast<size_t>(placed));

        size_t verified = 0;
        for (entityid id = 1; id < g.next_entityid; id++) {
            if (g.ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_PROP) {
                continue;
            }

            auto maybe_loc = g.ct.get<location>(id);
            TS_ASSERT(maybe_loc.has_value());
            const vec3 loc = maybe_loc.value_or(vec3{-1, -1, -1});
            TS_ASSERT(vec3_valid(loc));

            auto df = g.d.get_floor(static_cast<size_t>(loc.z));
            tile_t& tile = df->tile_at(loc);
            TS_ASSERT_EQUALS(tile.get_cached_prop(), id);
            TS_ASSERT(tile_is_walkable(tile.get_type()));
            TS_ASSERT(tile.get_type() != TILE_UPSTAIRS);
            TS_ASSERT(tile.get_type() != TILE_DOWNSTAIRS);
            TS_ASSERT(!tile.get_can_have_door());
            TS_ASSERT(g.ct.get<proptype>(id).value_or(PROP_NONE) != PROP_NONE);
            TS_ASSERT(g.ct.get<name>(id).has_value());
            TS_ASSERT(g.ct.get<description>(id).has_value());
            TS_ASSERT(g.ct.get<solid>(id).has_value());
            TS_ASSERT(g.ct.get<pushable>(id).has_value());
            TS_ASSERT(g.ct.get<update>(id).value_or(false));
            verified++;
        }

        TS_ASSERT_EQUALS(verified, static_cast<size_t>(placed));
    }

    void testPlacePropsSkipsChokepointAndDoorApproachLayouts() {
        gamestate g;
        add_chokepoint_floor(g);

        const int placed = g.place_props();
        TS_ASSERT_EQUALS(placed, 0);
        TS_ASSERT_EQUALS(count_entities_of_type(g, ENTITY_PROP), 0U);
    }

    void testGetRandomLocSkipsOccupiedChestTile() {
        gamestate g;
        add_simple_floor(g, 4, 4);

        auto df = g.d.get_floor(0);
        const vec3 blocked_loc{1, 1, 0};
        const entityid chest_id = g.create_chest_at_with(blocked_loc, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(chest_id, ENTITYID_INVALID);

        const vec3 random_loc = df->get_random_loc();
        TS_ASSERT(vec3_valid(random_loc));
        TS_ASSERT(!vec3_equal(random_loc, blocked_loc));
    }
};
