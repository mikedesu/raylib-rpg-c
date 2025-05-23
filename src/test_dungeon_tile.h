#pragma once

#include "dungeon_tile.h"
#include "gamestate.h"
#include "unit_test.h"
#include <stdlib.h>
// Forward declarations of test functions
//static void run_test_tile_creation(void);
//static void run_test_tile_entity_management(void);
//static void run_test_tile_visibility_and_exploration(void);
//static void run_test_tile_features_and_resizing(void);

void test_dungeon_tiles(void);

TEST(test_tile_serialization) {
    // Create and populate a test tile
    tile_t* original = tile_create(TILE_STONE_WALL_00);
    ASSERT(original != NULL, "Failed to create original tile");

    // Add some entities
    tile_add(original, 1);
    tile_add(original, 2);
    tile_add(original, 3);

    // Set various properties
    original->visible = true;
    original->explored = true;
    original->has_pressure_plate = true;
    original->pressure_plate_event = 42;
    original->has_wall_switch = true;
    original->wall_switch_on = true;
    original->wall_switch_event = 99;

    // Serialize
    size_t buffer_size = tile_serialized_size(original);
    char* buffer = (char*)malloc(buffer_size);
    ASSERT(buffer != NULL, "Failed to allocate serialization buffer");

    size_t bytes_written = tile_serialize(original, buffer, buffer_size);
    printf("Serialized size: %zu\n", bytes_written);
    printf("Buffer size: %zu\n", buffer_size);
    printf("Difference: %zd\n", (ssize_t)bytes_written - (ssize_t)buffer_size);
    printf("Serialized data: ");
    for (size_t i = 0; i < bytes_written; i++) {
        printf("%02X ", (unsigned char)buffer[i]);
    }
    printf("\n");
    ASSERT(bytes_written == buffer_size, "Serialization wrote incorrect number of bytes");

    // Deserialize
    tile_t* deserialized = tile_create(TILE_NONE); // Create empty tile
    ASSERT(deserialized != NULL, "Failed to create deserialized tile");

    bool success = tile_deserialize(deserialized, buffer, buffer_size);
    ASSERT(success, "Deserialization failed");

    // Verify all fields match
    ASSERT(deserialized->type == original->type, "Type mismatch after deserialization");
    ASSERT(deserialized->visible == original->visible, "Visible mismatch after deserialization");
    ASSERT(deserialized->explored == original->explored, "Explored mismatch after deserialization");
    ASSERT(deserialized->has_pressure_plate == original->has_pressure_plate, "Pressure plate mismatch");
    ASSERT(deserialized->pressure_plate_event == original->pressure_plate_event, "Pressure plate event mismatch");
    ASSERT(deserialized->has_wall_switch == original->has_wall_switch, "Wall switch mismatch");
    ASSERT(deserialized->wall_switch_on == original->wall_switch_on, "Wall switch state mismatch");
    ASSERT(deserialized->wall_switch_event == original->wall_switch_event, "Wall switch event mismatch");
    ASSERT(deserialized->entity_count == original->entity_count, "Entity count mismatch");
    ASSERT(deserialized->entity_max == original->entity_max, "Entity max mismatch");

    // Verify entities
    for (size_t i = 0; i < original->entity_count; i++) {
        ASSERT(deserialized->entities[i] == original->entities[i], "Entity mismatch at index %zu", i);
    }

    // Clean up
    tile_free(original);
    tile_free(deserialized);
    free(buffer);
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

void test_dungeon_tiles(void) {
    run_test_tile_creation();
    run_test_tile_entity_management();
    run_test_tile_visibility_and_exploration();
    run_test_tile_features_and_resizing();
    run_test_tile_serialization();
}
