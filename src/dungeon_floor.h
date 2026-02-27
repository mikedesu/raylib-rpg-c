#pragma once

#include "biome.h"
//#include "dungeon.h"
#include "dungeon_tile.h"
#include "dungeon_tile_type.h"
#include "entityid.h"
#include "mprint.h"
#include "raylib.h"
#include "room.h"
#include "tile_id.h"
#include "vec3.h"
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#define DUNGEON_FLOOR_WIDTH 256
#define DUNGEON_FLOOR_HEIGHT 256

using std::function;
using std::make_shared;
using std::shared_ptr;
using std::unordered_map;
using std::vector;

class dungeon_floor {
private:
    int floor; // the floor number, starting from 0
    int width;
    int height;
    biome_t biome;
    vec3 downstairs_loc;
    vec3 upstairs_loc;

    //vector<tile_id> tiles;

    //shared_ptr<vector<entityid>> living_npcs;
    //shared_ptr<vector<entityid>> dead_npcs;
    //shared_ptr<vector<room>> room_metadatas;
    //shared_ptr<unordered_map<tile_id, shared_ptr<tile_t>>> tile_map; // Maps tile_id to tile_t pointer

    tile_t grid[DUNGEON_FLOOR_HEIGHT][DUNGEON_FLOOR_WIDTH];


public:
    //bool room_id_exists(room_id id) {
    //    for (auto room : *room_metadatas) {
    //        if (room.get_id() == id) {
    //            return true;
    //        }
    //    }
    //    return false;
    //}

    //bool room_area_collides(Rectangle a) {
    //    for (auto room : *room_metadatas) {
    //        const Rectangle r = room.get_area();
    //        if (CheckCollisionRecs(a, r)) {
    //            return true;
    //        }
    //    }
    //    return false;
    //}

    //bool add_room(room r) {
    //    // need to check for existing rooms
    //    if (room_id_exists(r.get_id())) {
    //        return false;
    //    }
    //    else if (room_area_collides(r.get_area())) {
    //        return false;
    //    }
    //    room_metadatas->push_back(r);
    //    return true;
    //}

    //room get_room_metadata(size_t i) {
    //    return room_metadatas->at(i);
    //}

    //shared_ptr<vector<room>> get_room_metadatas() {
    //    return room_metadatas;
    //}

    //shared_ptr<vector<entityid>> get_dead_npcs() {
    //    return dead_npcs;
    //}

    //shared_ptr<vector<entityid>> get_living_npcs() {
    //return living_npcs;
    //}

    //void add_dead_npc(entityid id) {
    //    massert(id != ENTITYID_INVALID, "id is invalid");
    //    dead_npcs->push_back(id);
    //}

    //bool remove_dead_npc(entityid id) {
    //    massert(id != ENTITYID_INVALID, "id is invalid");
    //    auto it = find(dead_npcs->cbegin(), dead_npcs->cend(), id);
    //    if (it == dead_npcs->cend()) {
    //        return false;
    //    }
    //    dead_npcs->erase(it);
    //    return true;
    //}

    //void add_living_npc(entityid id) {
    //massert(id != ENTITYID_INVALID, "id is invalid");
    //living_npcs->push_back(id);
    //}

    //bool remove_living_npc(entityid id) {
    //massert(id != ENTITYID_INVALID, "id is invalid");

    //auto it = find(living_npcs->cbegin(), living_npcs->cend(), id);
    //if (it == living_npcs->cend()) {
    //return false;
    //}
    //living_npcs->erase(it);
    //return true;
    //}

    const vec3 get_upstairs_loc() {
        return upstairs_loc;
    }

    const vec3 get_downstairs_loc() {
        return downstairs_loc;
    }

    const biome_t get_biome() {
        return biome;
    }

    const int get_height() {
        return height;
    }

    const int get_width() {
        return width;
    }

    const int get_floor() {
        return floor;
    }

    //inline shared_ptr<tile_t> tile_at(vec3 loc) {
    inline tile_t& tile_at(vec3 loc) {
        // given that tiles is a 2D vector of shared pointers to tile_t
        // we can access the tile using the x and y coordinates
        // and calculate the index
        //const size_t index = loc.y * width + loc.x;
        minfo3("tile_at: (%d, %d, %d)", loc.x, loc.y, loc.z);

        //massert(index < tiles.size(), "index is out-of-bounds: %d, %d", loc.x, loc.y);
        //if (index >= tiles.size()) {
        //    merror("index is out-of-bounds: %d, %d", loc.x, loc.y);
        //    return nullptr;
        //}

        //const tile_id id = tiles[index];
        // Check if the tile_id exists in the map
        //auto it = tile_map->find(id);
        //return it->second;
    
        return grid[loc.y][loc.x];
    }

    inline void df_set_can_have_door(const vec3 loc) {
        //minfo("set can have door: %d, %d, %d", loc.x, loc.y, loc.z);
        //shared_ptr<tile_t> tile = tile_at(loc);
        tile_t& tile = tile_at(loc);


        tile.set_can_have_door(true);
    }

    inline bool df_is_good_door_loc(vec3 loc) {
        auto tile = tile_at(loc);
        if (loc.x >= 1 && loc.y >= 1 && loc.x < width - 1 && loc.y < height - 1) {
            tile_t& t0 = tile_at((vec3){loc.x - 1, loc.y - 1, loc.z});
            tile_t& t1 = tile_at((vec3){loc.x - 1, loc.y, loc.z});
            tile_t& t2 = tile_at((vec3){loc.x - 1, loc.y + 1, loc.z});
            tile_t& t3 = tile_at((vec3){loc.x, loc.y - 1, loc.z});
            tile_t& t4 = tile_at((vec3){loc.x, loc.y + 1, loc.z});
            tile_t& t5 = tile_at((vec3){loc.x + 1, loc.y - 1, loc.z});
            tile_t& t6 = tile_at((vec3){loc.x + 1, loc.y, loc.z});
            tile_t& t7 = tile_at((vec3){loc.x + 1, loc.y + 1, loc.z});
            auto tw0 = t0.tile_is_wall();
            auto tw1 = t1.tile_is_wall();
            auto tw2 = t2.tile_is_wall();
            auto tw3 = t3.tile_is_wall();
            auto tw4 = t4.tile_is_wall();
            auto tw5 = t5.tile_is_wall();
            auto tw6 = t6.tile_is_wall();
            auto tw7 = t7.tile_is_wall();
            if (tw3 && tw4 && !(tw0 || tw1 || tw2 || tw5 || tw6 || tw7)) {
                return true;
            }
            if (tw1 && tw6 && !(tw0 || tw2 || tw3 || tw4 || tw5 || tw7)) {
                return true;
            }
        }
        return false;
    }

    inline bool tile_is_good_for_upgrade(vec3 loc) {
        auto tile = tile_at(loc);
        if (loc.x >= 1 && loc.y >= 1 && loc.x < width - 1 && loc.y < height - 1) {
            auto t0 = tile_at((vec3){loc.x - 1, loc.y - 1, loc.z});
            auto t1 = tile_at((vec3){loc.x - 1, loc.y, loc.z});
            auto t2 = tile_at((vec3){loc.x - 1, loc.y + 1, loc.z});
            auto t3 = tile_at((vec3){loc.x, loc.y - 1, loc.z});
            auto t4 = tile_at((vec3){loc.x, loc.y + 1, loc.z});
            auto t5 = tile_at((vec3){loc.x + 1, loc.y - 1, loc.z});
            auto t6 = tile_at((vec3){loc.x + 1, loc.y, loc.z});
            auto t7 = tile_at((vec3){loc.x + 1, loc.y + 1, loc.z});

            /*
             035
             1.6
             247
             */

            auto top_row_none = t0.get_type() == TILE_NONE && t3.get_type() == TILE_NONE && t5.get_type() == TILE_NONE;

            auto bottom_row_none = t2.get_type() == TILE_NONE && t4.get_type() == TILE_NONE && t7.get_type() == TILE_NONE;

            auto left_not_none = t1.get_type() != TILE_NONE;
            auto right_not_none = t6.get_type() != TILE_NONE;

            auto case1 = top_row_none && bottom_row_none && left_not_none && right_not_none;

            if (case1) {
                return true;
            }
        }
        return false;
    }

    inline void df_set_tile(const tiletype_t type, const int x, const int y) {
        //shared_ptr<tile_t> current = tile_at((vec3){x, y, -1});
        tile_t&  current = tile_at((vec3){x, y, -1});
        current.set_type(type);
        //current.tile_init(type);
    }

    const inline tiletype_t random_tiletype(const tiletype_t a, const tiletype_t b) {
        return (tiletype_t)GetRandomValue(a, b);
    }

    inline void set_area(const tiletype_t a, const tiletype_t b, const Rectangle r) {
        //minfo("df set area");
        for (int x = r.x; x < r.x + r.width && x < width; x++) {
            for (int y = r.y; y < r.y + r.height && y < height; y++) {
                const tiletype_t t = random_tiletype(a, b);
                df_set_tile(t, x, y);
            }
        }
    }

    inline void df_set_perimeter(const tiletype_t a, const tiletype_t b, const Rectangle r) {
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





    //inline void init(const int f, const biome_t t, const int w, const int h) {
    inline void init(const int f, const biome_t t) {
        //minfo2("df init: f=%d, t=%d, w=%d, h=%d", f, t, w, h);
        //massert(w > 0, "width must be greater than zero");
        //massert(h > 0, "height must be greater than zero");
        massert(f >= 0, "floor must be greater than or equal to zero");
        // creating a new dungeon floor
        // init floor vars
        floor = f;
        width = DUNGEON_FLOOR_WIDTH;
        height = DUNGEON_FLOOR_HEIGHT;
        biome = t;
        // alloc the tile map
        //tile_map = make_shared<unordered_map<tile_id, shared_ptr<tile_t>>>();
        //massert(tile_map, "failed to create tile map");
        // create all the tiles and add to the tile vector and tile map
        //for (tile_id i = 0; i < width * height; i++) {
            //minfo2("Begin loop %d", i);
            //tiles.push_back(i);
            //shared_ptr<tile_t> tile = make_shared<tile_t>(TILE_NONE, i);
            //tile_map->insert({i, tile});
        //}


        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                grid[i][j].set_type(TILE_NONE);
                grid[i][j].set_id(i*64+j);
            }
        }





        //df_set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w2 / 2.0f, y + 6, 1, 2});
        //df_set_tile(TILE_UPSTAIRS, loc_u.x, loc_u.y);
        //df_set_tile(TILE_DOWNSTAIRS, loc_d.x, loc_d.y);
        msuccess2("Created dungeon floor %d with dimensions %dx%d", floor, width, height);
    }





    inline void df_xform(function<void()> mLambda) {
        mLambda();
    }






    inline int df_get_possible_downstairs_count_in_area(const Rectangle r) {
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
                //shared_ptr<tile_t> tile = tile_at((vec3){x0, y0, -1});
                tile_t& tile = tile_at(vec3{x0, y0, -1});
                // check if the tile is possible downstairs
                //if (tile_is_possible_downstairs(tile->get_type())) {
                if (tile_is_possible_downstairs(tile.get_type())) {
                    count++;
                }
            }
        }
        return count;
    }






    inline int df_get_possible_upstairs_count_in_area(const Rectangle r) {
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
                //shared_ptr<tile_t> tile = tile_at((vec3){x0, y0, -1});
                tile_t& tile = tile_at(vec3{x0, y0, -1});
                if (tile_is_possible_upstairs(tile.get_type())) {
                    count++;
                }
            }
        }
        return count;
    }






    inline shared_ptr<vector<vec3>> df_get_possible_upstairs_locs_in_area(const Rectangle r) {
        auto locations = make_shared<vector<vec3>>();
        massert(locations, "failed to make_shared locations");
        // now we can loop thru the dungeon floor again and fill the array with the locations
        for (int y0 = r.y; y0 < height; y0++) {
            for (int x0 = r.x; x0 < width; x0++) {
                //shared_ptr<tile_t> tile = tile_at((vec3){x0, y0, -1});
                tile_t& tile = tile_at(vec3{x0, y0, -1});
                // there wont be any entities yet so do not check for them
                //if (tile_is_possible_upstairs(tile->get_type())) {
                if (tile_is_possible_upstairs(tile.get_type())) {
                    locations->push_back(vec3{x0, y0, 0});
                }
            }
        }
        return locations;
    }






    inline shared_ptr<vector<vec3>> df_get_possible_downstairs_locs_in_area(const Rectangle r) {
        massert(r.x >= 0, "x is less than zero");
        massert(r.x < width, "x is out of bounds");
        massert(r.y >= 0, "y is less than zero");
        massert(r.y < height, "y is out of bounds");
        shared_ptr<vector<vec3>> locations = make_shared<vector<vec3>>();
        massert(locations, "failed to make_shared locations");
        // now we can loop thru the dungeon floor again and fill the array with the locations
        for (int y0 = r.y; y0 < height; y0++) {
            for (int x0 = r.x; x0 < width; x0++) {
                //shared_ptr<tile_t> tile = tile_at((vec3){x0, y0, -1});
                tile_t& tile = tile_at(vec3{x0, y0, -1});
                // there wont be any entities yet so do not check for them
                //if (tile_is_possible_downstairs(tile->get_type())) {
                if (tile_is_possible_downstairs(tile.get_type())) {
                    locations->push_back(vec3{x0, y0, 0});
                }
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
        //tiles.clear();
        //tile_map->clear();
    }






    //inline entityid df_add_at(entityid id, int x, int y)
    inline entityid df_add_at(entityid id, entitytype_t type, vec3 loc) {
        if (id == ENTITYID_INVALID) {
            minfo("id is invalid");
            return ENTITYID_INVALID;
        }

        //massert(id != ENTITYID_INVALID, "id is invalid");
        massert(!vec3_invalid(loc), "loc is invalid");
        massert(loc.x >= 0 && loc.x < width, "x is out of bounds");
        massert(loc.y >= 0 && loc.y < height, "y is out of bounds");

        //shared_ptr<tile_t> tile = df_tile_at(vec3{x, y, -1});
        //shared_ptr<tile_t> tile = tile_at(loc);
        tile_t& tile = tile_at(loc);
        //const entityid result = tile->tile_add(id, type);
        const entityid result = tile.tile_add(id, type);
        if (result == ENTITYID_INVALID) {
            minfo("tile_add returned %d", result);
        }
        return result;
    }




    //inline bool df_remove_at(entityid id, int x, int y)
    inline bool df_remove_at(entityid id, vec3 l) {
        massert(id != ENTITYID_INVALID, "id is -1");
        massert(l.x >= 0 && l.x < width, "x is out of bounds");
        massert(l.y >= 0 && l.y < height, "y is out of bounds");
        //shared_ptr<tile_t> tile = tile_at(l);
        tile_t& tile = tile_at(l);
        //const entityid r = tile->tile_remove(id);
        const entityid r = tile.tile_remove(id);
        if (r == ENTITYID_INVALID) {
            merror("df_remove_at: Failed to remove entity %d at (%d, %d)", id, l.x, l.y);
            return false;
        }

        if (r != id) {
            merror("df_remove_at: Removed entity %d but expected to remove %d at (%d, %d)", r, id, l.x, l.y);
            return false;
        }
        return true;
    }





    inline void df_set_all_tiles(const tiletype_t type) {
        //minfo("df_set_all_tiles: Setting all tiles to type %d", type);
        set_area(type, type, (Rectangle){0, 0, (float)width, (float)height});
    }





    const inline vec3 get_random_loc() {
        vector<vec3> tmp;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                const vec3 loc = {x, y, floor};
                tile_t& tile = tile_at(loc);
                const bool type_invalid = tile.get_type() == TILE_NONE || tile.get_type() == TILE_STONE_WALL_00 || tile.get_type() == TILE_STONE_WALL_01 ||
                                          tile.get_type() == TILE_UPSTAIRS || tile.get_type() == TILE_DOWNSTAIRS;
                if (type_invalid) {
                    //merror("loc at (%d, %d, %d) type invalid. type is: %d", x, y, floor, tile.type);
                    continue;
                }
                if (tile.entity_count() > 0) {
                    //merror("loc at (%d, %d, %d) type invalid. tile has %ld entities", x, y, floor, tile.entities->size());
                    continue;
                }
                tmp.push_back(loc);
            }
        }
        if (tmp.size() == 0) {
            merror2("no locations are suitable. returning (-1, -1, -1)");
            return (vec3){-1, -1, -1};
        }
        return tmp[GetRandomValue(0, tmp.size() - 1)];
    }





    const inline vec3 df_get_random_loc_of_type(const tiletype_t type) {
        vector<vec3> tmp;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                vec3 loc = {x, y, floor};
                tile_t& tile = tile_at(loc);
                if (tile.get_type() != type) {
                    continue;
                }
                //if (tile.type == TILE_NONE || tile.type == TILE_STONE_WALL_00 || tile.type == TILE_STONE_WALL_01 || tile.type == TILE_UPSTAIRS ||
                //    tile.type == TILE_DOWNSTAIRS)
                //    continue;
                //if (tile.entities->size() > 0)
                //    continue;
                tmp.push_back(loc);
            }
        }
        if (tmp.size() == 0) {
            return (vec3){-1, -1, -1};
        }
        vec3 loc = tmp[GetRandomValue(0, tmp.size() - 1)];
        return loc;
    }


    dungeon_floor() {
        upstairs_loc = {-1, -1, -1};
        downstairs_loc = {-1, -1, -1};
    }


    ~dungeon_floor() {
        minfo2("dungeon floor destroyed");
    }
};
