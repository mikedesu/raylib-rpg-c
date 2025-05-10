// dungeon.c
#include "dungeon.h"
//#include "massert.h"
#include "mprint.h"
#include <stdlib.h>

#define INITIAL_DUNGEON_CAPACITY 4

dungeon_t* d_create() {
    dungeon_t* dungeon = malloc(sizeof(dungeon_t));
    if (!dungeon) {
        merror("Failed to allocate memory for dungeon");
        return NULL;
    }

    dungeon->floors = malloc(sizeof(dungeon_floor_t*) * INITIAL_DUNGEON_CAPACITY);
    if (!dungeon->floors) {
        merror("Failed to allocate memory for dungeon floors");
        free(dungeon);
        return NULL;
    }

    dungeon->current_floor = 0;
    dungeon->num_floors = 0;
    dungeon->capacity_floors = INITIAL_DUNGEON_CAPACITY;
    dungeon->is_locked = false;
    msuccess("Created new dungeon with capacity %d", INITIAL_DUNGEON_CAPACITY);
    return dungeon;
}

void d_destroy(dungeon_t* const d) {
    if (!d) { return; }

    if (d->floors) {
        for (int i = 0; i < d->num_floors; i++) {
            if (d->floors[i]) { df_free(d->floors[i]); }
        }
        free(d->floors);
    }
    free(d);
}

void d_free(dungeon_t* const dungeon) {
    if (!dungeon) {
        merror("Cannot free - dungeon is NULL");
        return;
    }
    minfo("Freeing dungeon with %d floors", dungeon->num_floors);
    d_destroy(dungeon);
}

bool d_add_floor(dungeon_t* const dungeon, int width, int height) {
    if (!dungeon) {
        merror("dungeon is NULL");
        return false;
    }
    if (width <= 0 || height <= 0) {
        merror("Invalid floor dimensions %dx%d", width, height);
        return false;
    }
    if (dungeon->is_locked) {
        merror("dungeon is locked");
        return false;
    }
    if (dungeon->num_floors >= dungeon->capacity_floors) {
        int new_capacity = dungeon->capacity_floors * 2;
        dungeon_floor_t** new_floors = realloc(dungeon->floors, sizeof(dungeon_floor_t*) * new_capacity);
        if (!new_floors) {
            merror("Failed to reallocate floors array to capacity %d", new_capacity);
            return false;
        }
        dungeon->floors = new_floors;
        dungeon->capacity_floors = new_capacity;
        minfo("Expanded dungeon capacity to %d", new_capacity);
    }
    dungeon_floor_t* new_floor = df_create(width, height);
    if (!new_floor) {
        merror("Failed to create new floor");
        return false;
    }
    dungeon->floors[dungeon->num_floors++] = new_floor;
    msuccess("Added new floor %dx%d (total floors: %d)", width, height, dungeon->num_floors);
    return true;
}
