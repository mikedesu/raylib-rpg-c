#pragma once

#include "dungeon_floor.h"
#include "dungeonfloorid.h"

#include <functional>
#include <memory>
#include <vector>

using std::function;
using std::make_shared;
using std::shared_ptr;
using std::vector;

#define INITIAL_DUNGEON_CAPACITY 4

typedef struct {
    shared_ptr<vector<shared_ptr<dungeon_floor_t>>> floors; // vector of shared pointers to dungeon_floor_t
    int current_floor;
    bool is_locked;
} dungeon_t;


static inline shared_ptr<dungeon_t> d_create() {
    auto dungeon = make_shared<dungeon_t>();
    if (!dungeon)
        return nullptr;
    dungeon->floors = make_shared<vector<shared_ptr<dungeon_floor_t>>>();
    if (!dungeon->floors)
        return nullptr;
    dungeon->current_floor = 0;
    dungeon->is_locked = false;
    return dungeon;
}


static inline shared_ptr<dungeon_floor_t> d_add_floor(shared_ptr<dungeon_t> dungeon, dungeon_floor_type_t type, int width, int height) {
    if (!dungeon || width <= 0 || height <= 0 || dungeon->is_locked)
        return nullptr;
    const int current_floor = dungeon->floors->size();

    auto df = df_init(current_floor, type, width, height);
    if (!df)
        return nullptr;
    auto creation_rules = [df]() {
        const float x = df->width / 3.0;
        const float y = df->height / 3.0;
        const int w = 8;
        const int h = 8;
        df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, (Rectangle){x, y, w, h});
        df_set_perimeter(df, TILE_STONE_WALL_01, TILE_STONE_WALL_01, (Rectangle){x - 1, y - 1, w + 2, h + 2});
        df_set_area(df, TILE_STONE_WALL_01, TILE_STONE_WALL_01, (Rectangle){x + w / 2.0f, y, 1, h});
        df_set_area(df, TILE_STONE_WALL_01, TILE_STONE_WALL_01, (Rectangle){x, y + h / 2.0f, w, 1});
        df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w / 2.0f, y + 1, 1, 1});
        df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w / 2.0f + 2, y + 4, 1, 2});
        df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w / 2.0f, y + 6, 1, 2});
        const vec3 loc_u = {9, 9, df->floor};
        df_set_tile(df, TILE_UPSTAIRS, loc_u.x, loc_u.y);
        df->upstairs_loc = loc_u;
        const vec3 loc_d = {9, 14, df->floor};
        df_set_tile(df, TILE_DOWNSTAIRS, loc_d.x, loc_d.y);
        df->downstairs_loc = loc_d;
    };
    df_xform(creation_rules);
    dungeon->floors->push_back(df);
    return df;
}


static inline void d_destroy(shared_ptr<dungeon_t> d) {
    if (d && d->floors)
        d->floors->clear();
}


static inline void d_free(shared_ptr<dungeon_t> dungeon) {
    massert(dungeon, "dungeon is NULL");
    d_destroy(dungeon);
}


static inline shared_ptr<dungeon_floor_t> d_get_floor(shared_ptr<dungeon_t> dungeon, size_t index) {
    return !dungeon || index < 0 || index >= dungeon->floors->size() ? nullptr : dungeon->floors->at(index);
}

static inline shared_ptr<dungeon_floor_t> d_get_current_floor(shared_ptr<dungeon_t> dungeon) {
    return !dungeon ? nullptr : d_get_floor(dungeon, dungeon->current_floor);
}
