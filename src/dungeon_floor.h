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
    int x, y, w, h;
    char room_name[64]; // shorten to reasonable length
} room_data_t;

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

    room_data_t* rooms; // dynamic array pointer
    int room_count; // current room number
    int room_capacity; // allocated capacity

} dungeon_floor_t;

dungeon_floor_t* df_create(int width, int height);
void df_init(dungeon_floor_t* df);
void df_free(dungeon_floor_t* f);

bool df_tile_is_wall(const dungeon_floor_t* const df, int x, int y);
tile_t* df_tile_at(const dungeon_floor_t* const df, int x, int y);
bool df_add_at(dungeon_floor_t* const df, entityid id, int x, int y);
bool df_remove_at(dungeon_floor_t* const df, entityid id, int x, int y);
tiletype_t df_type_at(const dungeon_floor_t* const df, const int x, const int y);
loc_t df_get_upstairs(const dungeon_floor_t* const df);
loc_t df_get_downstairs(const dungeon_floor_t* const df);
int df_count_walkable(const dungeon_floor_t* const df);
int df_center_x(const dungeon_floor_t* const df);
int df_center_y(const dungeon_floor_t* const df);
bool df_add_room(dungeon_floor_t* df, int x, int y, int w, int h, const char* name);
