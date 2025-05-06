#pragma once

#include "dungeon_tile.h"
#include "entityid.h"
#include "location.h"
#include "mprint.h"
#include "raylib.h"
#include "room_data.h"
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

    room_data_t* rooms; // dynamic array pointer
    int room_count; // current room number
    int room_capacity; // allocated capacity

    loc_t downstairs_loc;
    loc_t upstairs_loc;

} dungeon_floor_t;

dungeon_floor_t* df_create(int width, int height);
void df_init(dungeon_floor_t* df);
void df_free(dungeon_floor_t* f);

bool df_add_at(dungeon_floor_t* const df, entityid id, int x, int y);
bool df_remove_at(dungeon_floor_t* const df, entityid id, int x, int y);

loc_t df_get_upstairs(dungeon_floor_t* const df);
loc_t df_get_downstairs(dungeon_floor_t* const df);

int df_count_walkable(const dungeon_floor_t* const df);
int df_count_empty(const dungeon_floor_t* const df);
int df_count_empty_non_walls(const dungeon_floor_t* const df);

int df_center_x(const dungeon_floor_t* const df);
int df_center_y(const dungeon_floor_t* const df);

void df_init_rooms(dungeon_floor_t* df);
bool df_add_room(dungeon_floor_t* df, int x, int y, int w, int h, const char* name);

static inline tile_t* df_tile_at(const dungeon_floor_t* const df, const int x, const int y) {
    //minfo("df_tile_at: %d, %d", x, y);
    massert(df, "df is NULL");
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        merror("x or y out of bounds: x: %d, y: %d", x, y);
        return NULL;
    }
    tile_t* tile = &df->tiles[y][x];
    massert(tile, "tile is NULL");
    return tile;
}

static inline tiletype_t df_type_at(const dungeon_floor_t* const df, const int x, const int y) {
    massert(df, "df is NULL");
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        merror("x or y out of bounds");
        return TILE_NONE;
    }
    return df->tiles[y][x].type;
}

static inline bool df_tile_is_wall(const dungeon_floor_t* const df, int x, int y) {
    massert(df, "dungeon floor is NULL");
    tile_t* tile = df_tile_at(df, x, y);
    return tile_is_wall(tile->type);
}

//static inline const room_data_t* df_get_room_at(const dungeon_floor_t* df, int px, int py) {
//    massert(df, "dungeon floor is NULL");
//    massert(px >= 0, "px is less than zero");
//    massert(px < df->width, "px is out of bounds");
//    massert(py >= 0, "py is less than zero");
//    massert(py < df->height, "py is out of bounds");
//    for (int i = 0; i < df->room_count; i++) {
//        const room_data_t* r = &df->rooms[i];
//        if (px >= r->x && px < r->x + r->w && py >= r->y && py < r->y + r->h) return r;
//    }
//    return NULL;
//}
