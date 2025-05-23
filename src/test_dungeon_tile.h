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
        ASSERT(deserialized->entities[i] == original->entities[i], "Entity mismatch");
    }

    // Clean up
    tile_free(original);
    tile_free(deserialized);
    free(buffer);
}

TEST(test_tile_serialization_edge_cases) {
    printf("\n--- Testing tile serialization edge cases ---\n");

    // Test case 1: Empty tile (no entities, no special features)
    printf("Test case 1: Empty tile\n");
    tile_t* empty_tile = tile_create(TILE_FLOOR_STONE_00);
    ASSERT(empty_tile != NULL, "Failed to create empty tile");

    size_t empty_size = tile_serialized_size(empty_tile);
    char* empty_buffer = (char*)malloc(empty_size);
    ASSERT(empty_buffer != NULL, "Failed to allocate buffer for empty tile");

    size_t empty_bytes = tile_serialize(empty_tile, empty_buffer, empty_size);
    ASSERT(empty_bytes == empty_size, "Empty tile serialization size mismatch");

    tile_t* empty_deserialized = tile_create(TILE_NONE);
    ASSERT(empty_deserialized != NULL, "Failed to create deserialized empty tile");

    bool empty_success = tile_deserialize(empty_deserialized, empty_buffer, empty_size);
    ASSERT(empty_success, "Empty tile deserialization failed");
    ASSERT(empty_deserialized->type == TILE_FLOOR_STONE_00, "Empty tile type mismatch");
    ASSERT(empty_deserialized->entity_count == 0, "Empty tile should have no entities");

    tile_free(empty_tile);
    tile_free(empty_deserialized);
    free(empty_buffer);

    // Test case 2: Tile with maximum entities
    printf("Test case 2: Tile with many entities\n");
    tile_t* max_entity_tile = tile_create(TILE_FLOOR_STONE_01);
    ASSERT(max_entity_tile != NULL, "Failed to create max entity tile");

    // Add a significant number of entities (not truly max to avoid excessive test time)
    const int test_entity_count = 20;
    for (int i = 0; i < test_entity_count; i++) {
        tile_add(max_entity_tile, i + 100); // Use offset to avoid entity ID 0
    }
    ASSERT(max_entity_tile->entity_count == test_entity_count, "Failed to add all test entities");

    size_t max_size = tile_serialized_size(max_entity_tile);
    char* max_buffer = (char*)malloc(max_size);
    ASSERT(max_buffer != NULL, "Failed to allocate buffer for max entity tile");

    size_t max_bytes = tile_serialize(max_entity_tile, max_buffer, max_size);
    ASSERT(max_bytes == max_size, "Max entity tile serialization size mismatch");

    tile_t* max_deserialized = tile_create(TILE_NONE);
    ASSERT(max_deserialized != NULL, "Failed to create deserialized max entity tile");

    bool max_success = tile_deserialize(max_deserialized, max_buffer, max_size);
    ASSERT(max_success, "Max entity tile deserialization failed");
    ASSERT(max_deserialized->type == TILE_FLOOR_STONE_01, "Max entity tile type mismatch");
    ASSERT(max_deserialized->entity_count == test_entity_count, "Entity count mismatch after max deserialization");

    // Verify all entities were correctly deserialized
    for (int i = 0; i < test_entity_count; i++) {
        ASSERT(max_deserialized->entities[i] == i + 100, "Entity ID mismatch at index");
    }

    tile_free(max_entity_tile);
    tile_free(max_deserialized);
    free(max_buffer);

    // Test case 3: Tile with all special features enabled
    printf("Test case 3: Tile with all special features\n");
    tile_t* feature_tile = tile_create(TILE_FLOOR_STONE_02);
    ASSERT(feature_tile != NULL, "Failed to create feature tile");

    // Set all special features
    feature_tile->visible = true;
    feature_tile->explored = true;
    feature_tile->has_pressure_plate = true;
    feature_tile->pressure_plate_up_tx_key = 123;
    feature_tile->pressure_plate_down_tx_key = 456;
    feature_tile->pressure_plate_event = 789;
    feature_tile->has_wall_switch = true;
    feature_tile->wall_switch_on = true;
    feature_tile->wall_switch_up_tx_key = 321;
    feature_tile->wall_switch_down_tx_key = 654;
    feature_tile->wall_switch_event = 987;
    feature_tile->dirty_entities = true;
    feature_tile->dirty_visibility = true;
    feature_tile->cached_live_npcs = 5;
    feature_tile->cached_player_present = true;

    // Add a few entities
    tile_add(feature_tile, 42);
    tile_add(feature_tile, 43);

    size_t feature_size = tile_serialized_size(feature_tile);
    char* feature_buffer = (char*)malloc(feature_size);
    ASSERT(feature_buffer != NULL, "Failed to allocate buffer for feature tile");

    size_t feature_bytes = tile_serialize(feature_tile, feature_buffer, feature_size);
    ASSERT(feature_bytes == feature_size, "Feature tile serialization size mismatch");

    tile_t* feature_deserialized = tile_create(TILE_NONE);
    ASSERT(feature_deserialized != NULL, "Failed to create deserialized feature tile");

    bool feature_success = tile_deserialize(feature_deserialized, feature_buffer, feature_size);
    ASSERT(feature_success, "Feature tile deserialization failed");

    // Verify all special features were correctly deserialized
    ASSERT(feature_deserialized->type == TILE_FLOOR_STONE_02, "Feature tile type mismatch");
    ASSERT(feature_deserialized->visible == true, "Visible flag not deserialized correctly");
    ASSERT(feature_deserialized->explored == true, "Explored flag not deserialized correctly");
    ASSERT(feature_deserialized->has_pressure_plate == true, "Pressure plate flag not deserialized correctly");
    ASSERT(feature_deserialized->pressure_plate_up_tx_key == 123, "Pressure plate up key not deserialized correctly");
    ASSERT(feature_deserialized->pressure_plate_down_tx_key == 456, "Pressure plate down key not deserialized correctly");
    ASSERT(feature_deserialized->pressure_plate_event == 789, "Pressure plate event not deserialized correctly");
    ASSERT(feature_deserialized->has_wall_switch == true, "Wall switch flag not deserialized correctly");
    ASSERT(feature_deserialized->wall_switch_on == true, "Wall switch state not deserialized correctly");
    ASSERT(feature_deserialized->wall_switch_up_tx_key == 321, "Wall switch up key not deserialized correctly");
    ASSERT(feature_deserialized->wall_switch_down_tx_key == 654, "Wall switch down key not deserialized correctly");
    ASSERT(feature_deserialized->wall_switch_event == 987, "Wall switch event not deserialized correctly");
    ASSERT(feature_deserialized->cached_live_npcs == 5, "Cached NPCs count not deserialized correctly");
    ASSERT(feature_deserialized->cached_player_present == true, "Player present flag not deserialized correctly");
    ASSERT(feature_deserialized->entity_count == 2, "Entity count not deserialized correctly");
    ASSERT(feature_deserialized->entities[0] == 42, "First entity not deserialized correctly");
    ASSERT(feature_deserialized->entities[1] == 43, "Second entity not deserialized correctly");

    tile_free(feature_tile);
    tile_free(feature_deserialized);
    free(feature_buffer);

    printf("--- All tile serialization edge cases passed ---\n");
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
    run_test_tile_serialization_edge_cases();
}
