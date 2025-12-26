#pragma once

#include "biome.h"
#include "dungeon_tile.h"
#include "dungeon_tile_type.h"
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
    vector<tile_id> tiles; // 2D vector of shared pointers to tile_t
    shared_ptr<unordered_map<tile_id, tile_t>> tile_map; // Maps tile_id to tile_t pointer
} dungeon_floor_t;


static inline tile_t& df_tile_at(dungeon_floor_t& df, vec3 loc) {
    // given that tiles is a 2D vector of shared pointers to tile_t
    // we can access the tile using the x and y coordinates
    // and calculate the index
    const size_t index = loc.y * df.width + loc.x;
    massert(index < df.tiles.size(), "index is out-of-bounds: %d, %d", loc.x, loc.y);
    const tile_id id = df.tiles[index];
    // Check if the tile_id exists in the map
    auto it = df.tile_map->find(id);
    return it->second;
}


static inline void df_set_can_have_door(dungeon_floor_t& df, vec3 loc) {
    //minfo("set can have door: %d, %d, %d", loc.x, loc.y, loc.z);
    tile_t& tile = df_tile_at(df, loc);
    tile.can_have_door = true;
}


static inline bool df_is_good_door_loc(dungeon_floor_t& df, vec3 loc) {
    auto tile = df_tile_at(df, loc);
    if (loc.x >= 1 && loc.y >= 1 && loc.x < df.width - 1 && loc.y < df.height - 1) {
        auto t0 = df_tile_at(df, (vec3){loc.x - 1, loc.y - 1, loc.z});
        auto t1 = df_tile_at(df, (vec3){loc.x - 1, loc.y, loc.z});
        auto t2 = df_tile_at(df, (vec3){loc.x - 1, loc.y + 1, loc.z});
        auto t3 = df_tile_at(df, (vec3){loc.x, loc.y - 1, loc.z});
        auto t4 = df_tile_at(df, (vec3){loc.x, loc.y + 1, loc.z});
        auto t5 = df_tile_at(df, (vec3){loc.x + 1, loc.y - 1, loc.z});
        auto t6 = df_tile_at(df, (vec3){loc.x + 1, loc.y, loc.z});
        auto t7 = df_tile_at(df, (vec3){loc.x + 1, loc.y + 1, loc.z});
        auto tw0 = tile_is_wall(t0);
        auto tw1 = tile_is_wall(t1);
        auto tw2 = tile_is_wall(t2);
        auto tw3 = tile_is_wall(t3);
        auto tw4 = tile_is_wall(t4);
        auto tw5 = tile_is_wall(t5);
        auto tw6 = tile_is_wall(t6);
        auto tw7 = tile_is_wall(t7);
        if (tw3 && tw4 && !(tw0 || tw1 || tw2 || tw5 || tw6 || tw7))
            return true;
        if (tw1 && tw6 && !(tw0 || tw2 || tw3 || tw4 || tw5 || tw7))
            return true;
    }
    return false;
}


static inline void df_set_tile(dungeon_floor_t& df, tiletype_t type, int x, int y) {
    tile_t& current = df_tile_at(df, (vec3){x, y, -1});
    tile_init(current, type);
}


static inline tiletype_t random_tiletype(tiletype_t a, tiletype_t b) {
    return (tiletype_t)GetRandomValue(a, b);
}


static inline void df_set_area(dungeon_floor_t& df, tiletype_t a, tiletype_t b, Rectangle r) {
    //minfo("df set area");
    for (int x = r.x; x < r.x + r.width && x < df.width; x++) {
        for (int y = r.y; y < r.y + r.height && y < df.height; y++) {
            const tiletype_t t = random_tiletype(a, b);
            df_set_tile(df, t, x, y);
        }
    }
}


static inline void df_set_perimeter(dungeon_floor_t& df, tiletype_t a, tiletype_t b, Rectangle r) {
    for (int x = r.x; x < r.x + r.width; x++) {
        const tiletype_t t = random_tiletype(a, b);
        df_set_tile(df, t, x, r.y);
        df_set_tile(df, t, x, r.y + r.height - 1);
    }

    for (int y = r.y; y < r.y + r.height; y++) {
        const tiletype_t t = random_tiletype(a, b);
        df_set_tile(df, t, r.x, y);
        df_set_tile(df, t, r.x + r.width - 1, y);
    }
}


static inline void df_init(dungeon_floor_t& df, int floor, biome_t t, int width, int height) {
    massert(width > 0, "width must be greater than zero");
    massert(height > 0, "height must be greater than zero");
    massert(floor >= 0, "floor must be greater than or equal to zero");

    // creating a new dungeon floor
    // init floor vars
    df.floor = floor;
    df.width = width;
    df.height = height;
    df.biome = t;

    // alloc the tile map
    df.tile_map = make_shared<unordered_map<tile_id, tile_t>>();
    massert(df.tile_map, "failed to create tile map");

    // create all the tiles and add to the tile vector and tile map
    for (tile_id i = 0; i < width * height; i++) {
        df.tiles.push_back(i);
        //auto tile = make_shared<tile_t>();
        tile_t tile;
        //massert(tile, "failed to create tile");
        tile.id = i;
        tile_init(tile, TILE_NONE);
        df.tile_map->insert({i, tile});
    }

    // locate "center"
    // in the 24x24 case,
    // this is hard-coded atm but we will move it into a function
    const float x = width / 3.0;
    const float y = height / 3.0;
    const int w = 8;
    const int h = 8;
    // init the inner w x h area
    // create a new room with walls
    df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, (Rectangle){x, y, w, h});
    df_set_perimeter(df, TILE_STONE_WALL_01, TILE_STONE_WALL_01, (Rectangle){x - 1, y - 1, w + 2, h + 2});
    df_set_area(df, TILE_STONE_WALL_01, TILE_STONE_WALL_01, (Rectangle){x + w / 2.0f, y, 1, h});
    df_set_area(df, TILE_STONE_WALL_01, TILE_STONE_WALL_01, (Rectangle){x, y + h / 2.0f, w, 1});
    df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w / 2.0f, y + 1, 1, 1});
    df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w / 2.0f + 2, y + 4, 1, 2});
    df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w / 2.0f, y + 6, 1, 2});

    const vec3 loc_u = {9, 9, floor};
    df_set_tile(df, TILE_UPSTAIRS, loc_u.x, loc_u.y);
    df.upstairs_loc = loc_u;

    const vec3 loc_d = {9, 14, floor};
    df_set_tile(df, TILE_DOWNSTAIRS, loc_d.x, loc_d.y);
    df.downstairs_loc = loc_d;

    df_set_can_have_door(df, (vec3){12, 9, 0});
    if (df_tile_at(df, (vec3){12, 9, 0}).can_have_door) {
        msuccess("door has can have door set");
        //DEBUG_BREAK();
    } else {
        merror("door does not have can have door set");
        DEBUG_BREAK();
    }

    msuccess("Created dungeon floor %d with dimensions %dx%d", floor, width, height);
}


static inline void df_xform(function<void()> mLambda) {
    mLambda();
}


static inline int df_get_possible_downstairs_count_in_area(dungeon_floor_t& df, Rectangle r) {
    massert(r.x >= 0, "x is less than zero");
    massert(r.x < df.width, "x is out of bounds");
    massert(r.y >= 0, "y is less than zero");
    massert(r.y < df.height, "y is out of bounds");
    massert(r.width > 0, "w is less than zero");
    massert(r.height > 0, "h is less than zero");
    massert(r.x + r.width <= df.width, "x + w is out of bounds");
    massert(r.y + r.height <= df.height, "y + h is out of bounds");
    int count = 0;
    for (int y0 = r.y; y0 < df.height; y0++) {
        for (int x0 = r.x; x0 < df.width; x0++) {
            tile_t& tile = df_tile_at(df, (vec3){x0, y0, -1});
            // check if the tile is possible downstairs
            if (tile_is_possible_downstairs(tile.type))
                count++;
        }
    }
    return count;
}


static inline int df_get_possible_upstairs_count_in_area(dungeon_floor_t& df, Rectangle r) {
    massert(r.x >= 0, "x is less than zero");
    massert(r.x < df.width, "x is out of bounds");
    massert(r.y >= 0, "y is less than zero");
    massert(r.y < df.height, "y is out of bounds");
    massert(r.width > 0, "w is less than zero");
    massert(r.height > 0, "h is less than zero");
    massert(r.x + r.width <= df.width, "x + w is out of bounds");
    massert(r.y + r.height <= df.height, "y + h is out of bounds");

    int count = 0;
    for (int y0 = r.y; y0 < df.height; y0++) {
        for (int x0 = r.x; x0 < df.width; x0++) {
            tile_t& tile = df_tile_at(df, (vec3){x0, y0, -1});
            if (tile_is_possible_upstairs(tile.type))
                count++;
        }
    }
    return count;
}


static inline shared_ptr<vector<vec3>> df_get_possible_upstairs_locs_in_area(dungeon_floor_t& df, Rectangle r) {
    auto locations = make_shared<vector<vec3>>();
    massert(locations, "failed to make_shared locations");
    // now we can loop thru the dungeon floor again and fill the array with the locations
    for (int y0 = r.y; y0 < df.height; y0++) {
        for (int x0 = r.x; x0 < df.width; x0++) {
            tile_t& tile = df_tile_at(df, (vec3){x0, y0, -1});
            // there wont be any entities yet so do not check for them
            if (tile_is_possible_upstairs(tile.type))
                locations->push_back((vec3){x0, y0, 0});
        }
    }
    return locations;
}


static inline shared_ptr<vector<vec3>> df_get_possible_downstairs_locs_in_area(dungeon_floor_t& df, Rectangle r) {
    massert(r.x >= 0, "x is less than zero");
    massert(r.x < df.width, "x is out of bounds");
    massert(r.y >= 0, "y is less than zero");
    massert(r.y < df.height, "y is out of bounds");
    shared_ptr<vector<vec3>> locations = make_shared<vector<vec3>>();
    massert(locations, "failed to make_shared locations");
    // now we can loop thru the dungeon floor again and fill the array with the locations
    for (int y0 = r.y; y0 < df.height; y0++) {
        for (int x0 = r.x; x0 < df.width; x0++) {
            tile_t& tile = df_tile_at(df, (vec3){x0, y0, -1});
            // there wont be any entities yet so do not check for them
            if (tile_is_possible_downstairs(tile.type))
                locations->push_back((vec3){x0, y0, 0});
        }
    }
    return locations;
}


static inline int df_get_possible_upstairs_count(dungeon_floor_t& df) {
    return df_get_possible_upstairs_count_in_area(df, (Rectangle){0, 0, (float)df.width, (float)df.height});
}


static inline int df_get_possible_downstairs_count(dungeon_floor_t& df) {
    return df_get_possible_downstairs_count_in_area(df, (Rectangle){0, 0, (float)df.width, (float)df.height});
}


static inline shared_ptr<vector<vec3>> df_get_possible_upstairs_locs(dungeon_floor_t& df) {
    auto locs = df_get_possible_upstairs_locs_in_area(df, (Rectangle){0, 0, (float)df.width, (float)df.height});
    massert(locs, "failed to get possible upstairs locations");
    return locs;
}


static inline shared_ptr<vector<vec3>> df_get_possible_downstairs_locs(dungeon_floor_t& df) {
    auto locs = df_get_possible_downstairs_locs_in_area(df, (Rectangle){0, 0, (float)df.width, (float)df.height});
    massert(locs, "failed to get possible downstairs locations");
    return locs;
}


static inline void df_free(dungeon_floor_t& df) {
    df.tiles.clear();
    df.tile_map->clear();
}


static inline entityid df_add_at(dungeon_floor_t& df, entityid id, int x, int y) {
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(x >= 0 && x < df.width, "x is out of bounds");
    massert(y >= 0 && y < df.height, "y is out of bounds");
    tile_t& tile = df_tile_at(df, (vec3){x, y, -1});
    return tile_add(tile, id);
}

static inline bool df_remove_at(dungeon_floor_t& df, entityid id, int x, int y) {
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(x >= 0 && x < df.width, "x is out of bounds");
    massert(y >= 0 && y < df.height, "y is out of bounds");
    tile_t& tile = df_tile_at(df, (vec3){x, y, -1});
    const entityid r = tile_remove(tile, id);
    if (r == ENTITYID_INVALID) {
        merror("df_remove_at: Failed to remove entity %d at (%d, %d)", id, x, y);
        return false;
    }
    if (r != id) {
        merror("df_remove_at: Removed entity %d but expected to remove %d at (%d, %d)", r, id, x, y);
        return false;
    }
    return true;
}


static inline void df_set_all_tiles(dungeon_floor_t& df, tiletype_t type) {
    //minfo("df_set_all_tiles: Setting all tiles to type %d", type);
    df_set_area(df, type, type, (Rectangle){0, 0, (float)df.width, (float)df.height});
}


static inline vec3 df_get_random_loc(dungeon_floor_t& df) {
    vector<vec3> tmp;
    for (int x = 0; x < df.width; x++) {
        for (int y = 0; y < df.height; y++) {
            vec3 loc = {x, y, df.floor};
            auto tile = df_tile_at(df, loc);
            if (tile.type == TILE_NONE || tile.type == TILE_STONE_WALL_00 || tile.type == TILE_STONE_WALL_01 || tile.type == TILE_UPSTAIRS ||
                tile.type == TILE_DOWNSTAIRS)
                continue;
            if (tile.entities->size() > 0)
                continue;
            tmp.push_back(loc);
        }
    }
    if (tmp.size() == 0)
        return (vec3){-1, -1, -1};
    vec3 loc = tmp[GetRandomValue(0, tmp.size() - 1)];
    return loc;
}


static inline vec3 df_get_random_loc_of_type(dungeon_floor_t& df, tiletype_t type) {
    vector<vec3> tmp;
    for (int x = 0; x < df.width; x++) {
        for (int y = 0; y < df.height; y++) {
            vec3 loc = {x, y, df.floor};
            auto tile = df_tile_at(df, loc);
            if (tile.type != type)
                continue;
            //if (tile.type == TILE_NONE || tile.type == TILE_STONE_WALL_00 || tile.type == TILE_STONE_WALL_01 || tile.type == TILE_UPSTAIRS ||
            //    tile.type == TILE_DOWNSTAIRS)
            //    continue;
            //if (tile.entities->size() > 0)
            //    continue;
            tmp.push_back(loc);
        }
    }
    if (tmp.size() == 0)
        return (vec3){-1, -1, -1};
    vec3 loc = tmp[GetRandomValue(0, tmp.size() - 1)];
    return loc;
}
