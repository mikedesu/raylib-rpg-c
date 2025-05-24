#pragma once

#include "dungeon_floor.h"
#include "unit_test.h"
#include <string.h>

TEST(test_df_create_destroy) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
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
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    printf("Initial room count: %d\n", df->room_count);

    // Test room creation
    bool add_result = df_add_room_info(df, 10, 10, 5, 5, "TestRoom1");
    //printf("Add room result: %d\n", add_result);
    //printf("Room count after add: %d\n", df->room_count);

    if (add_result) {
        // Test room location
        int in_room = df_loc_is_in_room(df, (loc_t){12, 12});
        printf("Location in room result: %d\n", in_room);

        const char* room_name = df_get_room_name(df, (loc_t){12, 12});
        printf("Room name at location: %s\n", room_name ? room_name : "NULL");
    }

    // Test overlapping rooms - should fail
    bool overlap_result = df_add_room_info(df, 10, 10, 5, 5, "OverlapRoom");
    //printf("Add overlapping room result: %d\n", overlap_result);
    //printf("Room count after overlap attempt: %d\n", df->room_count);
    ASSERT(!overlap_result, "Should not be able to add overlapping room");
    ASSERT(df->room_count == 2, "Room count should not increase after failed overlap");

    // Test adjacent rooms (should be allowed)
    bool adjacent_result = df_add_room_info(df, 15, 15, 5, 5, "AdjacentRoom");
    //printf("Add adjacent room result: %d\n", adjacent_result);
    //printf("Room count after adjacent add: %d\n", df->room_count);
    ASSERT(adjacent_result, "Should be able to add adjacent room");

    df_free(df);
    //printf("df_room tests completed\n");
}

TEST(test_df_tiles) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);

    // Test tile access
    loc_t loc = {5, 5};
    ASSERT(df_tile_at(df, loc) != NULL, "Failed to get tile");
    df_set_tile(df, TILE_STONE_WALL_00, loc.x, loc.y);
    ASSERT(df_type_at(df, loc.x, loc.y) == TILE_STONE_WALL_00, "Tile type not set");

    df_free(df);
    //printf("df_tile tests passed\n");
}

TEST(test_df_stairs) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);

    // Create a room to place stairs in
    df_add_room_info(df, 10, 10, 5, 5, "StairRoom");

    // Test stair placement
    df_assign_upstairs_in_area(df, 10, 10, 5, 5);
    df_assign_downstairs_in_area(df, 10, 10, 5, 5);

    ASSERT(df_get_upstairs(df).x != -1, "Upstairs not placed");
    ASSERT(df_get_downstairs(df).x != -1, "Downstairs not placed");
    ASSERT(df_get_upstairs(df).x != df_get_downstairs(df).x || df_get_upstairs(df).y != df_get_downstairs(df).y, "Stairs in same location");

    df_free(df);
    //printf("df_stair tests passed\n");
}

TEST(test_df_get_room_at) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);

    // Create a test room
    int room_x = 10, room_y = 10, room_w = 5, room_h = 5;
    const char* room_name = "TestRoom";
    ASSERT(df_add_room_info(df, room_x, room_y, room_w, room_h, room_name), "Failed to add room");

    // Test points inside the room
    for (int y = room_y; y < room_y + room_h; y++) {
        for (int x = room_x; x < room_x + room_w; x++) {
            const room_data_t* room = df_get_room_at(df, x, y);
            ASSERT(room != NULL, "Should find room for point inside room");
            ASSERT(strcmp(room->room_name, room_name) == 0, "Room name mismatch");
            ASSERT(room->x == room_x && room->y == room_y, "Room coordinates mismatch");
            ASSERT(room->w == room_w && room->h == room_h, "Room dimensions mismatch");
        }
    }

    // Test points outside the room
    ASSERT(df_get_room_at(df, room_x - 1, room_y) == NULL, "Should not find room for point left of room");
    ASSERT(df_get_room_at(df, room_x + room_w, room_y) == NULL, "Should not find room for point right of room");
    ASSERT(df_get_room_at(df, room_x, room_y - 1) == NULL, "Should not find room for point above room");
    ASSERT(df_get_room_at(df, room_x, room_y + room_h) == NULL, "Should not find room for point below room");

    df_free(df);
    //printf("df_get_room_at tests passed\n");
}

TEST(test_df_count_walkable) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);

    // Create a test room
    int room_x = 10, room_y = 10, room_w = 5, room_h = 5;
    ASSERT(df_add_room_info(df, room_x, room_y, room_w, room_h, "TestRoom"), "Failed to add room");

    // Count walkable tiles in the room
    int expected_walkable = room_w * room_h;
    int actual_walkable = df_count_walkable(df);
    //printf("Expected walkable tiles: %d, Actual walkable tiles: %d\n", expected_walkable, actual_walkable);
    ASSERT(actual_walkable >= expected_walkable, "Too few walkable tiles");

    // Add some walls and verify count decreases
    df_set_tile(df, TILE_STONE_WALL_00, room_x, room_y);
    df_set_tile(df, TILE_STONE_WALL_00, room_x + 1, room_y);
    int new_walkable = df_count_walkable(df);
    ASSERT(new_walkable >= actual_walkable - 2, "Walkable count should decrease after adding walls");

    df_free(df);
    //printf("df_count_walkable tests passed\n");
}

TEST(test_df_init) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);

    // Initialize the dungeon floor
    df_init(df);

    // Test basic initialization
    ASSERT(df->tiles != NULL, "Tiles not initialized");
    ASSERT(df->rooms != NULL, "Rooms not initialized");
    ASSERT(df->room_count > 0, "No rooms created");

    // Test tile initialization
    int walkable_count = df_count_walkable(df);
    ASSERT(walkable_count > 0, "No walkable tiles created");

    // Test room initialization
    int room_count = 0;
    room_data_t* rooms = df_get_rooms_with_prefix(df, &room_count, "room");
    ASSERT(room_count > 0, "No rooms created");

    // Test stair placement
    loc_t upstairs = df_get_upstairs(df);
    loc_t downstairs = df_get_downstairs(df);
    ASSERT(upstairs.x != -1 && upstairs.y != -1, "Upstairs not placed");
    ASSERT(downstairs.x != -1 && downstairs.y != -1, "Downstairs not placed");
    ASSERT(!(upstairs.x == downstairs.x && upstairs.y == downstairs.y), "Stairs in same location");

    // Test tile types
    tile_t* up_tile = df_tile_at(df, upstairs);
    tile_t* down_tile = df_tile_at(df, downstairs);
    ASSERT(up_tile->type == TILE_UPSTAIRS, "Upstairs tile type incorrect");
    ASSERT(down_tile->type == TILE_DOWNSTAIRS, "Downstairs tile type incorrect");

    df_free(df);
    //printf("df_init tests passed\n");
}

TEST(test_df_serialization) {
    // Create and initialize a dungeon floor
    dungeon_floor_t* df = df_create(20, 20); // Smaller size for testing
    df_init(df);

    // Add a test room
    df_add_room_info(df, 5, 5, 3, 3, "TestRoom");

    // Get serialized size
    size_t size = df_serialized_size(df);
    ASSERT(size > 0, "Serialized size should be greater than 0");

    // Allocate buffer for serialization
    char* buffer = (char*)malloc(size);
    ASSERT(buffer != NULL, "Failed to allocate serialization buffer");

    // Serialize
    size_t written = df_serialize(df, buffer, size);
    ASSERT(written > 0, "Serialization failed");
    ASSERT(written == size, "Serialized size mismatch");

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

    // Verify room data
    ASSERT(df2->rooms != NULL, "Rooms not deserialized");
    ASSERT(df2->room_count > 0, "No rooms deserialized");
    ASSERT(strcmp(df2->rooms[0].room_name, "TestRoom") == 0, "Room name mismatch");
    ASSERT(df2->rooms[0].x == 5 && df2->rooms[0].y == 5, "Room position mismatch");
    ASSERT(df2->rooms[0].w == 3 && df2->rooms[0].h == 3, "Room size mismatch");

    // Verify stair locations
    ASSERT(df2->upstairs_loc.x == df->upstairs_loc.x, "Upstairs X mismatch");
    ASSERT(df2->upstairs_loc.y == df->upstairs_loc.y, "Upstairs Y mismatch");
    ASSERT(df2->downstairs_loc.x == df->downstairs_loc.x, "Downstairs X mismatch");
    ASSERT(df2->downstairs_loc.y == df->downstairs_loc.y, "Downstairs Y mismatch");

    // Verify tile types at stair locations
    tile_t* up_tile = df_tile_at(df2, df2->upstairs_loc);
    tile_t* down_tile = df_tile_at(df2, df2->downstairs_loc);
    ASSERT(up_tile->type == TILE_UPSTAIRS, "Upstairs tile type incorrect after deserialization");
    ASSERT(down_tile->type == TILE_DOWNSTAIRS, "Downstairs tile type incorrect after deserialization");

    // Clean up
    free(buffer);
    df_free(df);
    df_free(df2);
}

void test_dungeon_floors(void) {
    run_test_df_create_destroy();
    run_test_df_rooms();
    run_test_df_tiles();
    run_test_df_stairs();
    run_test_df_init();
    run_test_df_get_room_at();
    run_test_df_count_walkable();
    run_test_df_serialization();
}
