// dungeon.cpp
#include "dungeon.h"
#include "dungeon_floor.h"
#include "dungeon_tile_type.h"
#include "massert.h"
#include "mprint.h"
#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr;
using std::vector;

#define INITIAL_DUNGEON_CAPACITY 4

shared_ptr<dungeon_t> d_create() {
    shared_ptr<dungeon_t> dungeon = make_shared<dungeon_t>();
    if (!dungeon) {
        merror("Failed to allocate memory for dungeon");
        return nullptr;
    }
    dungeon->floors = make_shared<vector<shared_ptr<dungeon_floor_t>>>();
    dungeon->current_floor = 0;
    dungeon->is_locked = false;
    msuccess("Created new dungeon");
    return dungeon;
}

//void d_destroy(dungeon_t* const d) {
void d_destroy(shared_ptr<dungeon_t> d) {
    massert(d, "dungeon is NULL");
    if (d->floors) {
        // clear the floors vector
        d->floors->clear();
    }
    //free(d);
}

//void d_free(dungeon_t* const dungeon) {
void d_free(shared_ptr<dungeon_t> dungeon) {
    massert(dungeon, "dungeon is NULL");
    //minfo("Freeing dungeon with %d floors", dungeon->num_floors);
    d_destroy(dungeon);
}

bool d_add_floor(shared_ptr<dungeon_t> dungeon, int width, int height) {
    minfo("d_add_floor: Adding new floor with dimensions %dx%d", width, height);
    if (!dungeon) {
        merror("dungeon is NULL");
        return false;
    }
    if (width <= 0 || height <= 0) {
        merror("Invalid dimensions: width %d, height %d", width, height);
        return false;
    }
    if (dungeon->is_locked) {
        merror("Cannot add floor - dungeon is locked");
        return false;
    }
    int current_floor = dungeon->floors->size();
    shared_ptr<dungeon_floor_t> new_floor = df_create(current_floor, width, height);
    if (!new_floor) {
        merror("Failed to create new dungeon floor");
        return false;
    }
    // Initialize the new floor
    minfo("Initializing new floor with dimensions %dx%d", width, height);
    //df_init(new_floor);
    // Add a room
    int w = 20;
    int h = 20;
    // Get center of the floor
    int cx = width / 2;
    int cy = height / 2;
    minfo("Creating room at center (%d, %d) with size %dx%d", cx, cy, w, h);
    //df_set_tile_area_range(new_floor, cx, cy, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    // Assign upstairs and downstairs locations
    minfo("Assigning upstairs and downstairs in area (%d, %d, %d, %d)", 0, 0, width, height);
    df_assign_upstairs_in_area(new_floor, 0, 0, width, height);
    minfo("Assigning downstairs in area (%d, %d, %d, %d)", 0, 0, width, height);
    df_assign_downstairs_in_area(new_floor, 0, 0, width, height);
    // Add the new floor to the dungeon
    minfo("Adding new floor to dungeon");
    dungeon->floors->push_back(shared_ptr<dungeon_floor_t>(new_floor));
    msuccess("Added new floor %d to dungeon", current_floor);
    return true;
}
