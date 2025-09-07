#pragma once

#include "dungeon_floor.h"
#include "dungeonfloorid.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

typedef struct {
    shared_ptr<vector<shared_ptr<dungeon_floor_t>>> floors; // vector of shared pointers to dungeon_floor_t
    int current_floor;
    bool is_locked;
} dungeon_t;


bool d_add_floor(shared_ptr<dungeon_t> dungeon, int width, int height);
shared_ptr<dungeon_t> d_create();
void d_destroy(shared_ptr<dungeon_t> dungeon);
void d_free(shared_ptr<dungeon_t> dungeon);


static inline shared_ptr<dungeon_floor_t> d_get_floor(shared_ptr<dungeon_t> dungeon, size_t index) {
    return !dungeon || index < 0 || index >= dungeon->floors->size() ? nullptr : dungeon->floors->at(index);
}

static inline shared_ptr<dungeon_floor_t> d_get_current_floor(shared_ptr<dungeon_t> dungeon) {
    return !dungeon ? nullptr : d_get_floor(dungeon, dungeon->current_floor);
}

static inline void d_lock(dungeon_t* dungeon) {
    if (dungeon) dungeon->is_locked = true;
}

static inline void d_unlock(dungeon_t* dungeon) {
    if (dungeon) dungeon->is_locked = false;
}

static inline size_t d_num_floors(const dungeon_t* const dungeon) { return !dungeon ? 0 : !dungeon->floors ? 0 : dungeon->floors->size(); }
