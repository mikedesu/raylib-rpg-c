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

TEST(test_dungeon_serialization) {
    // Create a dungeon with a couple of small floors
    dungeon_t* d = d_create();
    ASSERT(d != NULL, "Failed to create dungeon");
    
    // Add two small floors
    ASSERT(d_add_floor(d, 3, 3), "Failed to add first floor");
    ASSERT(d_add_floor(d, 4, 4), "Failed to add second floor");
    
    // Initialize the floors with some basic content
    for (int f = 0; f < d->num_floors; f++) {
        dungeon_floor_t* df = d->floors[f];
        
        // Set some tiles
        for (int y = 0; y < df->height; y++) {
            for (int x = 0; x < df->width; x++) {
                tile_init(&df->tiles[y][x], TILE_FLOOR_STONE_00);
            }
        }
        
        // Add a room
        df_add_room_info(df, 0, 0, df->width, df->height, "TestRoom");
        
        // Set stairs
        df->upstairs_loc = (loc_t){0, 0};
        df->downstairs_loc = (loc_t){df->width-1, df->height-1};
        tile_init(&df->tiles[0][0], TILE_UPSTAIRS);
        tile_init(&df->tiles[df->height-1][df->width-1], TILE_DOWNSTAIRS);
    }
    
    // Get serialized size
    size_t size = d_serialized_size(d);
    printf("Serialized size for dungeon: %zu bytes\n", size);
    ASSERT(size > 0, "Serialized size should be greater than 0");
    
    // Allocate buffer for serialization
    char* buffer = (char*)malloc(size);
    ASSERT(buffer != NULL, "Failed to allocate serialization buffer");
    
    // Serialize
    size_t written = d_serialize(d, buffer, size);
    ASSERT(written > 0, "Serialization failed");
    ASSERT(written == size, "Serialized size mismatch");
    
    // Create a new dungeon for deserialization
    dungeon_t* d2 = (dungeon_t*)malloc(sizeof(dungeon_t));
    ASSERT(d2 != NULL, "Failed to allocate new dungeon");
    
    // Deserialize
    bool success = d_deserialize(d2, buffer, size);
    ASSERT(success, "Deserialization failed");
    
    // Verify basic properties
    ASSERT(d2->num_floors == d->num_floors, "Floor count mismatch");
    ASSERT(d2->current_floor == d->current_floor, "Current floor mismatch");
    
    // Verify each floor
    for (int f = 0; f < d2->num_floors; f++) {
        dungeon_floor_t* df1 = d->floors[f];
        dungeon_floor_t* df2 = d2->floors[f];
        
        ASSERT(df2->width == df1->width, "Floor width mismatch");
        ASSERT(df2->height == df1->height, "Floor height mismatch");
        ASSERT(df2->room_count == df1->room_count, "Room count mismatch");
        
        // Verify stair locations
        ASSERT(df2->upstairs_loc.x == df1->upstairs_loc.x, "Upstairs X mismatch");
        ASSERT(df2->upstairs_loc.y == df1->upstairs_loc.y, "Upstairs Y mismatch");
        ASSERT(df2->downstairs_loc.x == df1->downstairs_loc.x, "Downstairs X mismatch");
        ASSERT(df2->downstairs_loc.y == df1->downstairs_loc.y, "Downstairs Y mismatch");
    }
    
    // Test memory size calculation
    size_t memory_size = d_memory_size(d);
    printf("Dungeon memory size: %zu bytes\n", memory_size);
    ASSERT(memory_size > 0, "Memory size should be greater than 0");
    
    // Clean up
    free(buffer);
    d_free(d);
    d_free(d2);
}

void test_dungeon(void) { 
    run_test_dungeon_create_destroy(); 
    run_test_dungeon_serialization();
}
