#pragma once

#include "dungeon_floor.h"

#include "dungeon_floor.h"

static void run_test_df_create_destroy(void) {
    dungeon_floor_t* df = df_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    massert(df != NULL, "Failed to create dungeon floor");
    
    // Basic sanity checks
    massert(df->width == DEFAULT_DUNGEON_FLOOR_WIDTH, "Incorrect width");
    massert(df->height == DEFAULT_DUNGEON_FLOOR_HEIGHT, "Incorrect height");
    massert(df->tiles != NULL, "Tiles array not allocated");
    massert(df->rooms != NULL, "Rooms array not allocated");
    
    df_destroy(df);
    msuccess("df_create and df_destroy passed");
}

void test_dungeon_floors(void) {
    run_test_df_create_destroy();
}
