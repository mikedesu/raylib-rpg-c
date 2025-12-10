#pragma once

#include "biome.h"
#include "dungeon_tile.h"
#include "entityid.h"
#include "mprint.h"
#include "raylib.h"
#include "tile_id.h"
#include "vec3.h"
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#define DEFAULT_DUNGEON_FLOOR_WIDTH (16)
#define DEFAULT_DUNGEON_FLOOR_HEIGHT (12)


using std::function;
using std::shared_ptr;
using std::unordered_map;
using std::vector;


typedef struct {
    int floor; // the floor number, starting from 0
    int width;
    int height;

    vec3 downstairs_loc;
    vec3 upstairs_loc;

    biome_t biome;

    shared_ptr<vector<tile_id>> tiles; // 2D vector of shared pointers to tile_t
    shared_ptr<unordered_map<tile_id, shared_ptr<tile_t>>> tile_map; // Maps tile_id to tile_t pointer
} dungeon_floor_t;


//shared_ptr<dungeon_floor_t> df_create(int floor, dungeon_floor_type_t t, int width, int height);
shared_ptr<dungeon_floor_t> df_init(int floor, biome_t t, int width, int height);
//shared_ptr<dungeon_floor_t> df_create_with(int floor, dungeon_floor_type_t t, int width, int height, function<void()> mLambda);
void df_xform(function<void()> mLambda);

void df_set_area(shared_ptr<dungeon_floor_t> df, tiletype_t a, tiletype_t b, Rectangle r);
void df_set_perimeter(shared_ptr<dungeon_floor_t> df, tiletype_t a, tiletype_t b, Rectangle r);


void df_free(shared_ptr<dungeon_floor_t> df);
void df_set_tile(shared_ptr<dungeon_floor_t> const df, tiletype_t type, int x, int y);
entityid df_add_at(shared_ptr<dungeon_floor_t> const df, entityid id, int x, int y);
bool df_remove_at(shared_ptr<dungeon_floor_t> const df, entityid id, int x, int y);


bool df_assign_upstairs_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r);
bool df_assign_downstairs_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r);


vec3 df_get_upstairs(shared_ptr<dungeon_floor_t> const df);
vec3 df_get_downstairs(shared_ptr<dungeon_floor_t> const df);


int df_count_walkable(shared_ptr<dungeon_floor_t> const df);
int df_count_empty(shared_ptr<dungeon_floor_t> const df);
int df_count_empty_non_walls(shared_ptr<dungeon_floor_t> const df);
int df_count_empty_non_walls_in_area(shared_ptr<dungeon_floor_t> const df, int x0, int y0, int w, int h);
int df_count_non_walls_in_area(shared_ptr<dungeon_floor_t> const df, int x0, int y0, int w, int h);
int df_count_non_walls(shared_ptr<dungeon_floor_t> const df);
int df_center_x(shared_ptr<dungeon_floor_t> const df);
int df_center_y(shared_ptr<dungeon_floor_t> const df);


static inline shared_ptr<tile_t> df_tile_at(shared_ptr<dungeon_floor_t> df, vec3 loc) {
    massert(df, "df is NULL");
    if (loc.x < 0 || loc.x >= df->width || loc.y < 0 || loc.y >= df->height) {
        //merror("x or y out of bounds: x: %d, y: %d", loc.x, loc.y);
        return NULL;
    }
    // given that tiles is a 2D vector of shared pointers to tile_t
    // we can access the tile using the x and y coordinates
    // and calculate the index
    const size_t index = loc.y * df->width + loc.x;
    if (index < 0 || index >= df->tiles->size()) {
        return NULL;
    }
    const tile_id id = df->tiles->at(index);
    // Check if the tile_id exists in the map
    auto it = df->tile_map->find(id);
    if (it != df->tile_map->end())
        return it->second;
    //merror("Tile not found at location (%d, %d)", loc.x, loc.y);
    return NULL; // Tile not found
}


static inline tiletype_t df_type_at(shared_ptr<dungeon_floor_t> df, int x, int y) {
    massert(df, "df is NULL");
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        return TILE_NONE;
    }
    const tile_id id = df->tiles->at(y * df->width + x);
    auto it = df->tile_map->find(id);
    if (it != df->tile_map->end())
        // Return the tile type
        return it->second->type;
    // Tile not found
    return TILE_NONE;
}


static inline bool df_tile_is_wall(shared_ptr<dungeon_floor_t> df, int x, int y) {
    //massert(df, "dungeon floor is NULL");
    const tiletype_t type = df_type_at(df, x, y);
    if (type == TILE_NONE || type == TILE_COUNT)
        //merror("Tile type is invalid at (%d, %d)", x, y);
        return false;
    return tile_is_wall(type);
}


static inline void df_set_can_have_door(shared_ptr<dungeon_floor_t> df, vec3 loc) {
    minfo("set can have door: %d, %d, %d", loc.x, loc.y, loc.z);
    auto tile = df_tile_at(df, loc);
    if (tile) {
        msuccess("Setting can have door to true");
        tile->can_have_door = true;
    } else {
        merror("Cannot set door, tile is NULL");
    }
}
