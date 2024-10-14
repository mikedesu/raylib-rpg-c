#pragma once

#include "tiletype.h"
#include "vectorentityid.h"
#include <raylib.h>
#include <stdlib.h>


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
