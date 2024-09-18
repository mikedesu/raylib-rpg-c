#pragma once

#include "mprint.h"
#include <stdlib.h>

typedef enum {
    TILETYPE_NONE,
    TILETYPE_VOID,
    TILETYPE_DIRT,
    TILETYPE_COUNT

} tiletype_t;


typedef struct tile_t {
    tiletype_t type;
} tile_t;


typedef struct dungeonfloor_t {
    int len;
    int wid;
    tile_t** grid;

} dungeonfloor_t;

dungeonfloor_t* create_dungeonfloor(int len, int wid, tiletype_t basetype);
