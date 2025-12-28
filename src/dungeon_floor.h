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

class dungeon_floor_t {
public:
    int floor; // the floor number, starting from 0
    int width;
    int height;
    vec3 downstairs_loc;
    vec3 upstairs_loc;
    biome_t biome;
    vector<tile_id> tiles; // 2D vector of shared pointers to tile_t
    shared_ptr<unordered_map<tile_id, tile_t>> tile_map; // Maps tile_id to tile_t pointer


    inline tile_t& df_tile_at(vec3 loc) {
        // given that tiles is a 2D vector of shared pointers to tile_t
        // we can access the tile using the x and y coordinates
        // and calculate the index
        const size_t index = loc.y * width + loc.x;
        massert(index < tiles.size(), "index is out-of-bounds: %d, %d", loc.x, loc.y);
        const tile_id id = tiles[index];
        // Check if the tile_id exists in the map
        auto it = tile_map->find(id);
        return it->second;
    }


    inline void df_set_can_have_door(vec3 loc) {
        //minfo("set can have door: %d, %d, %d", loc.x, loc.y, loc.z);
        tile_t& tile = df_tile_at(loc);
        tile.can_have_door = true;
    }




    inline bool df_is_good_door_loc(vec3 loc) {
        auto tile = df_tile_at(loc);
        if (loc.x >= 1 && loc.y >= 1 && loc.x < width - 1 && loc.y < height - 1) {
            auto t0 = df_tile_at((vec3){loc.x - 1, loc.y - 1, loc.z});
            auto t1 = df_tile_at((vec3){loc.x - 1, loc.y, loc.z});
            auto t2 = df_tile_at((vec3){loc.x - 1, loc.y + 1, loc.z});
            auto t3 = df_tile_at((vec3){loc.x, loc.y - 1, loc.z});
            auto t4 = df_tile_at((vec3){loc.x, loc.y + 1, loc.z});
            auto t5 = df_tile_at((vec3){loc.x + 1, loc.y - 1, loc.z});
            auto t6 = df_tile_at((vec3){loc.x + 1, loc.y, loc.z});
            auto t7 = df_tile_at((vec3){loc.x + 1, loc.y + 1, loc.z});
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




    inline void df_set_tile(tiletype_t type, int x, int y) {
        tile_t& current = df_tile_at((vec3){x, y, -1});
        tile_init(current, type);
    }


    inline tiletype_t random_tiletype(tiletype_t a, tiletype_t b) {
        return (tiletype_t)GetRandomValue(a, b);
    }

    inline void df_set_area(tiletype_t a, tiletype_t b, Rectangle r) {
        //minfo("df set area");
        for (int x = r.x; x < r.x + r.width && x < width; x++) {
            for (int y = r.y; y < r.y + r.height && y < height; y++) {
                const tiletype_t t = random_tiletype(a, b);
                df_set_tile(t, x, y);
            }
        }
    }




    inline void df_set_perimeter(tiletype_t a, tiletype_t b, Rectangle r) {
        for (int x = r.x; x < r.x + r.width; x++) {
            const tiletype_t t = random_tiletype(a, b);
            df_set_tile(t, x, r.y);
            df_set_tile(t, x, r.y + r.height - 1);
        }

        for (int y = r.y; y < r.y + r.height; y++) {
            const tiletype_t t = random_tiletype(a, b);
            df_set_tile(t, r.x, y);
            df_set_tile(t, r.x + r.width - 1, y);
        }
    }




    inline void init(const int f, const biome_t t, const int w, const int h) {
        massert(w > 0, "width must be greater than zero");
        massert(h > 0, "height must be greater than zero");
        massert(floor >= 0, "floor must be greater than or equal to zero");

        // creating a new dungeon floor
        // init floor vars
        floor = f;
        width = w;
        height = h;
        biome = t;

        // alloc the tile map
        tile_map = make_shared<unordered_map<tile_id, tile_t>>();
        massert(tile_map, "failed to create tile map");

        // create all the tiles and add to the tile vector and tile map
        for (tile_id i = 0; i < width * height; i++) {
            tiles.push_back(i);
            //auto tile = make_shared<tile_t>();
            tile_t tile;
            //massert(tile, "failed to create tile");
            tile.id = i;
            tile_init(tile, TILE_NONE);
            tile_map->insert({i, tile});
        }

        // locate "center"
        // in the 24x24 case,
        // this is hard-coded atm but we will move it into a function
        const float x = width / 3.0;
        const float y = height / 3.0;
        const int w2 = 8;
        const int h2 = 8;
        // init the inner w x h area
        // create a new room with walls
        df_set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, (Rectangle){x, y, w2, h2});
        df_set_perimeter(TILE_STONE_WALL_01, TILE_STONE_WALL_01, (Rectangle){x - 1, y - 1, w2 + 2, h2 + 2});
        df_set_area(TILE_STONE_WALL_01, TILE_STONE_WALL_01, (Rectangle){x + w2 / 2.0f, y, 1, h2});
        df_set_area(TILE_STONE_WALL_01, TILE_STONE_WALL_01, (Rectangle){x, y + h2 / 2.0f, w2, 1});
        df_set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w2 / 2.0f, y + 1, 1, 1});
        df_set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w2 / 2.0f + 2, y + 4, 1, 2});
        df_set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w2 / 2.0f, y + 6, 1, 2});

        const vec3 loc_u = {9, 9, floor};
        df_set_tile(TILE_UPSTAIRS, loc_u.x, loc_u.y);
        upstairs_loc = loc_u;

        const vec3 loc_d = {9, 14, floor};
        df_set_tile(TILE_DOWNSTAIRS, loc_d.x, loc_d.y);
        downstairs_loc = loc_d;

        df_set_can_have_door((vec3){12, 9, 0});
        if (df_tile_at((vec3){12, 9, 0}).can_have_door) {
            //msuccess("door has can have door set");
            //DEBUG_BREAK();
        } else {
            //merror("door does not have can have door set");
            DEBUG_BREAK();
        }

        //msuccess("Created dungeon floor %d with dimensions %dx%d", floor, width, height);
    }




    inline void df_xform(function<void()> mLambda) {
        mLambda();
    }



    inline int df_get_possible_downstairs_count_in_area(Rectangle r) {
        massert(r.x >= 0, "x is less than zero");
        massert(r.x < width, "x is out of bounds");
        massert(r.y >= 0, "y is less than zero");
        massert(r.y < height, "y is out of bounds");
        massert(r.width > 0, "w is less than zero");
        massert(r.height > 0, "h is less than zero");
        massert(r.x + r.width <= width, "x + w is out of bounds");
        massert(r.y + r.height <= height, "y + h is out of bounds");
        int count = 0;
        for (int y0 = r.y; y0 < height; y0++) {
            for (int x0 = r.x; x0 < width; x0++) {
                tile_t& tile = df_tile_at((vec3){x0, y0, -1});
                // check if the tile is possible downstairs
                if (tile_is_possible_downstairs(tile.type))
                    count++;
            }
        }
        return count;
    }



    inline int df_get_possible_upstairs_count_in_area(Rectangle r) {
        massert(r.x >= 0, "x is less than zero");
        massert(r.x < width, "x is out of bounds");
        massert(r.y >= 0, "y is less than zero");
        massert(r.y < height, "y is out of bounds");
        massert(r.width > 0, "w is less than zero");
        massert(r.height > 0, "h is less than zero");
        massert(r.x + r.width <= width, "x + w is out of bounds");
        massert(r.y + r.height <= height, "y + h is out of bounds");

        int count = 0;
        for (int y0 = r.y; y0 < height; y0++) {
            for (int x0 = r.x; x0 < width; x0++) {
                tile_t& tile = df_tile_at((vec3){x0, y0, -1});
                if (tile_is_possible_upstairs(tile.type))
                    count++;
            }
        }
        return count;
    }



    inline shared_ptr<vector<vec3>> df_get_possible_upstairs_locs_in_area(Rectangle r) {
        auto locations = make_shared<vector<vec3>>();
        massert(locations, "failed to make_shared locations");
        // now we can loop thru the dungeon floor again and fill the array with the locations
        for (int y0 = r.y; y0 < height; y0++) {
            for (int x0 = r.x; x0 < width; x0++) {
                tile_t& tile = df_tile_at((vec3){x0, y0, -1});
                // there wont be any entities yet so do not check for them
                if (tile_is_possible_upstairs(tile.type))
                    locations->push_back((vec3){x0, y0, 0});
            }
        }
        return locations;
    }


    inline shared_ptr<vector<vec3>> df_get_possible_downstairs_locs_in_area(Rectangle r) {
        massert(r.x >= 0, "x is less than zero");
        massert(r.x < width, "x is out of bounds");
        massert(r.y >= 0, "y is less than zero");
        massert(r.y < height, "y is out of bounds");
        shared_ptr<vector<vec3>> locations = make_shared<vector<vec3>>();
        massert(locations, "failed to make_shared locations");
        // now we can loop thru the dungeon floor again and fill the array with the locations
        for (int y0 = r.y; y0 < height; y0++) {
            for (int x0 = r.x; x0 < width; x0++) {
                tile_t& tile = df_tile_at((vec3){x0, y0, -1});
                // there wont be any entities yet so do not check for them
                if (tile_is_possible_downstairs(tile.type))
                    locations->push_back((vec3){x0, y0, 0});
            }
        }
        return locations;
    }



    inline int df_get_possible_upstairs_count() {
        return df_get_possible_upstairs_count_in_area((Rectangle){0, 0, (float)width, (float)height});
    }

    inline int df_get_possible_downstairs_count() {
        return df_get_possible_downstairs_count_in_area((Rectangle){0, 0, (float)width, (float)height});
    }


    inline shared_ptr<vector<vec3>> df_get_possible_upstairs_locs() {
        auto locs = df_get_possible_upstairs_locs_in_area((Rectangle){0, 0, (float)width, (float)height});
        massert(locs, "failed to get possible upstairs locations");
        return locs;
    }


    inline shared_ptr<vector<vec3>> df_get_possible_downstairs_locs() {
        auto locs = df_get_possible_downstairs_locs_in_area((Rectangle){0, 0, (float)width, (float)height});
        massert(locs, "failed to get possible downstairs locations");
        return locs;
    }



    inline void df_free() {
        //minfo("df_free");
        tiles.clear();
        tile_map->clear();
    }




    const inline entityid df_add_at(const entityid id, const int x, const int y) {
        //minfo("df_add_at: %d, %d, %d", id, x, y);
        massert(id != ENTITYID_INVALID, "id is invalid");
        massert(x >= 0 && x < width, "x is out of bounds");
        massert(y >= 0 && y < height, "y is out of bounds");
        minfo("getting tile...");
        tile_t& tile = df_tile_at((vec3){x, y, -1});
        minfo("adding to tile...");
        const entityid result = tile_add(tile, id);
        minfo("tile_add returned %d", result);
        return result;
    }




    inline bool df_remove_at(entityid id, int x, int y) {
        massert(id != ENTITYID_INVALID, "id is -1");
        massert(x >= 0 && x < width, "x is out of bounds");
        massert(y >= 0 && y < height, "y is out of bounds");
        tile_t& tile = df_tile_at((vec3){x, y, -1});
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


    inline void df_set_all_tiles(tiletype_t type) {
        //minfo("df_set_all_tiles: Setting all tiles to type %d", type);
        df_set_area(type, type, (Rectangle){0, 0, (float)width, (float)height});
    }



    const inline vec3 df_get_random_loc() {
        vector<vec3> tmp;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                const vec3 loc = {x, y, floor};
                auto tile = df_tile_at(loc);
                const bool type_invalid = tile.type == TILE_NONE || tile.type == TILE_STONE_WALL_00 || tile.type == TILE_STONE_WALL_01 ||
                                          tile.type == TILE_UPSTAIRS || tile.type == TILE_DOWNSTAIRS;
                if (type_invalid)
                    continue;
                if (tile.entities->size() > 0)
                    continue;
                tmp.push_back(loc);
            }
        }
        if (tmp.size() == 0)
            return (vec3){-1, -1, -1};
        return tmp[GetRandomValue(0, tmp.size() - 1)];
    }



    const inline vec3 df_get_random_loc_of_type(tiletype_t type) {
        vector<vec3> tmp;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                vec3 loc = {x, y, floor};
                auto tile = df_tile_at(loc);
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




    dungeon_floor_t() {
    }

    ~dungeon_floor_t() {
    }
};
