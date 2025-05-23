#pragma once

#include "dungeon_floor.h"
#include "unit_test.h"
#include <string.h>

static void run_test_df_create_destroy(void) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    ASSERT(df != NULL, "Failed to create dungeon floor");

    // Basic sanity checks
    ASSERT(df->width == DEFAULT_DUNGEON_FLOOR_WIDTH, "Incorrect width");
    ASSERT(df->height == DEFAULT_DUNGEON_FLOOR_HEIGHT, "Incorrect height");
    ASSERT(df->tiles != NULL, "Tiles array not allocated");
    ASSERT(df->rooms != NULL, "Rooms array not allocated");

    df_free(df);
    printf("df_create and df_destroy passed\n");
}

static void run_test_df_rooms(void) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    printf("Initial room count: %d\n", df->room_count);

    // Test room creation
    bool add_result = df_add_room_info(df, 10, 10, 5, 5, "TestRoom1");
    printf("Add room result: %d\n", add_result);
    printf("Room count after add: %d\n", df->room_count);

    if (add_result) {
        // Test room location
        int in_room = df_loc_is_in_room(df, (loc_t){12, 12});
        printf("Location in room result: %d\n", in_room);

        const char* room_name = df_get_room_name(df, (loc_t){12, 12});
        printf("Room name at location: %s\n", room_name ? room_name : "NULL");
    }

    // Test overlapping rooms
    bool overlap_result = df_add_room_info(df, 12, 12, 5, 5, "OverlapRoom");
    printf("Add overlapping room result: %d\n", overlap_result);
    printf("Room count after overlap attempt: %d\n", df->room_count);
    ASSERT(!overlap_result, "Should not be able to add overlapping room");

    // Test adjacent rooms (should be allowed)
    bool adjacent_result = df_add_room_info(df, 15, 15, 5, 5, "AdjacentRoom");
    printf("Add adjacent room result: %d\n", adjacent_result);
    printf("Room count after adjacent add: %d\n", df->room_count);
    ASSERT(adjacent_result, "Should be able to add adjacent room");

    df_free(df);
    printf("df_room tests completed\n");
}

static void run_test_df_tiles(void) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);

    // Test tile access
    loc_t loc = {5, 5};
    ASSERT(df_tile_at(df, loc) != NULL, "Failed to get tile");
    df_set_tile(df, TILE_STONE_WALL_00, loc.x, loc.y);
    ASSERT(df_type_at(df, loc.x, loc.y) == TILE_STONE_WALL_00, "Tile type not set");

    df_free(df);
    printf("df_tile tests passed\n");
}

static void run_test_df_stairs(void) {
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
    printf("df_stair tests passed\n");
}

static void run_test_df_init(void) {
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
    printf("df_init tests passed\n");
}

void test_dungeon_floors(void) {
    run_test_df_create_destroy();
    run_test_df_rooms();
    run_test_df_tiles();
    run_test_df_stairs();
    run_test_df_init();
}
