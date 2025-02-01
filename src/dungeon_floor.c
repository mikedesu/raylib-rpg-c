#include "dungeon_floor.h"
//#include "dungeon_tile_type.h"



void dungeon_floor_init(dungeon_floor_t* floor) {
    floor->width = DEFAULT_DUNGEON_FLOOR_WIDTH;
    floor->height = DEFAULT_DUNGEON_FLOOR_HEIGHT;

    for (int i = 0; i < floor->width; i++) {
        for (int j = 0; j < floor->height; j++) {
            dungeon_tile_t* current = &floor->tiles[i][j];
            dungeon_tile_init(current, DUNGEON_TILE_TYPE_FLOOR_DIRT);
        }
    }
}
