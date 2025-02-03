#pragma once

#include "dungeon_tile.h"

#define DEFAULT_DUNGEON_FLOOR_WIDTH 8
#define DEFAULT_DUNGEON_FLOOR_HEIGHT 8

typedef struct {

    //dungeon_tile_t tiles[DEFAULT_DUNGEON_FLOOR_WIDTH][DEFAULT_DUNGEON_FLOOR_HEIGHT];
    dungeon_tile_t** tiles;
    size_t width;
    size_t height;

} dungeon_floor_t;


dungeon_floor_t* dungeon_floor_create(int width, int height);

void dungeon_floor_init(dungeon_floor_t* floor);

void dungeon_floor_free(dungeon_floor_t* floor);
