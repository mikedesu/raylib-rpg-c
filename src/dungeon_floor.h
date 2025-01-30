#pragma once

#include "dungeon_tile.h"

#define DEFAULT_DUNGEON_FLOOR_WIDTH 8
#define DEFAULT_DUNGEON_FLOOR_HEIGHT 8

typedef struct {

    dungeon_tile_t tiles[DEFAULT_DUNGEON_FLOOR_WIDTH][DEFAULT_DUNGEON_FLOOR_HEIGHT];
    int width;
    int height;

} dungeon_floor_t;



void dungeon_floor_init(dungeon_floor_t* floor);
