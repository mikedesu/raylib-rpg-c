#pragma once

#include "dungeon_floor.h"
#include "dungeonfloorid.h"

typedef struct {
    dungeon_floor_t** floors;
    int num_floors;
    int capacity_floors;
    int current_floor;
    bool is_locked;
} dungeon_t;

bool dungeon_add_floor(dungeon_t* dungeon, int width, int height);
dungeon_t* dungeon_create();
void dungeon_destroy(dungeon_t* dungeon);
void dungeon_free(dungeon_t* dungeon);

static inline dungeon_floor_t* dungeon_get_floor(dungeon_t* const dungeon, const int index) {
    massert(dungeon, "dungeon is NULL");
    massert(index >= 0, "index is negative");
    if (!dungeon || index < 0 || index >= dungeon->num_floors) {
        return NULL;
    }
    return dungeon->floors[index];
}

static inline dungeon_floor_t* dungeon_get_current_floor(dungeon_t* const dungeon) {
    massert(dungeon, "dungeon is NULL");
    if (!dungeon) {
        return NULL;
    }
    return dungeon_get_floor(dungeon, dungeon->current_floor);
}

static inline void dungeon_lock(dungeon_t* dungeon) {
    massert(dungeon, "dungeon is NULL");
    if (dungeon) {
        dungeon->is_locked = true;
    }
}

static inline void dungeon_unlock(dungeon_t* dungeon) {
    massert(dungeon, "dungeon is NULL");
    if (dungeon) {
        dungeon->is_locked = false;
    }
}

static inline size_t dungeon_num_floors(const dungeon_t* const dungeon) {
    massert(dungeon, "dungeon is NULL");
    return dungeon ? dungeon->num_floors : 0;
}
