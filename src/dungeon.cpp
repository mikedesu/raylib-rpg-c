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
    dungeon->floors = make_shared<vector<shared_ptr<dungeon_floor_t>>>(INITIAL_DUNGEON_CAPACITY);
    dungeon->current_floor = 0;
    dungeon->is_locked = false;
    msuccess("Created new dungeon with capacity %d", INITIAL_DUNGEON_CAPACITY);
    return dungeon;
}

void d_destroy(dungeon_t* const d) {
    massert(d, "dungeon is NULL");
    if (d->floors) {
        // clear the floors vector
        d->floors->clear();
    }
    free(d);
}

void d_free(dungeon_t* const dungeon) {
    massert(dungeon, "dungeon is NULL");
    minfo("Freeing dungeon with %d floors", dungeon->num_floors);
    d_destroy(dungeon);
}

bool d_add_floor(dungeon_t* const dungeon, int width, int height) {
    if (!dungeon) return false;
    if (width <= 0 || height <= 0) return false;
    if (dungeon->is_locked) return false;
    int current_floor = dungeon->floors->size();
    dungeon_floor_t* new_floor = df_create(current_floor, width, height);
    if (!new_floor) {
        return false;
    }
    // Initialize the new floor
    df_init(new_floor);
    // Add a room
    int w = 20;
    int h = 20;
    // Get center of the floor
    int cx = width / 2;
    int cy = height / 2;
    df_set_tile_area_range(new_floor, cx, cy, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    // Assign upstairs and downstairs locations
    df_assign_upstairs_in_area(new_floor, 0, 0, width, height);
    df_assign_downstairs_in_area(new_floor, 0, 0, width, height);
    // Add the new floor to the dungeon
    dungeon->floors->push_back(shared_ptr<dungeon_floor_t>(new_floor));
    return true;
}
