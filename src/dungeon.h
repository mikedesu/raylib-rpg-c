#pragma once

#include "dungeon_floor.h"
#include "dungeon_tile_type.h"
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


//static inline shared_ptr<dungeon_t> d_create() {
static inline void d_create(dungeon_t& dungeon) {
    //auto dungeon = make_shared<dungeon_t>();
    //if (!dungeon)
    //    return nullptr;
    dungeon.floors = make_shared<vector<shared_ptr<dungeon_floor_t>>>();
    if (!dungeon.floors)
        return;
    dungeon.current_floor = 0;
    dungeon.is_locked = false;
    //return dungeon;
}


//static inline shared_ptr<dungeon_floor_t> d_add_floor(shared_ptr<dungeon_t> dungeon, biome_t type, int width, int height) {
static inline void d_add_floor(dungeon_t& dungeon, biome_t type, int width, int height) {
    if (width <= 0 || height <= 0 || dungeon.is_locked)
        return;

    const int current_floor = dungeon.floors->size();
    auto df = df_init(current_floor, type, width, height);
    if (!df)
        return;

    auto creation_rules = [df]() {
        const float x = df->width / 4.0;
        const float y = df->height / 4.0;
        const int w = 4;
        const int h = 3;

        // create an wxh area
        //df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, (Rectangle){x, y, w, h});
        Rectangle base = {x - 1, y - 1, static_cast<float>(df->width), static_cast<float>(df->height)};
        df_set_area(df, TILE_STONE_WALL_01, TILE_STONE_WALL_01, base);
        // draw room 1
        Rectangle room1 = {base.x + 1, base.y + 1, w, h};
        df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, room1);
        // draw room 2
        Rectangle room2 = {room1.x + room1.width + 1, room1.y, w, h};
        df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, room2);
        // poke a hole
        Rectangle entryway1 = {room2.x - 1, room2.y + 1, 1, 1};
        df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, entryway1);
        // draw room 3
        Rectangle room3 = {room2.x, room2.y + room2.height + 1, w, h};
        df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, room3);
        // poke a hole in the wall
        Rectangle entryway2 = {room3.x + 1, room3.y - 1, 1, 1};
        df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, entryway2);
        // draw room 4
        Rectangle room4 = {room1.x, room1.y + room1.height + 1, w, h};
        df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, room4);
        // poke a hole in the wall
        Rectangle entryway3 = {room3.x - 1, room3.y + 1, 1, 1};
        df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, entryway3);


        //df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w / 2.0f, y + 1, 1, 1});
        // poke a hole in the wall
        //df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w / 2.0f + 2, y + 4, 1, 2});
        // poke a hole in the wall
        //df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w / 2.0f, y + 6, 1, 1});

        // set upstairs
        const vec3 loc_u = {static_cast<int>(room1.x), static_cast<int>(room1.y), df->floor};
        df_set_tile(df, TILE_UPSTAIRS, loc_u.x, loc_u.y);
        df->upstairs_loc = loc_u;

        // set downstairs
        const vec3 loc_d = {9, 14, df->floor};
        df_set_tile(df, TILE_DOWNSTAIRS, loc_d.x, loc_d.y);
        df->downstairs_loc = loc_d;

        // automatic door placement
        for (int x = 0; x < df->width; x++)
            for (int y = 0; y < df->height; y++)
                if (df_is_good_door_loc(df, (vec3){x, y, df->floor}))
                    df_set_can_have_door(df, (vec3){x, y, df->floor});
    };

    df_xform(creation_rules);
    dungeon.floors->push_back(df);
    //return df;
}


//static inline void d_destroy(shared_ptr<dungeon_t> d) {
static inline void d_destroy(dungeon_t& d) {
    if (d.floors)
        d.floors->clear();
}


//static inline void d_free(shared_ptr<dungeon_t> dungeon) {
//    massert(dungeon, "dungeon is NULL");
//    d_destroy(dungeon);
//}


//static inline shared_ptr<dungeon_floor_t> d_get_floor(shared_ptr<dungeon_t> dungeon, size_t index) {
static inline shared_ptr<dungeon_floor_t> d_get_floor(dungeon_t& dungeon, size_t index) {
    return index < 0 || index >= dungeon.floors->size() ? nullptr : dungeon.floors->at(index);
}

//static inline shared_ptr<dungeon_floor_t> d_get_current_floor(shared_ptr<dungeon_t> dungeon) {
static inline shared_ptr<dungeon_floor_t> d_get_current_floor(dungeon_t& dungeon) {
    return d_get_floor(dungeon, dungeon.current_floor);
}
