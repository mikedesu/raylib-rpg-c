#pragma once

#include "dungeon.h"
#include "unit_test.h"

void test_dungeon(void);

TEST(test_dungeon_create_destroy) {
    // Create a new dungeon
    dungeon_t* dungeon = d_create();
    ASSERT(dungeon != NULL, "Failed to create dungeon");

    // Check initial properties
    ASSERT(dungeon->num_floors == 0, "Initial number of floors should be 0");
    ASSERT(dungeon->current_floor == 0, "Initial current floor should be 0");
    ASSERT(dungeon->is_locked == false, "Dungeon should not be locked initially");

    // Destroy the dungeon
    d_destroy(dungeon);
}

void test_dungeon(void) { run_test_dungeon_create_destroy(); }
