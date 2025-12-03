#pragma once

#include "dungeon_floor.h"
#include "dungeonfloorid.h"

#include <functional>
#include <memory>
#include <vector>

using std::function;
using std::shared_ptr;
using std::vector;

typedef struct {
    shared_ptr<vector<shared_ptr<dungeon_floor_t>>> floors; // vector of shared pointers to dungeon_floor_t
    int current_floor;
    bool is_locked;
} dungeon_t;


shared_ptr<dungeon_t> d_create();
shared_ptr<dungeon_floor_t> d_add_floor(shared_ptr<dungeon_t> dungeon, dungeon_floor_type_t type, int width, int height);
void d_destroy(shared_ptr<dungeon_t> dungeon);
void d_free(shared_ptr<dungeon_t> dungeon);


static inline shared_ptr<dungeon_floor_t> d_get_floor(shared_ptr<dungeon_t> dungeon, size_t index) {
    return !dungeon || index < 0 || index >= dungeon->floors->size() ? nullptr : dungeon->floors->at(index);
}

static inline shared_ptr<dungeon_floor_t> d_get_current_floor(shared_ptr<dungeon_t> dungeon) {
    return !dungeon ? nullptr : d_get_floor(dungeon, dungeon->current_floor);
}
