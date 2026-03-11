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

class dungeon {
public:
    vector<shared_ptr<dungeon_floor>> floors; // vector of shared pointers to dungeon_floor_t
    int current_floor;
    bool is_locked;
    bool is_initialized;

    dungeon() {
        current_floor = 0;
        is_locked = false;
        is_initialized = false;
    }

    ~dungeon() {
        floors.clear();
    }

    size_t get_floor_count() {
        return floors.size();
    }

    shared_ptr<dungeon_floor> get_floor(const size_t index) {
        massert(index >= 0 && index < floors.size(), "index is OOB: index is %ld", index);
        return floors.at(index);
    }

    shared_ptr<dungeon_floor> get_current_floor() {
        return get_floor(current_floor);
    }

    shared_ptr<dungeon_floor> create_floor(biome_t type, int width = DEFAULT_DUNGEON_FLOOR_WIDTH, int height = DEFAULT_DUNGEON_FLOOR_HEIGHT) {
        massert(!is_locked, "dungeon is locked");
        shared_ptr<dungeon_floor> df = make_shared<dungeon_floor>(width, height);
        df->init(floors.size(), type);
        return df;
    }

    void add_floor(shared_ptr<dungeon_floor> df) {
        floors.push_back(df);
    }
};
