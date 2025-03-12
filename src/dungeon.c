// dungeon.c
#include "dungeon.h"
#include <stdlib.h>

#define INITIAL_DUNGEON_CAPACITY 4

dungeon_t* dungeon_create() {
    dungeon_t* dungeon = malloc(sizeof(dungeon_t));
    if (!dungeon) return NULL;
    dungeon->floors = malloc(sizeof(dungeon_floor_t*) * INITIAL_DUNGEON_CAPACITY);
    if (!dungeon->floors) {
        free(dungeon);
        return NULL;
    }
    dungeon->current_floor = 0;
    dungeon->num_floors = 0;
    dungeon->capacity_floors = INITIAL_DUNGEON_CAPACITY;
    dungeon->is_locked = false;
    return dungeon;
}


void dungeon_destroy(dungeon_t* dungeon) {
    if (!dungeon) return;
    for (int i = 0; i < dungeon->num_floors; i++) {
        dungeon_floor_free(dungeon->floors[i]);
    }
    free(dungeon->floors);
    free(dungeon);
}


const bool dungeon_add_floor(dungeon_t* dungeon, int width, int height) {
    if (!dungeon || dungeon->is_locked) return false;
    if (dungeon->num_floors >= dungeon->capacity_floors) {
        int new_capacity = dungeon->capacity_floors * 2;
        dungeon_floor_t** new_floors = realloc(dungeon->floors, sizeof(dungeon_floor_t*) * new_capacity);
        if (!new_floors) return false;
        dungeon->floors = new_floors;
        dungeon->capacity_floors = new_capacity;
    }
    dungeon_floor_t* new_floor = dungeon_floor_create(width, height);
    if (!new_floor) return false;
    dungeon->floors[dungeon->num_floors++] = new_floor;
    return true;
}


void dungeon_lock(dungeon_t* dungeon) {
    if (dungeon) dungeon->is_locked = true;
}


void dungeon_unlock(dungeon_t* dungeon) {
    if (dungeon) dungeon->is_locked = false;
}


const int dungeon_num_floors(const dungeon_t* const dungeon) {
    if (!dungeon) return -1;
    return dungeon->num_floors;
}


dungeon_floor_t* dungeon_get_floor(dungeon_t* const dungeon, const int index) {
    if (!dungeon || index < 0 || index >= dungeon->num_floors) return NULL;
    return dungeon->floors[index];
}


dungeon_floor_t* dungeon_get_current_floor(dungeon_t* const dungeon) {
    return dungeon_get_floor(dungeon, dungeon->current_floor);
}
