// dungeon.c
#include "dungeon.h"
#include "mprint.h"
#include <stdlib.h>

#define INITIAL_DUNGEON_CAPACITY 4

dungeon_t* dungeon_create() {
    dungeon_t* dungeon = malloc(sizeof(dungeon_t));
    if (!dungeon) {
        merror("dungeon_create: dungeon malloc failed");
        return NULL;
    }
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


void dungeon_destroy(dungeon_t* dungeon) {
    if (!dungeon) {
        merror("dungeon_destroy: dungeon is NULL");
        return;
    }
    for (int i = 0; i < dungeon->num_floors; i++) {
        dungeon_floor_free(dungeon->floors[i]);
    }
    free(dungeon->floors);
    free(dungeon);
}


const bool dungeon_add_floor(dungeon_t* const dungeon, const int width, const int height) {
    if (!dungeon || dungeon->is_locked) {
        merror("dungeon_add_floor: dungeon is NULL or locked");
        return false;
    }
    if (dungeon->num_floors >= dungeon->capacity_floors) {
        int new_capacity = dungeon->capacity_floors * 2;
        dungeon_floor_t** new_floors = realloc(dungeon->floors, sizeof(dungeon_floor_t*) * new_capacity);
        if (!new_floors) return false;
        dungeon->floors = new_floors;
        dungeon->capacity_floors = new_capacity;
    }
    dungeon_floor_t* new_floor = dungeon_floor_create(width, height);
    if (!new_floor) {
        merror("dungeon_add_floor: new_floor is NULL");
        return false;
    }
    dungeon->floors[dungeon->num_floors++] = new_floor;
    return true;
}


void dungeon_lock(dungeon_t* dungeon) {
    if (!dungeon) {
        merror("dungeon_lock: dungeon is NULL");
        return;
    }
    dungeon->is_locked = true;
}


void dungeon_unlock(dungeon_t* dungeon) {
    if (!dungeon) {
        merror("dungeon_unlock: dungeon is NULL");
        return;
    }
    dungeon->is_locked = false;
}


const int dungeon_num_floors(const dungeon_t* const dungeon) {
    if (!dungeon) {
        merror("dungeon_num_floors: dungeon is NULL");
        return -1;
    }
    return dungeon->num_floors;
}


dungeon_floor_t* dungeon_get_floor(dungeon_t* const dungeon, const int index) {
    // break this up into multiple if-elses with merror calls
    if (!dungeon) {
        merror("dungeon_get_floor: dungeon is NULL");
        return NULL;
    }
    if (index < 0) {
        merror("dungeon_get_floor: index is negative");
        return NULL;
    }
    if (index >= dungeon->num_floors) {
        merror("dungeon_get_floor: index is out of bounds");
        return NULL;
    }
    return dungeon->floors[index];
}


dungeon_floor_t* dungeon_get_current_floor(dungeon_t* const dungeon) {
    return dungeon_get_floor(dungeon, dungeon->current_floor);
}
