#pragma once

#include "dungeon_tile.h"
#include "entityid.h"
#include <stdbool.h>

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

const bool dungeon_floor_add_at(dungeon_floor_t* const df, const entityid id, const int x, const int y);
