#pragma once

#include "../dead_npc_cache.h"
#include "../dungeon_tile.h"
#include "../item_cache.h"
#include <cxxtest/TestSuite.h>

class TileCacheTestSuite : public CxxTest::TestSuite {
public:
    void testDeadNpcCacheBasic() {
        dead_npc_cache cache;
        TS_ASSERT_EQUALS(cache.get_count(), 0U);

        cache.add_id(100);
        TS_ASSERT_EQUALS(cache.get_count(), 1U);
        TS_ASSERT(cache.contains(100) != -1);

        cache.remove_id(100);
        TS_ASSERT_EQUALS(cache.get_count(), 0U);
        TS_ASSERT_EQUALS(cache.contains(100), -1);
    }

    void testDeadNpcCacheBoundaries() {
        dead_npc_cache cache;

        for (int i = 0; i < DEAD_NPC_CACHE_SIZE; i++) {
            cache.add_id(i);
            TS_ASSERT_EQUALS(cache.get_count(), static_cast<size_t>(i + 1));
        }

        for (int i = 0; i < DEAD_NPC_CACHE_SIZE; i++) {
            TS_ASSERT(cache.contains(i) != -1);
        }

        cache.add_id(9);
        TS_ASSERT_EQUALS(cache.get_count(), static_cast<size_t>(DEAD_NPC_CACHE_SIZE));
        TS_ASSERT_EQUALS(cache.contains(0), -1);
        TS_ASSERT(cache.contains(9) != -1);
    }

    void testDeadNpcCacheRemove() {
        dead_npc_cache cache;
        cache.add_id(1);
        cache.add_id(2);
        cache.add_id(3);

        TS_ASSERT(cache.remove_id(2));
        TS_ASSERT_EQUALS(cache.get_count(), 2U);
        TS_ASSERT_EQUALS(cache.contains(2), -1);
        TS_ASSERT_EQUALS(cache.contains(3), 1);

        TS_ASSERT(!cache.remove_id(999));
        TS_ASSERT_EQUALS(cache.get_count(), 2U);
    }

    void testDeadNpcCacheTopAndPop() {
        dead_npc_cache cache;
        TS_ASSERT_EQUALS(cache.top(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(cache.pop(), ENTITYID_INVALID);

        cache.add_id(10);
        cache.add_id(20);
        cache.add_id(30);

        TS_ASSERT_EQUALS(cache.top(), 30);
        TS_ASSERT_EQUALS(cache.pop(), 30);
        TS_ASSERT_EQUALS(cache.pop(), 20);
        TS_ASSERT_EQUALS(cache.pop(), 10);
        TS_ASSERT_EQUALS(cache.pop(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(cache.get_count(), 0U);
    }

    void testItemCacheBasic() {
        item_cache cache;

        TS_ASSERT_EQUALS(cache.get_count(), 0U);
        TS_ASSERT_EQUALS(cache.top(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(cache.at(0), ENTITYID_INVALID);

        TS_ASSERT(cache.add_id(11));
        TS_ASSERT(cache.add_id(12));
        TS_ASSERT(cache.add_id(13));

        TS_ASSERT_EQUALS(cache.get_count(), 3U);
        TS_ASSERT_EQUALS(cache.contains(11), 0);
        TS_ASSERT_EQUALS(cache.contains(12), 1);
        TS_ASSERT_EQUALS(cache.contains(13), 2);
        TS_ASSERT_EQUALS(cache.top(), 13);
        TS_ASSERT_EQUALS(cache.at(1), 12);
        TS_ASSERT_EQUALS(cache.at(99), ENTITYID_INVALID);
    }

    void testItemCacheCapacityAndRemove() {
        item_cache cache;

        for (int i = 0; i < ITEM_CACHE_SIZE; i++) {
            TS_ASSERT(cache.add_id(100 + i));
        }

        TS_ASSERT_EQUALS(cache.get_count(), static_cast<size_t>(ITEM_CACHE_SIZE));
        TS_ASSERT(!cache.add_id(999));

        TS_ASSERT(cache.remove_id(103));
        TS_ASSERT_EQUALS(cache.get_count(), static_cast<size_t>(ITEM_CACHE_SIZE - 1));
        TS_ASSERT_EQUALS(cache.contains(103), -1);
        TS_ASSERT_EQUALS(cache.contains(104), 3);

        TS_ASSERT(!cache.remove_id(999));
    }

    void testTileInitAndCacheReset() {
        tile_t tile(TILE_FLOOR_STONE_00);

        TS_ASSERT_EQUALS(tile.get_type(), TILE_FLOOR_STONE_00);
        TS_ASSERT(!tile.get_visible());
        TS_ASSERT(!tile.get_explored());
        TS_ASSERT(!tile.get_cached_player_present());
        TS_ASSERT_EQUALS(tile.get_cached_live_npc(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(tile.get_cached_item(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(tile.get_cached_dead_npc(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(tile.entity_count(), 0U);
        TS_ASSERT(tile.get_dirty_entities());

        tile.set_visible(true);
        tile.set_explored(true);
        tile.set_cached_player_present(true);
        tile.set_cached_live_npc(50);
        tile.set_cached_item(60);
        tile.set_cached_box(70);
        tile.set_cached_chest(75);
        tile.set_cached_prop(80);
        tile.set_cached_door(90);
        tile.set_dirty_entities(false);
        tile.tile_reset_cache();

        TS_ASSERT_EQUALS(tile.get_type(), TILE_FLOOR_STONE_00);
        TS_ASSERT(tile.get_visible());
        TS_ASSERT(tile.get_explored());
        TS_ASSERT(!tile.get_cached_player_present());
        TS_ASSERT_EQUALS(tile.get_cached_live_npc(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(tile.get_cached_item(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(tile.get_cached_box(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(tile.get_cached_chest(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(tile.get_cached_prop(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(tile.get_cached_door(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(tile.entity_count(), 0U);
        TS_ASSERT(!tile.get_dirty_entities());
    }

    void testTileAddAndRemoveAcrossEntityKinds() {
        tile_t tile(TILE_FLOOR_STONE_00);

        TS_ASSERT_EQUALS(tile.tile_add(1, ENTITY_PLAYER), 1);
        TS_ASSERT(tile.get_cached_player_present());
        TS_ASSERT_EQUALS(tile.get_cached_live_npc(), 1);
        TS_ASSERT_EQUALS(tile.entity_count(), 1U);

        TS_ASSERT_EQUALS(tile.tile_remove(1), 1);
        TS_ASSERT(!tile.get_cached_player_present());
        TS_ASSERT_EQUALS(tile.get_cached_live_npc(), ENTITYID_INVALID);

        TS_ASSERT_EQUALS(tile.tile_add(2, ENTITY_NPC), 2);
        TS_ASSERT_EQUALS(tile.tile_add(3, ENTITY_ITEM), 3);
        TS_ASSERT_EQUALS(tile.tile_add(4, ENTITY_ITEM), 4);
        TS_ASSERT_EQUALS(tile.tile_add(5, ENTITY_PROP), 5);
        TS_ASSERT_EQUALS(tile.tile_add(6, ENTITY_BOX), 6);
        TS_ASSERT_EQUALS(tile.tile_add(7, ENTITY_CHEST), 7);
        TS_ASSERT_EQUALS(tile.tile_add(8, ENTITY_DOOR), 8);
        TS_ASSERT(tile.add_dead_npc(9));

        TS_ASSERT_EQUALS(tile.get_cached_live_npc(), 2);
        TS_ASSERT_EQUALS(tile.get_cached_item(), 4);
        TS_ASSERT_EQUALS(tile.get_cached_item_at(0), 3);
        TS_ASSERT_EQUALS(tile.get_cached_item_at(1), 4);
        TS_ASSERT_EQUALS(tile.get_cached_prop(), 5);
        TS_ASSERT_EQUALS(tile.get_cached_box(), 6);
        TS_ASSERT_EQUALS(tile.get_cached_chest(), 7);
        TS_ASSERT_EQUALS(tile.get_cached_door(), 8);
        TS_ASSERT_EQUALS(tile.get_cached_dead_npc(), 9);
        TS_ASSERT_EQUALS(tile.get_item_count(), 2U);
        TS_ASSERT_EQUALS(tile.get_dead_npc_count(), 1U);
        TS_ASSERT_EQUALS(tile.entity_count(), 8U);

        TS_ASSERT_EQUALS(tile.tile_remove(9), 9);
        TS_ASSERT_EQUALS(tile.get_dead_npc_count(), 0U);
        TS_ASSERT_EQUALS(tile.tile_remove(3), 3);
        TS_ASSERT_EQUALS(tile.get_item_count(), 1U);
        TS_ASSERT_EQUALS(tile.get_cached_item(), 4);
        TS_ASSERT_EQUALS(tile.tile_remove(5), 5);
        TS_ASSERT_EQUALS(tile.tile_remove(6), 6);
        TS_ASSERT_EQUALS(tile.tile_remove(7), 7);
        TS_ASSERT_EQUALS(tile.tile_remove(8), 8);
        TS_ASSERT_EQUALS(tile.tile_remove(2), 2);
        TS_ASSERT_EQUALS(tile.entity_count(), 1U);
        TS_ASSERT_EQUALS(tile.tile_remove(4), 4);
        TS_ASSERT_EQUALS(tile.entity_count(), 0U);
        TS_ASSERT_EQUALS(tile.tile_remove(999), ENTITYID_INVALID);
    }

    void testTileItemCacheLimit() {
        tile_t tile(TILE_FLOOR_STONE_00);

        for (int i = 0; i < ITEM_CACHE_SIZE; i++) {
            TS_ASSERT_EQUALS(tile.tile_add(200 + i, ENTITY_ITEM), 200 + i);
        }

        TS_ASSERT_EQUALS(tile.get_item_count(), static_cast<size_t>(ITEM_CACHE_SIZE));
        TS_ASSERT_EQUALS(tile.tile_add(999, ENTITY_ITEM), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(tile.get_item_count(), static_cast<size_t>(ITEM_CACHE_SIZE));
        TS_ASSERT_EQUALS(tile.get_cached_item(), 200 + ITEM_CACHE_SIZE - 1);
    }

    void testTileRejectsSecondDoorCacheEntry() {
        tile_t tile(TILE_FLOOR_STONE_00);

        TS_ASSERT_EQUALS(tile.tile_add(10, ENTITY_DOOR), 10);
        TS_ASSERT_EQUALS(tile.tile_add(11, ENTITY_DOOR), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(tile.get_cached_door(), 10);
        TS_ASSERT_EQUALS(tile.entity_count(), 1U);
    }

    void testTileSupportsMixedOccupancyAcrossSupportedCacheKinds() {
        tile_t tile(TILE_FLOOR_STONE_00);

        TS_ASSERT_EQUALS(tile.tile_add(10, ENTITY_PLAYER), 10);
        TS_ASSERT_EQUALS(tile.tile_add(20, ENTITY_ITEM), 20);
        TS_ASSERT_EQUALS(tile.tile_add(21, ENTITY_ITEM), 21);
        TS_ASSERT_EQUALS(tile.tile_add(30, ENTITY_PROP), 30);
        TS_ASSERT_EQUALS(tile.tile_add(40, ENTITY_BOX), 40);
        TS_ASSERT_EQUALS(tile.tile_add(45, ENTITY_CHEST), 45);
        TS_ASSERT_EQUALS(tile.tile_add(50, ENTITY_DOOR), 50);
        TS_ASSERT(tile.add_dead_npc(60));

        TS_ASSERT(tile.get_cached_player_present());
        TS_ASSERT_EQUALS(tile.get_cached_live_npc(), 10);
        TS_ASSERT_EQUALS(tile.get_cached_item_at(0), 20);
        TS_ASSERT_EQUALS(tile.get_cached_item(), 21);
        TS_ASSERT_EQUALS(tile.get_cached_prop(), 30);
        TS_ASSERT_EQUALS(tile.get_cached_box(), 40);
        TS_ASSERT_EQUALS(tile.get_cached_chest(), 45);
        TS_ASSERT_EQUALS(tile.get_cached_door(), 50);
        TS_ASSERT_EQUALS(tile.get_cached_dead_npc(), 60);
        TS_ASSERT_EQUALS(tile.get_item_count(), 2U);
        TS_ASSERT_EQUALS(tile.get_dead_npc_count(), 1U);
        TS_ASSERT_EQUALS(tile.entity_count(), 8U);
    }

    void testTileMixedOccupancyRemovalPreservesRemainingCaches() {
        tile_t tile(TILE_FLOOR_STONE_00);

        TS_ASSERT_EQUALS(tile.tile_add(10, ENTITY_PLAYER), 10);
        TS_ASSERT_EQUALS(tile.tile_add(20, ENTITY_ITEM), 20);
        TS_ASSERT_EQUALS(tile.tile_add(21, ENTITY_ITEM), 21);
        TS_ASSERT_EQUALS(tile.tile_add(30, ENTITY_PROP), 30);
        TS_ASSERT_EQUALS(tile.tile_add(40, ENTITY_BOX), 40);
        TS_ASSERT_EQUALS(tile.tile_add(45, ENTITY_CHEST), 45);
        TS_ASSERT_EQUALS(tile.tile_add(50, ENTITY_DOOR), 50);
        TS_ASSERT(tile.add_dead_npc(60));

        TS_ASSERT_EQUALS(tile.tile_remove(50), 50);
        TS_ASSERT_EQUALS(tile.tile_remove(40), 40);
        TS_ASSERT_EQUALS(tile.tile_remove(45), 45);
        TS_ASSERT_EQUALS(tile.tile_remove(60), 60);
        TS_ASSERT_EQUALS(tile.tile_remove(21), 21);

        TS_ASSERT(tile.get_cached_player_present());
        TS_ASSERT_EQUALS(tile.get_cached_live_npc(), 10);
        TS_ASSERT_EQUALS(tile.get_cached_item(), 20);
        TS_ASSERT_EQUALS(tile.get_cached_prop(), 30);
        TS_ASSERT_EQUALS(tile.get_cached_box(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(tile.get_cached_chest(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(tile.get_cached_door(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(tile.get_cached_dead_npc(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(tile.get_item_count(), 1U);
        TS_ASSERT_EQUALS(tile.get_dead_npc_count(), 0U);
        TS_ASSERT_EQUALS(tile.entity_count(), 3U);
    }

    void testTileTypeHelpers() {
        tile_t wall(TILE_STONE_WALL_00);
        tile_t floor(TILE_FLOOR_STONE_00);

        TS_ASSERT(wall.tile_is_wall());
        TS_ASSERT(!floor.tile_is_wall());
        TS_ASSERT(tile_is_walkable(TILE_FLOOR_STONE_00));
        TS_ASSERT(!tile_is_walkable(TILE_STONE_WALL_00));
        TS_ASSERT(tile_is_possible_upstairs(TILE_FLOOR_STONE_00));
        TS_ASSERT(tile_is_possible_downstairs(TILE_FLOOR_STONE_00));
        TS_ASSERT(!tile_is_possible_upstairs(TILE_DOWNSTAIRS));
        TS_ASSERT(!tile_is_possible_downstairs(TILE_UPSTAIRS));
    }
};
