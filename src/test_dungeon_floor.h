#pragma once

#include "dungeon_floor.h"
#include "dungeon_tile_type.h"
#include "unit_test.h"
#include <string.h>

TEST(test_df_create_destroy) {
    dungeon_floor_t* df = df_create(0, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    ASSERT(df != NULL, "Failed to create dungeon floor");
    // Basic sanity checks
    ASSERT(df->width == DEFAULT_DUNGEON_FLOOR_WIDTH, "Incorrect width");
    ASSERT(df->height == DEFAULT_DUNGEON_FLOOR_HEIGHT, "Incorrect height");
    ASSERT(df->tiles != NULL, "Tiles array not allocated");
    ASSERT(df->rooms != NULL, "Rooms array not allocated");
    df_free(df);
    //printf("df_create and df_destroy passed\n");
}

TEST(test_df_rooms) {
    dungeon_floor_t* df = df_create(0, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    //minfo("Initial room count: %d", df->room_count);
    // Test room creation
    bool add_result = df_add_room_info(df, 10, 10, 5, 5, "TestRoom1");
    //printf("Add room result: %d\n", add_result);
    //printf("Room count after add: %d\n", df->room_count);
    if (add_result) {
        // Test room location
        int in_room = df_loc_is_in_room(df, (vec3){12, 12});
        //minfo("Location in room result: %d", in_room);
        const char* room_name = df_get_room_name(df, (vec3){12, 12});
        //minfo("Room name at location: %s", room_name ? room_name : "NULL");
    }
    // Test overlapping rooms - should fail
    bool overlap_result = df_add_room_info(df, 10, 10, 5, 5, "OverlapRoom");
    //printf("Add overlapping room result: %d\n", overlap_result);
    //printf("Room count after overlap attempt: %d\n", df->room_count);
    ASSERT(!overlap_result, "Should not be able to add overlapping room");
    ASSERT(df->room_count == 1, "Room count should not increase after failed overlap");
    // Test adjacent rooms (should be allowed)
    bool adjacent_result = df_add_room_info(df, 15, 15, 5, 5, "AdjacentRoom");
    //printf("Add adjacent room result: %d\n", adjacent_result);
    //printf("Room count after adjacent add: %d\n", df->room_count);
    ASSERT(adjacent_result, "Should be able to add adjacent room");
    df_free(df);
    //printf("df_room tests completed\n");
}

TEST(test_df_tiles) {
    dungeon_floor_t* df = df_create(0, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    vec3 loc = {5, 5};
    ASSERT(df_tile_at(df, loc) != NULL, "Failed to get tile");
    df_set_tile(df, TILE_STONE_WALL_00, loc.x, loc.y);
    ASSERT(df_type_at(df, loc.x, loc.y) == TILE_STONE_WALL_00, "Tile type not set");
    df_free(df);
}

TEST(test_df_stairs) {
    dungeon_floor_t* df = df_create(0, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    // Create a room to place stairs in
    bool res = df_add_room(df, 10, 10, 5, 5, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_01, "StairRoom");
    ASSERT(res, "Failed to add room for stairs");
    // Test stair placement
    df_assign_upstairs_in_area(df, 10, 10, 5, 5);
    df_assign_downstairs_in_area(df, 10, 10, 5, 5);
    ASSERT(df_get_upstairs(df).x != -1, "Upstairs not placed");
    ASSERT(df_get_downstairs(df).x != -1, "Downstairs not placed");
    //minfo("Upstairs location: (%d, %d)", df_get_upstairs(df).x, df_get_upstairs(df).y);
    //minfo("Dnstairs location: (%d, %d)", df_get_downstairs(df).x, df_get_downstairs(df).y);
    // make sure stairs are not in the same location
    ASSERT(df_get_upstairs(df).x != df_get_downstairs(df).x || df_get_upstairs(df).y != df_get_downstairs(df).y, "Stairs in same location");
    df_free(df);
}

TEST(test_df_get_room_at) {
    dungeon_floor_t* df = df_create(0, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    // Create a test room
    int x = 10, y = 10, w = 5, h = 5;
    const char* room_name = "TestRoom";
    ASSERT(df_add_room_info(df, x, y, w, h, room_name), "Failed to add room");
    // Test points inside the room
    for (int y0 = y; y0 < y + h; y0++) {
        for (int x0 = x; x0 < x + w; x0++) {
            const room_data_t* room = df_get_room_at(df, x0, y0);
            ASSERT(room != NULL, "Should find room for point inside room");
            ASSERT(strcmp(room->room_name, room_name) == 0, "Room name mismatch");
            ASSERT(room->x == x && room->y == y, "Room coordinates mismatch");
            ASSERT(room->w == w && room->h == h, "Room dimensions mismatch");
        }
    }
    // Test points outside the room
    ASSERT(df_get_room_at(df, x - 1, y) == NULL, "Should not find room for point left of room");
    ASSERT(df_get_room_at(df, x + w, y) == NULL, "Should not find room for point right of room");
    ASSERT(df_get_room_at(df, x, y - 1) == NULL, "Should not find room for point above room");
    ASSERT(df_get_room_at(df, x, y + h) == NULL, "Should not find room for point below room");
    df_free(df);
}

TEST(test_df_count_empty) {
    dungeon_floor_t* df = df_create(0, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    // Count walkable tiles in the room
    int expected_walkable = 0;
    int actual_walkable = df_count_walkable(df);
    //minfo("Expected walkable tiles: %d, Actual walkable tiles: %d\n", expected_walkable, actual_walkable);
    ASSERT(actual_walkable == expected_walkable, "Too few walkable tiles");
    df_free(df);
}

TEST(test_df_count_walkable) {
    dungeon_floor_t* df = df_create(0, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    // Create a test room
    int x = 10;
    int y = 10;
    int w = 5;
    int h = 5;
    ASSERT(df_add_room(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_01, "TestRoom"), "Failed to add room");
    // room count should be 1
    ASSERT(df->room_count == 1, "Room count should be 1 after adding a room");
    // Count walkable tiles in the room
    int expected_walkable = w * h;
    int actual_walkable = df_count_walkable(df);
    //minfo("Expected walkable tiles: %d, Actual walkable tiles: %d", expected_walkable, actual_walkable);
    ASSERT(actual_walkable >= expected_walkable, "Too few walkable tiles");
    // Add some walls and verify count decreases
    df_set_tile(df, TILE_STONE_WALL_00, x, y);
    //df_set_tile(df, TILE_STONE_WALL_00, room_x + 1, room_y);
    int new_walkable = df_count_walkable(df);
    ASSERT(new_walkable >= actual_walkable - 1, "Walkable count should decrease after adding walls");
    df_free(df);
}

TEST(test_df_init) {
    dungeon_floor_t* df = df_create(0, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    // Initialize the dungeon floor
    df_init(df);
    // Test basic initialization
    ASSERT(df->tiles != NULL, "Tiles not initialized");
    ASSERT(df->rooms != NULL, "Rooms not initialized");
    // Test tile initialization
    int walkable_count = df_count_walkable(df);
    ASSERT(walkable_count == 0, "There should be no walkable tiles created");
    // Test room initialization
    int room_count = df->room_count;
    ASSERT(room_count == 0, "There should be no rooms created");
    // create a room to test room initialization
    int x = 10;
    int y = 10;
    int w = 5;
    int h = 5;
    tiletype_t floor_type_1 = TILE_FLOOR_STONE_00;
    tiletype_t floor_type_2 = TILE_FLOOR_STONE_01;
    bool res = df_add_room(df, x, y, w, h, floor_type_1, floor_type_2, "TestRoom");
    ASSERT(res, "Failed to add test room");
    // Test room count after adding a room
    ASSERT(df->room_count == 1, "Room count should be 1 after adding a room");
    // Test tile types in the room
    for (int y0 = y; y0 < y + h; y0++) {
        for (int x0 = x; x0 < x + w; x0++) {
            tile_t* tile = df_tile_at(df, (vec3){x0, y0});
            ASSERT(tile != NULL, "Tile should not be NULL in the room");
            ASSERT(tile->type == TILE_FLOOR_STONE_00 || tile->type == TILE_FLOOR_STONE_01, "Tile type should be one of the room floor types");
        }
    }
    int room_count2 = 0;
    const char* prefix = "room";
    room_data_t* rooms = df_get_rooms_with_prefix(df, &room_count2, prefix);
    ASSERT(room_count2 == 0, "No rooms with prefix [room] should have been created");
    rooms = df_get_rooms_with_prefix(df, &room_count2, "TestRoom");
    ASSERT(room_count2 == 1, "One room with prefix [TestRoom] should have been created");
    // Test stair placement
    vec3 upstairs = df_get_upstairs(df);
    // stairs should not exist yet
    //ASSERT(upstairs.x == -1 && upstairs.y == -1, "Upstairs should not be placed yet");
    ASSERT(vec3_equal(upstairs, (vec3){-1, -1, -1}), "Upstairs should not be placed yet");
    vec3 downstairs = df_get_downstairs(df);
    ASSERT(vec3_equal(downstairs, (vec3){-1, -1, -1}), "Dnstairs should not be placed yet");
    ASSERT(upstairs.x == downstairs.x && upstairs.y == downstairs.y && upstairs.z == downstairs.z,
           "Upstairs and downstairs should be at the same location because they are not placed yet");
    // Assign stairs in the room
    df_assign_upstairs_in_area(df, x, y, w, h);
    df_assign_downstairs_in_area(df, x, y, w, h);
    upstairs = df_get_upstairs(df);
    downstairs = df_get_downstairs(df);
    // Stairs should now be placed
    ASSERT(upstairs.x != -1 && upstairs.y != -1, "Upstairs should be placed now");
    ASSERT(downstairs.x != -1 && downstairs.y != -1, "Downstairs should be placed now");
    // Test tile types
    tile_t* up_tile = df_tile_at(df, upstairs);
    tile_t* down_tile = df_tile_at(df, downstairs);
    ASSERT(up_tile->type == TILE_UPSTAIRS, "Upstairs tile type incorrect");
    ASSERT(down_tile->type == TILE_DOWNSTAIRS, "Downstairs tile type incorrect");
    df_free(df);
}

TEST(test_df_serialization) {
    // Create a very small dungeon floor for testing
    dungeon_floor_t* df = df_create(0, 2, 2); // Just 2x2 for easier debugging
    // Set up some simple tiles instead of using df_init
    for (int y = 0; y < df->height; y++)
        for (int x = 0; x < df->width; x++) tile_init(&df->tiles[y][x], TILE_FLOOR_STONE_00);
    // Set specific tiles for stairs
    df->upstairs_loc = (vec3){0, 0};
    df->downstairs_loc = (vec3){1, 1};
    tile_init(&df->tiles[0][0], TILE_UPSTAIRS);
    tile_init(&df->tiles[1][1], TILE_DOWNSTAIRS);
    // Add a test room
    df_add_room_info(df, 0, 0, 2, 2, "TestRoom");
    // Get serialized size
    size_t size = df_serialized_size(df);
    printf("Serialized size for 2x2 dungeon: %zu bytes\n", size);
    ASSERT(size > 0, "Serialized size should be greater than 0");
    // Allocate buffer for serialization
    char* buffer = (char*)malloc(size);
    ASSERT(buffer != NULL, "Failed to allocate serialization buffer");
    // Serialize
    size_t written = df_serialize(df, buffer, size);
    ASSERT(written > 0, "Serialization failed");
    ASSERT(written == size, "Serialized size mismatch");
    // Print buffer contents for debugging
    //printf("Serialized buffer contents (first 32 bytes):\n");
    //for (int i = 0; i < 32 && i < size; i++) {
    //    printf("%02X ", (unsigned char)buffer[i]);
    //    if ((i + 1) % 16 == 0) printf("\n");
    //}
    //printf("\n");
    // Create a new dungeon floor for deserialization
    dungeon_floor_t* df2 = (dungeon_floor_t*)malloc(sizeof(dungeon_floor_t));
    ASSERT(df2 != NULL, "Failed to allocate new dungeon floor");
    memset(df2, 0, sizeof(dungeon_floor_t)); // Initialize to zeros
    // Deserialize
    bool success = df_deserialize(df2, buffer, size);
    ASSERT(success, "Deserialization failed");
    // Verify basic properties
    ASSERT(df2->width == df->width, "Width mismatch");
    ASSERT(df2->height == df->height, "Height mismatch");
    ASSERT(df2->room_count == df->room_count, "Room count mismatch");
    // Verify stair locations
    ASSERT(df2->upstairs_loc.x == df->upstairs_loc.x, "Upstairs X mismatch");
    ASSERT(df2->upstairs_loc.y == df->upstairs_loc.y, "Upstairs Y mismatch");
    ASSERT(df2->downstairs_loc.x == df->downstairs_loc.x, "Downstairs X mismatch");
    ASSERT(df2->downstairs_loc.y == df->downstairs_loc.y, "Downstairs Y mismatch");
    // Clean up
    free(buffer);
    df_free(df);
    df_free(df2);
}

TEST(test_df_memory_size) {
    dungeon_floor_t* df = df_create(0, 10, 10);
    size_t initial_size = df_memory_size(df);
    printf("Initial dungeon floor memory size (10x10): %zu bytes\n", initial_size);
    df_add_room_info(df, 0, 0, 5, 5, "TestRoom");
    size_t with_room_size = df_memory_size(df);
    printf("Memory size after adding a room: %zu bytes\n", with_room_size);
    printf("Difference: %zu bytes\n", with_room_size - initial_size);
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
            tile_add(&df->tiles[y][x], y * 10 + x);
        }
    }
    size_t with_entities_size = df_memory_size(df);
    printf("Memory size after adding entities: %zu bytes\n", with_entities_size);
    printf("Difference: %zu bytes\n", with_entities_size - with_room_size);
    size_t total_tiles = df->width * df->height;
    printf("Average memory per tile: %zu bytes\n", with_entities_size / total_tiles);
    df_free(df);
    //int sizes[] = {5, 20, 50};
    int sizes[] = {DEFAULT_DUNGEON_FLOOR_WIDTH};
    int sizes_count = sizeof(sizes) / sizeof(sizes[0]);
    //for (int i = 0; i < 3; i++) {
    for (int i = 0; i < sizes_count; i++) {
        int size = sizes[i];
        dungeon_floor_t* df_test = df_create(0, size, size);
        size_t test_size = df_memory_size(df_test);
        printf("Memory size for %dx%d dungeon floor: %zu bytes\n", size, size, test_size);
        printf("Memory size for %dx%d dungeon floor: %zu kilobytes\n", size, size, test_size / 1024);
        printf("Memory size for %dx%d dungeon floor: %zu megabytes\n", size, size, test_size / 1024 / 1024);
        df_free(df_test);
    }
}

void test_dungeon_floors(void) {
    run_test_df_create_destroy();
    run_test_df_rooms();
    run_test_df_tiles();
    run_test_df_stairs();
    run_test_df_count_empty();
    run_test_df_count_walkable();
    run_test_df_init();
    run_test_df_serialization();
    run_test_df_memory_size();
    run_test_df_get_room_at();
}
