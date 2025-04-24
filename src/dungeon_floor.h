#pragma once

#include "dungeon_tile.h"
#include "entityid.h"
#include "location.h"
#include "raylib.h"
#include <stdbool.h>

#define DEFAULT_DUNGEON_FLOOR_WIDTH 512
#define DEFAULT_DUNGEON_FLOOR_HEIGHT 512
#define DEFAULT_DF_EVENTS 128
#define DEFAULT_DF_PLATES 128

typedef int df_event_id;

typedef struct {
    df_event_id listen_event;
    int x;
    int y;
    tiletype_t on_type;
    tiletype_t off_type;
} df_event_t;

typedef struct {
    tile_t** tiles;
    int width;
    int height;

    df_event_t events[DEFAULT_DF_EVENTS];
    bool plates[DEFAULT_DF_PLATES];

} dungeon_floor_t;

dungeon_floor_t* df_create(int width, int height);
void df_init(dungeon_floor_t* df);
void df_free(dungeon_floor_t* f);
void df_set_tile_area_range(dungeon_floor_t* const df, int x, int y, int w, int h, tiletype_t begin, tiletype_t end);
bool df_tile_is_wall(const dungeon_floor_t* const df, int x, int y);

tile_t* df_tile_at(const dungeon_floor_t* const df, int x, int y);
bool df_add_at(dungeon_floor_t* const df, entityid id, int x, int y);
bool df_remove_at(dungeon_floor_t* const df, entityid id, int x, int y);
void df_set_pressure_plate(dungeon_floor_t* const df, int x, int y, int up_tx_key, int dn_tx_key, int event);
void df_set_wall_switch(dungeon_floor_t* const df, int x, int y, int up_tx_key, int dn_tx_key, int event);

void df_set_tile_area(dungeon_floor_t* const df, tiletype_t type, int x, int y, int w, int h);
void df_set_all_tiles(dungeon_floor_t* const df, tiletype_t type);
void df_set_all_tiles_range(dungeon_floor_t* const df, tiletype_t begin, tiletype_t end);
void df_set_tile(dungeon_floor_t* const df, tiletype_t type, int x, int y);
void df_set_tile_perimeter(dungeon_floor_t* const df, tiletype_t type, int x, int y, int w, int h);
void df_init_test(dungeon_floor_t* const df);
void df_set_tile_perimeter_range(dungeon_floor_t* const df, tiletype_t begin, tiletype_t end, int x, int y, int w, int h);
void df_set_event(dungeon_floor_t* const df, int x, int y, int event_id, tiletype_t on_type, tiletype_t off_type);
void df_reset_plates(dungeon_floor_t* const df);
void df_reset_events(dungeon_floor_t* const df);
void df_init_rect(dungeon_floor_t* df, int x, int y, int w, int h, tiletype_t t1, tiletype_t t2);
void df_create_trap_event(dungeon_floor_t* df, int x, int y, tiletype_t on, tiletype_t off, df_event_id id);
void df_place_wall_switch(dungeon_floor_t* df, int x, int y, int up, int down, df_event_id trigger_id);
void df_set_tile_area_range2(dungeon_floor_t* const df, Rectangle r, tiletype_t begin, tiletype_t end);
void df_init_rect2(dungeon_floor_t* df, Rectangle r, tiletype_t t1, tiletype_t t2);
loc_t df_get_upstairs(const dungeon_floor_t* const df);
loc_t df_get_downstairs(const dungeon_floor_t* const df);
tiletype_t df_type_at(const dungeon_floor_t* const df, const int x, const int y);
int df_count_walkable(const dungeon_floor_t* const df);

int df_center_x(const dungeon_floor_t* const df);
int df_center_y(const dungeon_floor_t* const df);
