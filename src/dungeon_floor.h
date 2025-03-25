#pragma once

#include "dungeon_tile.h"
#include "entityid.h"
#include <stdbool.h>

#define DEFAULT_DUNGEON_FLOOR_WIDTH 128
#define DEFAULT_DUNGEON_FLOOR_HEIGHT 128
#define DEFAULT_DF_EVENTS 16
#define DEFAULT_DF_PLATES 16

typedef int df_event_id;

typedef struct {
    df_event_id listen_event;
    int x;
    int y;
    dungeon_tile_type_t on_type;
    dungeon_tile_type_t off_type;
} df_event_t;

typedef struct {
    dungeon_tile_t** tiles;
    int width;
    int height;

    df_event_t events[DEFAULT_DF_EVENTS];
    bool plates[DEFAULT_DF_PLATES];

} dungeon_floor_t;

dungeon_floor_t* dungeon_floor_create(int width, int height);
dungeon_tile_t* dungeon_floor_tile_at(const dungeon_floor_t* const df, int x, int y);

bool df_tile_is_wall(const dungeon_floor_t* const df, int x, int y);
bool dungeon_floor_add_at(dungeon_floor_t* const df, entityid id, int x, int y);
bool dungeon_floor_remove_at(dungeon_floor_t* const df, entityid id, int x, int y);

void df_init(dungeon_floor_t* df);
void dungeon_floor_free(dungeon_floor_t* floor);
void dungeon_floor_set_pressure_plate(dungeon_floor_t* const df, int x, int y, int up_tx_key, int dn_tx_key, int event);
void dungeon_floor_set_wall_switch(dungeon_floor_t* const df, int x, int y, int up_tx_key, int dn_tx_key, int event);
void df_set_tile_area(dungeon_floor_t* const df, dungeon_tile_type_t type, int x, int y, int w, int h);
void df_set_all_tiles(dungeon_floor_t* const df, dungeon_tile_type_t type);
void df_set_all_tiles_range(dungeon_floor_t* const df, dungeon_tile_type_t begin, dungeon_tile_type_t end);
void df_set_tile(dungeon_floor_t* const df, dungeon_tile_type_t type, int x, int y);
void df_set_tile_perimeter(dungeon_floor_t* const df, dungeon_tile_type_t type, int x, int y, int w, int h);
void df_init_test(dungeon_floor_t* const df);

void df_set_tile_area_range(dungeon_floor_t* const df,
                            int x,
                            int y,
                            int w,
                            int h,
                            dungeon_tile_type_t begin,
                            dungeon_tile_type_t end);

void df_set_tile_perimeter_range(dungeon_floor_t* const df,
                                 dungeon_tile_type_t begin,
                                 dungeon_tile_type_t end,
                                 int x,
                                 int y,
                                 int w,
                                 int h);

void df_set_event(dungeon_floor_t* const df,
                  int x,
                  int y,
                  int event_id,
                  dungeon_tile_type_t on_type,
                  dungeon_tile_type_t off_type);

void df_reset_plates(dungeon_floor_t* const df);
void df_reset_events(dungeon_floor_t* const df);
bool df_malloc_tiles(dungeon_floor_t* const df);
