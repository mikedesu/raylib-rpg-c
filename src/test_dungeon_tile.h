#pragma once

#include "dungeon_tile.h"
#include "unit_test.h"

// Forward declarations of test functions
static void run_test_tile_creation(void);
static void run_test_tile_entity_management(void);

void test_dungeon_tiles(void) {
    run_test_tile_creation();
    run_test_tile_entity_management();
}

TEST(test_tile_creation) {
    tile_t* tile = tile_create(TILE_NONE);
    ASSERT(tile != NULL, "Failed to create tile");
    ASSERT(tile->type == TILE_NONE, "Tile type not initialized correctly");
    ASSERT(tile->entity_count == 0, "Entity count not initialized correctly");
    ASSERT(tile->entity_max == DUNGEON_TILE_MAX_ENTITIES_DEFAULT, "Entity max not initialized correctly");
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
