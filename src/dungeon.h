#pragma once

#include "dungeon_floor.h"
#include "dungeon_tile_type.h"
#include "dungeonfloorid.h"
#include "room.h"
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




    inline shared_ptr<dungeon_floor> create_floor(biome_t type, int width, int height)
    {
        massert(width > 0, "width is 0");
        massert(height > 0, "height is 0");
        massert(!is_locked, "dungeon is locked");
        shared_ptr<dungeon_floor> df = make_shared<dungeon_floor>();
        df->init(floors.size(), type, width, height);
        return df;
    }




    inline void add_floor(shared_ptr<dungeon_floor> df)
    {
        floors.push_back(df);
    }
};
