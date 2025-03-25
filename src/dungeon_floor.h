#pragma once

#include "dungeon_tile.h"
#include "entityid.h"
#include <stdbool.h>

#define DEFAULT_DUNGEON_FLOOR_WIDTH 32
#define DEFAULT_DUNGEON_FLOOR_HEIGHT 8

typedef struct {
    dungeon_tile_t** tiles;
    int width;
    int height;
} dungeon_floor_t;

dungeon_floor_t* dungeon_floor_create(int width, int height);
dungeon_tile_t* dungeon_floor_tile_at(const dungeon_floor_t* const df, int x, int y);

void dungeon_floor_init(dungeon_floor_t* floor);
void dungeon_floor_free(dungeon_floor_t* floor);
void dungeon_floor_set_pressure_plate(dungeon_floor_t* const df, int x, int y, int up_tx_key, int dn_tx_key, int event);
void dungeon_floor_set_wall_switch(dungeon_floor_t* const df, int x, int y, int up_tx_key, int dn_tx_key, int event);
void df_set_all_tiles(dungeon_floor_t* const df, dungeon_tile_type_t type);
void df_set_tile_area(dungeon_floor_t* const df, dungeon_tile_type_t type, int x, int y, int w, int h);

bool df_tile_is_wall(const dungeon_floor_t* const df, int x, int y);
bool dungeon_floor_add_at(dungeon_floor_t* const df, entityid id, int x, int y);
bool dungeon_floor_remove_at(dungeon_floor_t* const df, entityid id, int x, int y);
