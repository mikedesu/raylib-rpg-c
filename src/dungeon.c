// dungeon.c
#include "dungeon.h"
#include "massert.h"
#include "mprint.h"
#include <stdlib.h>

#define INITIAL_DUNGEON_CAPACITY 4

dungeon_t* dungeon_create() {
    dungeon_t* dungeon = malloc(sizeof(dungeon_t));
    massert(dungeon, "dungeon_create: dungeon malloc failed");
    dungeon->floors = malloc(sizeof(dungeon_floor_t*) * INITIAL_DUNGEON_CAPACITY);
    if (!dungeon->floors) {
        merror("dungeon_create: dungeon->floors malloc failed");
        free(dungeon);
        return NULL;
    }
    dungeon->current_floor = dungeon->num_floors = 0;
    dungeon->capacity_floors = INITIAL_DUNGEON_CAPACITY;
    dungeon->is_locked = false;
    return dungeon;
}

void dungeon_destroy(dungeon_t* d) {
    massert(d, "dungeon_destroy: dungeon is NULL");
    //for (int i = 0; i < dungeon->num_floors; i++) { dungeon_floor_free(dungeon->floors[i]); }
    if (d->floors) {
        for (int i = 0; i < d->num_floors; i++) df_free(d->floors[i]);
        free(d->floors);
    }
    free(d);
}

bool dungeon_add_floor(dungeon_t* const dungeon, int width, int height) {
    massert(dungeon, "dungeon_add_floor: dungeon is NULL");
    massert(!dungeon->is_locked, "dungeon_add_floor: dungeon is locked");
    massert(width > 0, "dungeon_add_floor: width is less than 1");
    massert(height > 0, "dungeon_add_floor: height is less than 1");
    if (dungeon->num_floors >= dungeon->capacity_floors) {
        int new_capacity = dungeon->capacity_floors * 2;
        dungeon_floor_t** new_floors = realloc(dungeon->floors, sizeof(dungeon_floor_t*) * new_capacity);
        if (!new_floors) return false;
        dungeon->floors = new_floors;
        dungeon->capacity_floors = new_capacity;
    }
    dungeon_floor_t* new_floor = df_create(width, height);
    massert(new_floor, "dungeon_add_floor: failed to create new floor");
    if (!new_floor) {
        merror("dungeon_add_floor: new_floor is NULL");
        return false;
    }
    dungeon->floors[dungeon->num_floors++] = new_floor;
    return true;
}

void dungeon_lock(dungeon_t* dungeon) {
    massert(dungeon, "dungeon_lock: dungeon is NULL");
    dungeon->is_locked = true;
}

void dungeon_unlock(dungeon_t* dungeon) {
    massert(dungeon, "dungeon_unlock: dungeon is NULL");
    dungeon->is_locked = false;
}

int dungeon_num_floors(const dungeon_t* const dungeon) {
    massert(dungeon, "dungeon_num_floors: dungeon is NULL");
    return dungeon->num_floors;
}

dungeon_floor_t* dungeon_get_floor(dungeon_t* const dungeon, const int index) {
    massert(dungeon, "dungeon_get_floor: dungeon is NULL");
    massert(index >= 0, "dungeon_get_floor: index is negative");
    massert(index < dungeon->num_floors, "dungeon_get_floor: index is out of bounds");
    return dungeon->floors[index];
}

dungeon_floor_t* dungeon_get_current_floor(dungeon_t* const dungeon) {
    return dungeon_get_floor(dungeon, dungeon->current_floor);
}
