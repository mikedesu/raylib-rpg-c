#pragma once

#include "dungeon_floor.h"
#include "dungeonfloorid.h"

#include <memory>
#include <vector>

typedef struct {
    std::shared_ptr<std::vector<std::shared_ptr<dungeon_floor_t>>> floors; // vector of shared pointers to dungeon_floor_t
    int current_floor;
    bool is_locked;
} dungeon_t;

bool d_add_floor(std::shared_ptr<dungeon_t> dungeon, int width, int height);
std::shared_ptr<dungeon_t> d_create();
void d_destroy(std::shared_ptr<dungeon_t> dungeon);
void d_free(std::shared_ptr<dungeon_t> dungeon);

static inline std::shared_ptr<dungeon_floor_t> d_get_floor(std::shared_ptr<dungeon_t> dungeon, int index) {
    massert(dungeon, "dungeon is NULL");
    massert(index >= 0, "index is negative");
    if (!dungeon) {
        merror("dungeon is NULL");
        return NULL;
    }
    if (index < 0 || index >= dungeon->floors->size()) {
        //merror("index %d is out of bounds (num_floors: %d)", index, dungeon->num_floors);
        return NULL;
    }
    return dungeon->floors->at(index);
}

static inline std::shared_ptr<dungeon_floor_t> d_get_current_floor(std::shared_ptr<dungeon_t> dungeon) {
    massert(dungeon, "dungeon is NULL");
    if (!dungeon) {
        merror("dungeon is NULL");
        return NULL;
    }
    std::shared_ptr<dungeon_floor_t> floor = d_get_floor(dungeon, dungeon->current_floor);
    if (!floor) {
        merror("Failed to get current floor %d", dungeon->current_floor);
    }
    return floor;
}

static inline void d_lock(dungeon_t* dungeon) {
    massert(dungeon, "dungeon is NULL");
    if (dungeon) {
        dungeon->is_locked = true;
        minfo("Locked dungeon");
    } else {
        merror("Cannot lock - dungeon is NULL");
    }
}

static inline void d_unlock(dungeon_t* dungeon) {
    massert(dungeon, "dungeon is NULL");
    if (dungeon) {
        dungeon->is_locked = false;
        minfo("Unlocked dungeon");
    } else {
        merror("Cannot unlock - dungeon is NULL");
    }
}

static inline size_t d_num_floors(const dungeon_t* const dungeon) {
    massert(dungeon, "dungeon is NULL");
    if (!dungeon) {
        merror("dungeon is NULL");
        return 0;
    }
    if (!dungeon->floors) {
        merror("floors vector is NULL");
        return 0;
    }
    return dungeon->floors->size();
}
