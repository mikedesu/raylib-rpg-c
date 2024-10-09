#pragma once

#include "vectorentityid.h"
#include <raylib.h>
#include <stdlib.h>

typedef enum {
    TILETYPE_NONE = 0,
    TILETYPE_VOID = 1,
    TILETYPE_DIRT_00 = 2,
    TILETYPE_DIRT_01 = 3,
    TILETYPE_DIRT_02 = 4,
    TILETYPE_STONE_00 = 5,
    TILETYPE_STONE_01 = 6,
    TILETYPE_STONE_02 = 7,
    TILETYPE_STONE_03 = 8,
    TILETYPE_STONE_04 = 9,
    TILETYPE_STONE_05 = 10,
    TILETYPE_STONE_06 = 11,
    TILETYPE_STONE_07 = 12,
    TILETYPE_STONE_08 = 13,
    TILETYPE_STONE_09 = 14,
    TILETYPE_STONE_10 = 15,
    TILETYPE_STONE_11 = 16,
    TILETYPE_STONE_12 = 17,
    TILETYPE_STONE_13 = 18,
    TILETYPE_STONE_14 = 19,
    TILETYPE_STONE_WALL_00 = 20,
    TILETYPE_COUNT

} tiletype_t;


typedef struct tile_t {
    tiletype_t type;
    vectorentityid_t entityids;
    //Vector2 pos; // may not need this
} tile_t;


typedef struct dungeonfloor_t {
    int len;
    int wid;
    tile_t* grid;

} dungeonfloor_t;

dungeonfloor_t* create_dungeonfloor(int len, int wid, tiletype_t basetype);
void dungeonfloor_free(dungeonfloor_t* d);
void dungeonfloor_set_all_tiles_to_type(dungeonfloor_t* d, tiletype_t type);
void dungeonfloor_init_all_tiles_entityids(dungeonfloor_t* d);

void dungeonfloor_set_tiletype_at(dungeonfloor_t* d, tiletype_t type, int x, int y);
void dungeonfloor_set_tiletype_at_unsafe(dungeonfloor_t* d, tiletype_t type, int x, int y);

//tile_t* dungeonfloor_get_tile(dungeonfloor_t* d, Vector2 pos);
tile_t* dungeonfloor_get_tile(dungeonfloor_t* d, int x, int y);
