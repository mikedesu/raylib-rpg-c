// dungeon.h
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

int dungeon_num_floors(const dungeon_t* const dungeon);

dungeon_t* dungeon_create();

dungeon_floor_t* dungeon_get_floor(dungeon_t* const dungeon, const int index);
dungeon_floor_t* dungeon_get_current_floor(dungeon_t* const dungeon);

void dungeon_destroy(dungeon_t* dungeon);
void dungeon_lock(dungeon_t* dungeon);
void dungeon_unlock(dungeon_t* dungeon);
