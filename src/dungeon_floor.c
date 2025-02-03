#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "mprint.h"
#include <stdlib.h>


dungeon_floor_t* dungeon_floor_create(int width, int height) {
    dungeon_floor_t* floor = (dungeon_floor_t*)malloc(sizeof(dungeon_floor_t));
    if (floor == NULL) {
        merror("dungeon_floor_create: floor malloc failed");
        return NULL;
    }

    dungeon_floor_init(floor);

    return floor;
}




void dungeon_floor_init(dungeon_floor_t* floor) {
    floor->width = DEFAULT_DUNGEON_FLOOR_WIDTH;
    floor->height = DEFAULT_DUNGEON_FLOOR_HEIGHT;

    floor->tiles = (dungeon_tile_t**)malloc(sizeof(dungeon_tile_t*) * floor->width);
    if (floor->tiles == NULL) {
        merror("dungeon_floor_init: floor->tiles malloc failed");
        return;
    }

    for (int i = 0; i < floor->width; i++) {
        floor->tiles[i] = (dungeon_tile_t*)malloc(sizeof(dungeon_tile_t) * floor->height);
        if (floor->tiles[i] == NULL) {
            merror("dungeon_floor_init: floor->tiles[i] malloc failed");
            return;
        }
    }

    for (int i = 0; i < floor->width; i++) {
        for (int j = 0; j < floor->height; j++) {
            dungeon_tile_t* current = &floor->tiles[i][j];
            dungeon_tile_init(current, DUNGEON_TILE_TYPE_FLOOR_DIRT);
        }
    }
}




void dungeon_floor_free(dungeon_floor_t* floor) {
    if (floor) {
        for (int i = 0; i < floor->width; i++) {
            dungeon_tile_t* current = floor->tiles[i];
            free(current);
        }
        free(floor);
    }
}




const bool dungeon_floor_add_at(dungeon_floor_t* const df, const entityid id, const int x, const int y) {
    if (df) {
        if (id != -1) {
            if (x >= 0 && x < df->width && y >= 0 && y < df->height) {
                dungeon_tile_add(&df->tiles[x][y], id);
                return true;
            } else {
                merror("dungeon_floor_add_at: x or y out of bounds");
            }
        } else {
            merror("dungeon_floor_add_at: id is -1");
        }
    } else {
        merror("dungeon_floor_add_at: df is NULL");
    }
    return false;
}
