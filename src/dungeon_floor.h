#pragma once

#include "dungeon_tile.h"
#include "entityid.h"
#include "mprint.h"
#include "raylib.h"
#include "room_data.h"
#include "vec3.h"
//#include <stdbool.h>
#include "tile_id.h"
#include <memory>
#include <unordered_map>
#include <vector>

#define DEFAULT_DUNGEON_FLOOR_WIDTH 256
#define DEFAULT_DUNGEON_FLOOR_HEIGHT 256

typedef struct {
    //tile_id** tiles;
    //tile_id** tiles;
    std::shared_ptr<std::vector<tile_id>> tiles; // 2D vector of shared pointers to tile_t

    //tile_t** tiles;
    int floor; // the floor number, starting from 0
    int width;
    int height;

    // this will replace the tiles array
    std::shared_ptr<std::unordered_map<tile_id, std::shared_ptr<tile_t>>> tile_map; // Maps tile_id to tile_t pointer

    vec3 downstairs_loc;
    vec3 upstairs_loc;
} dungeon_floor_t;

dungeon_floor_t* df_create(int floor, int width, int height);

void df_set_tile_area_range(dungeon_floor_t* const df, int x, int y, int w, int h, tiletype_t begin, tiletype_t end);

void df_init(dungeon_floor_t* df);
void df_free(dungeon_floor_t* f);
bool df_add_at(dungeon_floor_t* const df, entityid id, int x, int y);
bool df_remove_at(dungeon_floor_t* const df, entityid id, int x, int y);
vec3 df_get_upstairs(dungeon_floor_t* const df);
vec3 df_get_downstairs(dungeon_floor_t* const df);

// Add these function declarations
void df_set_tile(dungeon_floor_t* const df, tiletype_t type, int x, int y);
bool df_assign_upstairs_in_area(dungeon_floor_t* df, int x, int y, int w, int h);
bool df_assign_downstairs_in_area(dungeon_floor_t* df, int x, int y, int w, int h);
vec3* const df_get_all_locs(dungeon_floor_t* const df, int* external_count);
//vec3* const df_get_all_locs_outside_of_rooms(dungeon_floor_t* const df, int* external_count);
int df_count_walkable(const dungeon_floor_t* const df);
int df_count_empty(const dungeon_floor_t* const df);
int df_count_empty_non_walls(const dungeon_floor_t* const df);
int df_count_empty_non_walls_in_area(const dungeon_floor_t* const df, int x0, int y0, int w, int h);
int df_count_non_walls_in_area(const dungeon_floor_t* const df, int x0, int y0, int w, int h);
int df_count_non_walls(const dungeon_floor_t* const df);

int df_center_x(const dungeon_floor_t* const df);
int df_center_y(const dungeon_floor_t* const df);

//static inline tile_t* df_tile_at(const dungeon_floor_t* const df, vec3 loc) {
//static inline std::shared_ptr<tile_t> df_tile_at(const dungeon_floor_t* const df, vec3 loc) {
static inline std::shared_ptr<tile_t> df_tile_at(std::shared_ptr<dungeon_floor_t> df, vec3 loc) {
    massert(df, "df is NULL");
    if (loc.x < 0 || loc.x >= df->width || loc.y < 0 || loc.y >= df->height) {
        merror("x or y out of bounds: x: %d, y: %d", loc.x, loc.y);
        return NULL;
    }

    // given that tiles is a 2D vector of shared pointers to tile_t
    // we can access the tile using the x and y coordinates
    // and calculate the index
    int index = loc.y * df->width + loc.x;
    if (index < 0 || index >= df->tiles->size()) {
        merror("Index out of bounds: %d", index);
        return NULL;
    }
    tile_id id = df->tiles->at(index);
    // Check if the tile_id exists in the map
    //auto it = df->tile_map.find(id);
    //if (it != df->tile_map.end()) {
    //    return it->second; // Return the shared pointer to tile_t
    //}
    auto it = df->tile_map->find(id);
    if (it != df->tile_map->end()) {
        return it->second; // Return the shared pointer to tile_t
    }
    merror("Tile not found at location (%d, %d)", loc.x, loc.y);
    return NULL; // Tile not found
}

static inline tiletype_t df_type_at(const dungeon_floor_t* const df, const int x, const int y) {
    massert(df, "df is NULL");
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        merror("x or y out of bounds");
        return TILE_NONE;
    }
    tile_id id = df->tiles->at(y * df->width + x);
    //auto it = df->tile_map.find(id);
    //if (it != df->tile_map.end()) {
    //    return it->second->type; // Return the tile type
    //}
    auto it = df->tile_map->find(id);
    if (it != df->tile_map->end()) {
        return it->second->type; // Return the tile type
    }

    merror("Tile not found at (%d, %d)", x, y);
    return TILE_NONE; // Tile not found
}

static inline bool df_tile_is_wall(const dungeon_floor_t* const df, int x, int y) {
    massert(df, "dungeon floor is NULL");
    //tile_t* tile = df_tile_at(df, (vec3){x, y, -1});
    //return tile_is_wall(tile->type);

    tiletype_t type = df_type_at(df, x, y);
    if (type == TILE_NONE || type == TILE_COUNT) {
        merror("Tile type is invalid at (%d, %d)", x, y);
        return false;
    }
    return tile_is_wall(type);
}
