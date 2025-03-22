#pragma once

#include "dungeon_tile.h"
#include "entityid.h"
#include <stdbool.h>

#define DEFAULT_DUNGEON_FLOOR_WIDTH 16
#define DEFAULT_DUNGEON_FLOOR_HEIGHT 8
//#define DEFAULT_DUNGEON_FLOOR_WIDTH 256
//#define DEFAULT_DUNGEON_FLOOR_HEIGHT 128


typedef struct {
    dungeon_tile_t** tiles;
    int width;
    int height;

} dungeon_floor_t;


dungeon_floor_t* dungeon_floor_create(const int width, const int height);

dungeon_tile_t* dungeon_floor_tile_at(const dungeon_floor_t* const df, const int x, const int y);

const bool dungeon_floor_add_at(dungeon_floor_t* const df, const entityid id, const int x, const int y);
const bool dungeon_floor_remove_at(dungeon_floor_t* const df, const entityid id, const int x, const int y);

void dungeon_floor_init(dungeon_floor_t* floor);
void dungeon_floor_free(dungeon_floor_t* floor);


void dungeon_floor_set_pressure_plate(dungeon_floor_t* const df,
                                      const int x,
                                      const int y,
                                      const int up_tx_key,
                                      const int dn_tx_key,
                                      const int event);
