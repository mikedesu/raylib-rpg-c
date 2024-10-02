#pragma once

#include "vectorentityid.h"
#include <raylib.h>
#include <stdlib.h>

typedef enum {
    TILETYPE_NONE,
    TILETYPE_VOID,
    TILETYPE_DIRT,
    TILETYPE_STONE_00,
    TILETYPE_STONE_01,
    TILETYPE_STONE_02,
    TILETYPE_STONE_03,
    TILETYPE_STONE_04,
    TILETYPE_STONE_05,
    TILETYPE_STONE_06,
    TILETYPE_STONE_07,
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

tile_t* dungeonfloor_get_tile(dungeonfloor_t* d, Vector2 pos);
