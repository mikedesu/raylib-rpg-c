#pragma once

#include "dungeon_tile.h"
#include "gamestate.h"
#include "unit_test.h"

// Forward declarations of test functions
static void run_test_tile_creation(void);
static void run_test_tile_entity_management(void);
static void run_test_tile_visibility_and_exploration(void);
static void run_test_tile_features_and_resizing(void);

void test_dungeon_tiles(void) {
    run_test_tile_creation();
    run_test_tile_entity_management();
    run_test_tile_visibility_and_exploration();
    run_test_tile_features_and_resizing();
}

TEST(test_tile_creation) {
    tile_t* tile = tile_create(TILE_NONE);
    ASSERT(tile != NULL, "Failed to create tile");
    ASSERT(tile->type == TILE_NONE, "Tile type not initialized correctly");
    ASSERT(tile->entity_count == 0, "Entity count not initialized correctly");
    ASSERT(tile->entity_max == DUNGEON_TILE_MAX_ENTITIES_DEFAULT, "Entity max not initialized correctly");
    tile_free(tile);
}

TEST(test_tile_features_and_resizing) {
    // Test tile creation with different types
    tile_t* tile = tile_create(TILE_STONE_WALL_00);
    ASSERT(tile != NULL, "Failed to create wall tile");
    ASSERT(tile->type == TILE_STONE_WALL_00, "Wall tile type not initialized correctly");
    tile_free(tile);
    // Test pressure plate functionality
    tile = tile_create(TILE_NONE);
    ASSERT(tile != NULL, "Failed to create tile");
    tile_set_pressure_plate(tile, true);
    ASSERT(tile_has_pressure_plate(tile) == true, "Failed to set pressure plate");
    ASSERT(tile_get_pressure_plate_event(tile) == -1, "Pressure plate event should be -1 by default");
    tile_set_pressure_plate_event(tile, 42);
    ASSERT(tile_get_pressure_plate_event(tile) == 42, "Failed to set pressure plate event");
    tile_free(tile);
    // Test wall switch functionality
    tile = tile_create(TILE_NONE);
    ASSERT(tile != NULL, "Failed to create tile");
    tile_set_wall_switch(tile, true);
    ASSERT(tile_has_wall_switch(tile) == true, "Failed to set wall switch");
    ASSERT(tile_is_wall_switch_on(tile) == false, "Wall switch should be off by default");
    tile_set_wall_switch_on(tile, true);
    ASSERT(tile_is_wall_switch_on(tile) == true, "Failed to turn on wall switch");
    tile_free(tile);
    // Test tile resizing
    tile = tile_create(TILE_NONE);
    ASSERT(tile != NULL, "Failed to create tile");
    // Fill the tile to capacity
    for (int i = 0; i < DUNGEON_TILE_MAX_ENTITIES_DEFAULT; i++) {
        ASSERT(tile_add(tile, i) == i, "Failed to add entity during resize test");
    }
    // Add one more to trigger resize
    ASSERT(tile_add(tile, DUNGEON_TILE_MAX_ENTITIES_DEFAULT) == DUNGEON_TILE_MAX_ENTITIES_DEFAULT, "Failed to add entity after resize");
    ASSERT(tile->entity_max > DUNGEON_TILE_MAX_ENTITIES_DEFAULT, "Tile did not resize properly");
    tile_free(tile);
}

TEST(test_tile_entity_management) {
    tile_t* tile = tile_create(TILE_NONE);
    ASSERT(tile != NULL, "Failed to create tile");
    // Test adding entities
    entityid id1 = 1;
    entityid id2 = 2;
    entityid id3 = 3;
    ASSERT(tile_add(tile, id1) == id1, "Failed to add first entity");
    ASSERT(tile->entity_count == 1, "Entity count incorrect after first add");
    ASSERT(tile_add(tile, id2) == id2, "Failed to add second entity");
    ASSERT(tile->entity_count == 2, "Entity count incorrect after second add");
    ASSERT(tile_add(tile, id3) == id3, "Failed to add third entity");
    ASSERT(tile->entity_count == 3, "Entity count incorrect after third add");
    // Test removing entities
    ASSERT(tile_remove(tile, id2) == id2, "Failed to remove entity");
    ASSERT(tile->entity_count == 2, "Entity count incorrect after removal");
    // Clean up
    tile_free(tile);
}

TEST(test_tile_visibility_and_exploration) {
    tile_t* tile = tile_create(TILE_NONE);
    ASSERT(tile != NULL, "Failed to create tile");
    // Initial state checks
    ASSERT(tile->visible == false, "Tile should not be visible by default");
    ASSERT(tile->explored == false, "Tile should not be explored by default");
    // Test visibility
    tile->visible = true;
    ASSERT(tile->visible == true, "Failed to set tile visibility");
    // Test exploration
    tile->explored = true;
    ASSERT(tile->explored == true, "Failed to set tile exploration");
    // Test visibility reset
    tile->visible = false;
    ASSERT(tile->visible == false, "Failed to reset tile visibility");
    // Exploration should persist after visibility reset
    ASSERT(tile->explored == true, "Tile exploration state should persist");
    // Clean up
    tile_free(tile);
}
