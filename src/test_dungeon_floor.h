#pragma once

#include "dungeon_floor.h"

static void run_test_df_create_destroy(void) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    massert(df != NULL, "Failed to create dungeon floor");

    // Basic sanity checks
    massert(df->width == DEFAULT_DUNGEON_FLOOR_WIDTH, "Incorrect width");
    massert(df->height == DEFAULT_DUNGEON_FLOOR_HEngth, "Incorrect height");
    massert(df->tiles != NULL, "Tiles array not allocated");
    massert(df->rooms != NULL, "Rooms array not allocated");

    df_free(df);
    msuccess("df_create and df_destroy passed");
}

static void run_test_df_rooms(void) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    
    // Test room creation
    massert(df_add_room_info(df, 10, 10, 5, 5, "TestRoom1"), "Failed to add room");
    massert(df->room_count == 1, "Room count incorrect");
    
    // Test room location
    loc_t loc = {12, 12};
    int room_idx = df_loc_is_in_room(df, loc);
    massert(room_idx == 0, "Failed to locate room");
    
    // Test room name
    const char* name = df_get_room_name(df, loc);
    massert(name != NULL && strcmp(name, "TestRoom1") == 0, "Room name incorrect");
    
    df_free(df);
    msuccess("df_room tests passed");
}

static void run_test_df_tiles(void) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    
    // Test tile access
    loc_t loc = {5, 5};
    tile_t* tile = df_tile_at(df, loc);
    massert(tile != NULL, "Failed to get tile");
    
    // Test tile modification
    df_set_tile(df, TILE_STONE_WALL_00, loc.x, loc.y);
    massert(df_type_at(df, loc.x, loc.y) == TILE_STONE_WALL_00, "Tile type not set");
    
    df_free(df);
    msuccess("df_tile tests passed");
}

static void run_test_df_stairs(void) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    
    // Create a room to place stairs in
    df_add_room_info(df, 10, 10, 5, 5, "StairRoom");
    
    // Test stair placement
    df_assign_upstairs_in_area(df, 10, 10, 5, 5);
    df_assign_downstairs_in_area(df, 10, 10, 5, 5);
    
    loc_t up = df_get_upstairs(df);
    loc_t down = df_get_downstairs(df);
    massert(up.x != -1 && up.y != -1, "Upstairs not placed");
    massert(down.x != -1 && down.y != -1, "Downstairs not placed");
    massert(up.x != down.x || up.y != down.y, "Stairs in same location");
    
    df_free(df);
    msuccess("df_stair tests passed");
}

void test_dungeon_floors(void) {
    run_test_df_create_destroy();
    run_test_df_rooms();
    run_test_df_tiles();
    run_test_df_stairs();
}
