#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "mprint.h"
#include <stdlib.h>


//dungeon_floor_t* dungeon_floor_create(int width, int height) {
//    dungeon_floor_t* floor = (dungeon_floor_t*)malloc(sizeof(dungeon_floor_t));
//    if (floor) {
//        dungeon_floor_init(floor);
//    } else {
//        merror("dungeon_floor_create: floor malloc failed");
//        floor = NULL;
//    }
//    return floor;
//}


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
    /*
    if (floor) {
        floor->width = DEFAULT_DUNGEON_FLOOR_WIDTH;
        floor->height = DEFAULT_DUNGEON_FLOOR_HEIGHT;
        floor->tiles = (dungeon_tile_t**)malloc(sizeof(dungeon_tile_t*) * floor->height);

        if (floor->tiles) {
            bool success = true;
            for (int i = 0; i < floor->width; i++) {
                floor->tiles[i] = (dungeon_tile_t*)malloc(sizeof(dungeon_tile_t) * floor->width);
                if (floor->tiles[i] == NULL) {
                    success = false;
                    break;
                }
            }

            if (success) {
                for (int i = 0; i < floor->height; i++) {
                    for (int j = 0; j < floor->width; j++) {
                        dungeon_tile_t* current = &floor->tiles[i][j];
                        // set the perimeter to stone walls
                        if (i == 0 || i == floor->height - 1 || j == 0 || j == floor->width - 1) {
                            if (j % 2 == 0) {
                                dungeon_tile_init(current, DUNGEON_TILE_TYPE_STONE_WALL_00);
                            } else if (j % 3 == 0) {
                                dungeon_tile_init(current, DUNGEON_TILE_TYPE_STONE_WALL_02);
                            } else {
                                dungeon_tile_init(current, DUNGEON_TILE_TYPE_STONE_WALL_01);
                            }
                        } else {
                            dungeon_tile_init(current, DUNGEON_TILE_TYPE_FLOOR_DIRT);
                        }
                    }
                }
            } else {
                merror("dungeon_floor_init: floor->tiles[i] malloc failed");
            }
        } else {
            merror("dungeon_floor_init: floor->tiles malloc failed");
        }
    } else {
        merror("dungeon_floor_init: floor is NULL");
    }

    */


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
    for (int i = 0; i < floor->width; i++) {
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
            // set the perimeter to stone walls
            if (i == 0 || i == floor->height - 1 || j == 0 || j == floor->width - 1) {
                if (j % 2 == 0) {
                    dungeon_tile_init(current, DUNGEON_TILE_TYPE_STONE_WALL_00);
                } else if (j % 3 == 0) {
                    dungeon_tile_init(current, DUNGEON_TILE_TYPE_STONE_WALL_02);
                } else {
                    dungeon_tile_init(current, DUNGEON_TILE_TYPE_STONE_WALL_01);
                }
            } else {
                dungeon_tile_init(current, DUNGEON_TILE_TYPE_FLOOR_DIRT);
            }
        }
    }
}


void dungeon_floor_free(dungeon_floor_t* floor) {
    if (floor) {
        for (int i = 0; i < floor->height; i++) {
            dungeon_tile_t* current = floor->tiles[i];
            free(current);
        }
        free(floor);
    }
}


const bool dungeon_floor_add_at(dungeon_floor_t* const df, const entityid id, const int x, const int y) {
    bool retval = false;
    if (df) {
        if (id != -1) {
            if (x >= 0 && x < df->width && y >= 0 && y < df->height) {
                const int result = dungeon_tile_add(&df->tiles[y][x], id);
                if (result != -1) {
                    retval = true;
                }
            } else {
                merror("dungeon_floor_add_at: x or y out of bounds");
            }
        } else {
            merror("dungeon_floor_add_at: id is -1");
        }
    } else {
        merror("dungeon_floor_add_at: df is NULL");
    }
    msuccessint3("dungeon_floor_add_at: added entity", id, x, y);
    return retval;
}


const bool dungeon_floor_remove_at(dungeon_floor_t* const df, const entityid id, const int x, const int y) {
    bool retval = false;
    if (df) {
        if (id != -1) {
            if (x >= 0 && x < df->width && y >= 0 && y < df->height) {
                dungeon_tile_remove(&df->tiles[y][x], id);
                retval = true;
            } else {
                merror("dungeon_floor_remove_at: x or y out of bounds");
            }
        } else {
            merror("dungeon_floor_remove_at: id is -1");
        }
    } else {
        merror("dungeon_floor_remove_at: df is NULL");
    }
    msuccessint3("dungeon_floor_remove_at: added entity", id, x, y);
    return retval;
}


dungeon_tile_t* dungeon_floor_tile_at(dungeon_floor_t* const df, const int x, const int y) {
    if (df) {
        if (x >= 0 && x < df->width && y >= 0 && y < df->height) {
            return &df->tiles[y][x];
        } else {
            merror("dungeon_floor_tile_at: x or y out of bounds");
        }
    } else {
        merror("dungeon_floor_tile_at: df is NULL");
    }
    return NULL;
}
