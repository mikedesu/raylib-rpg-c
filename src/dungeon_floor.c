#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "mprint.h"
#include <stdlib.h>


dungeon_floor_t* dungeon_floor_create(const int width, const int height) {
    dungeon_floor_t* floor = malloc(sizeof(dungeon_floor_t));
    if (!floor) {
        merror("dungeon_floor_create: floor malloc failed");
        return NULL;
    }
    dungeon_floor_init(floor);
    return floor;
}


void dungeon_floor_init(dungeon_floor_t* floor) {
    if (!floor) {
        merror("dungeon_floor_init: floor is NULL");
        return;
    }
    floor->width = DEFAULT_DUNGEON_FLOOR_WIDTH;
    floor->height = DEFAULT_DUNGEON_FLOOR_HEIGHT;
    floor->tiles = (dungeon_tile_t**)malloc(sizeof(dungeon_tile_t*) * floor->height);
    if (!floor->tiles) {
        merror("dungeon_floor_init: floor->tiles malloc failed");
        return;
    }
    bool success = true;
    //for (int i = 0; i < floor->width; i++) {
    for (int i = 0; i < floor->height; i++) {
        floor->tiles[i] = (dungeon_tile_t*)malloc(sizeof(dungeon_tile_t) * floor->width);
        if (floor->tiles[i] == NULL) {
            success = false;
            break;
        }
    }
    if (!success) {
        merror("dungeon_floor_init: floor->tiles[i] malloc failed");
        return;
    }
    for (int i = 0; i < floor->height; i++) {
        for (int j = 0; j < floor->width; j++) {
            dungeon_tile_t* current = &floor->tiles[i][j];
            dungeon_tile_type_t type = DUNGEON_TILE_TYPE_STONE_WALL_00;
            if (i == 0 || i == floor->height - 1 || j == 0 || j == floor->width - 1) {
                //type = DUNGEON_TILE_TYPE_NONE;

                if (j % 2 == 0) {
                    type = DUNGEON_TILE_TYPE_STONE_WALL_00;
                } else if (j % 3 == 0) {
                    type = DUNGEON_TILE_TYPE_STONE_WALL_01;
                } else {
                    type = DUNGEON_TILE_TYPE_STONE_WALL_02;
                }
            } else {
                //type = DUNGEON_TILE_TYPE_FLOOR_DIRT;

                type = DUNGEON_TILE_TYPE_FLOOR_STONE_00 + (rand() % 12);
                //type = DUNGEON_TILE_TYPE_FLOOR_STONE_00;
                // randomly generate a value between 0 and 12 inclusive


                //dungeon_tile_init(current, DUNGEON_TILE_TYPE_FLOOR_DIRT);
            }
            dungeon_tile_init(current, type);
        }
    }
    // lets set the corners to NONE
    //dungeon_tile_init(&floor->tiles[0][0], DUNGEON_TILE_TYPE_NONE);
    //dungeon_tile_init(&floor->tiles[0][floor->width - 1], DUNGEON_TILE_TYPE_NONE);
    //dungeon_tile_init(&floor->tiles[floor->height - 1][0], DUNGEON_TILE_TYPE_NONE);
    //dungeon_tile_init(&floor->tiles[floor->height - 1][floor->width - 1], DUNGEON_TILE_TYPE_NONE);
}


void dungeon_floor_free(dungeon_floor_t* floor) {
    minfo("Freeing dungeon floor");
    if (!floor) {
        merror("dungeon_floor_free: floor is NULL");
        return;
    }
    for (int i = 0; i < floor->height; i++) {
        dungeon_tile_t* current = floor->tiles[i];
        free(current);
    }
    free(floor);
    msuccess("Freed dungeon floor");
}


const bool dungeon_floor_add_at(dungeon_floor_t* const df, const entityid id, const int x, const int y) {
    bool retval = false;
    if (!df) {
        merror("dungeon_floor_add_at: df is NULL");
        return false;
    }
    if (id == -1) {
        merror("dungeon_floor_add_at: id is -1");
        return false;
    }
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        merror("dungeon_floor_add_at: x or y out of bounds");
        return false;
    }
    retval = dungeon_tile_add(&df->tiles[y][x], id) != -1;
    msuccessint3("dungeon_floor_add_at: added entity", id, x, y);
    return retval;
}


const bool dungeon_floor_remove_at(dungeon_floor_t* const df, const entityid id, const int x, const int y) {
    //bool retval = false;
    if (!df) {
        merror("dungeon_floor_remove_at: df is NULL");
        return false;
    }
    if (id == -1) {
        merror("dungeon_floor_remove_at: id is -1");
        return false;
    }
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        merror("dungeon_floor_remove_at: x or y out of bounds");
        return false;
    }
    const entityid r = dungeon_tile_remove(&df->tiles[y][x], id);
    msuccessint3("dungeon_floor_remove_at: added entity", id, x, y);
    return r != -1 && r == id;
}


dungeon_tile_t* dungeon_floor_tile_at(const dungeon_floor_t* const df, const int x, const int y) {
    if (!df) {
        merror("dungeon_floor_tile_at: df is NULL");
        return NULL;
    }
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        merror("dungeon_floor_tile_at: x or y out of bounds");
        return NULL;
    }
    return &df->tiles[y][x];
}


//void dungeon_floor_set_pressure_plate(dungeon_floor_t* const df,
//                                      const int x,
//                                      const int y,
//                                      const int texture_key,
//                                      const int event) {
//    if (!df) {
//        merror("dungeon_floor_set_pressure_plate: df is NULL");
//        return;
//    }
//    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
//        merror("dungeon_floor_set_pressure_plate: x or y out of bounds");
//        return;
//    }
//    dungeon_tile_set_pressure_plate(&df->tiles[y][x], true);
//    dungeon_tile_set_pressure_plate_texture_key(&df->tiles[y][x], texture_key);
//    dungeon_tile_set_pressure_plate_event(&df->tiles[y][x], event);
//}


void dungeon_floor_set_pressure_plate(dungeon_floor_t* const df,
                                      const int x,
                                      const int y,
                                      const int up_tx_key,
                                      const int dn_tx_key,
                                      const int event) {
    if (!df) {
        merror("dungeon_floor_set_pressure_plate: df is NULL");
        return;
    }
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        merror("dungeon_floor_set_pressure_plate: x or y out of bounds");
        return;
    }
    dungeon_tile_set_pressure_plate(&df->tiles[y][x], true);
    dungeon_tile_set_pressure_plate_up_tx_key(&df->tiles[y][x], up_tx_key);
    dungeon_tile_set_pressure_plate_down_tx_key(&df->tiles[y][x], dn_tx_key);
    dungeon_tile_set_pressure_plate_event(&df->tiles[y][x], event);
}
