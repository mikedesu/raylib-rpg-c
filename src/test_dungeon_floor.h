#pragma once

#include "dungeon_floor.h"


static void run_test_df_create_destroy(void) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    massert(df != NULL, "Failed to create dungeon floor");

    // Basic sanity checks
    massert(df->width == DEFAULT_DUNGEON_FLOOR_WIDTH, "Incorrect width");
    massert(df->height == DEFAULT_DUNGEON_FLOOR_HEIGHT, "Incorrect height");
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
    massert(df_loc_is_in_room(df, (loc_t){12, 12}) == 0, "Failed to locate room");
    massert(strcmp(df_get_room_name(df, (loc_t){12, 12}), "TestRoom1") == 0, "Room name incorrect");
    
    df_free(df);
    msuccess("df_room tests passed");
}

static void run_test_df_tiles(void) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    
    // Test tile access
    loc_t loc = {5, 5};
    massert(df_tile_at(df, loc) != NULL, "Failed to get tile");
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
    
    massert(df_get_upstairs(df).x != -1, "Upstairs not placed");
    massert(df_get_downstairs(df).x != -1, "Downstairs not placed");
    massert(df_get_upstairs(df).x != df_get_downstairs(df).x || 
            df_get_upstairs(df).y != df_get_downstairs(df).y, "Stairs in same location");
    
    df_free(df);
    msuccess("df_stair tests passed");
}

void test_dungeon_floors(void) {
    run_test_df_create_destroy();
    run_test_df_rooms();
    run_test_df_tiles();
    run_test_df_stairs();
}
