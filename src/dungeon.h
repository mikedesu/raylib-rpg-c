#pragma once

#include "dungeon_floor.h"
#include "dungeon_tile_type.h"
#include "dungeonfloorid.h"
#include <functional>
#include <memory>
#include <optional>
#include <vector>

using std::function;
using std::make_shared;
using std::shared_ptr;
using std::vector;


#define INITIAL_DUNGEON_CAPACITY 4


class dungeon
{
public:
    vector<shared_ptr<dungeon_floor>> floors; // vector of shared pointers to dungeon_floor_t
    int current_floor;
    bool is_locked;
    bool is_initialized;



    dungeon()
    {
        current_floor = 0;
        is_locked = false;
        is_initialized = false;
    }




    ~dungeon()
    {
        floors.clear();
    }




    const inline size_t get_floor_count()
    {
        return floors.size();
    }




    inline shared_ptr<dungeon_floor> get_floor(const size_t index)
    {
        //minfo2("get_floor: %ld", index);
        massert(index >= 0 && index < floors.size(), "index is OOB: index is %ld", index);
        return floors.at(index);
    }




    inline shared_ptr<dungeon_floor> get_current_floor()
    {
        return get_floor(current_floor);
    }




    inline void add_floor(const biome_t type, const int width, const int height)
    {
        //minfo2("dungeon add floor");

        massert(width > 0, "width is 0");
        massert(height > 0, "height is 0");
        massert(!is_locked, "dungeon is locked");

        shared_ptr<dungeon_floor> df = make_shared<dungeon_floor>();
        df->init(floors.size(), type, width, height);
        //minfo("creation rules...");

        // at present, these creation rules do not account for the df's width and height
        // as passed in by the parameters to this function or the properties of the df
        // this is not ideal
        auto creation_rules = [&df, width, height]()
        {
            //const float x = df.width / 4.0;
            //const float y = df.height / 4.0;
            //constexpr int w = 4;
            //constexpr int h = 4;
            // draw room 1
            //df->df_set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, {0, 0, static_cast<float>(width), static_cast<float>(height)});

            // draw room 1
            df->df_set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, {0, 0, 5.0, 5.0});
            df->df_set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, {6, 0, 5.0, 5.0});
            //df->df_set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, {0, 6, 5.0, 5.0});
            //df->df_set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, {6, 6, 5.0, 5.0});

            df->df_set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, {5, 2, 1.0, 1.0});
            df->df_set_can_have_door((vec3){5, 2, 0});

            //const Rectangle base = {x - 1, y - 1, static_cast<float>(df.width), static_cast<float>(df.height)};
            //df.df_set_area(TILE_STONE_WALL_01, TILE_STONE_WALL_01, base);
            // draw room 1
            //const Rectangle room1 = {base.x + 1, base.y + 1, w, h};
            //df.df_set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, room1);
            // draw room 2
            //const Rectangle room2 = {room1.x + room1.width + 1, room1.y, w, h};
            //df.df_set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, room2);
            // poke a hole
            //const Rectangle entryway1 = {room2.x - 1, room2.y + 1, 1, 1};
            //df.df_set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, entryway1);
            // draw room 3
            //Rectangle room3 = {room2.x, room2.y + room2.height + 1, w, h};
            //df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, room3);
            // poke a hole in the wall
            //Rectangle entryway2 = {room3.x + 1, room3.y - 1, 1, 1};
            //df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, entryway2);
            // draw room 4
            //Rectangle room4 = {room1.x, room1.y + room1.height + 1, w, h};
            //df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, room4);
            // poke a hole in the wall
            //Rectangle entryway3 = {room3.x - 1, room3.y + 1, 1, 1};
            //df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, entryway3);
            // set upstairs
            //const vec3 loc_u = {static_cast<int>(room1.x), static_cast<int>(room1.y), df.floor};
            //df.df_set_tile(TILE_UPSTAIRS, loc_u.x, loc_u.y);
            //df.upstairs_loc = loc_u;
            // set downstairs
            //const vec3 loc_d = {static_cast<int>(room1.x + 1), static_cast<int>(room1.y + 1), df.floor};
            //df.df_set_tile(TILE_DOWNSTAIRS, loc_d.x, loc_d.y);
            //df.downstairs_loc = loc_d;
            // automatic door placement
            for (int x = 0; x < df->get_width(); x++)
            {
                for (int y = 0; y < df->get_height(); y++)
                {
                    if (df->df_is_good_door_loc((vec3){x, y, df->get_floor()}))
                    {
                        df->df_set_can_have_door((vec3){x, y, df->get_floor()});
                    }
                }
            }
        };

        df->df_xform(creation_rules);
        floors.push_back(df);
    }
};
