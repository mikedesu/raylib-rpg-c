#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "dungeon_tile_type.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

dungeon_floor_t* dungeon_floor_create(const int width, const int height) {
    dungeon_floor_t* floor = malloc(sizeof(dungeon_floor_t));
    if (!floor) {
        merror("dungeon_floor_create: floor malloc failed");
        return NULL;
    }
    df_init(floor);
    return floor;
}

void df_set_tile_area(dungeon_floor_t* const df, dungeon_tile_type_t type, int x, int y, int w, int h) {
    if (!df) return;
    for (int i = x; i < df->height; i++) {
        for (int j = y; j < df->width; j++) {
            //dungeon_tile_t* current = &df->tiles[i][j];
            dungeon_tile_t* current = dungeon_floor_tile_at(df, j, i);
            //dungeon_tile_type_t type = DUNGEON_TILE_TYPE_FLOOR_STONE_00 + (rand() % 12);
            dungeon_tile_init(current, type);
        }
    }
}

void df_init(dungeon_floor_t* df) {
    if (!df) return;
    df->width = DEFAULT_DUNGEON_FLOOR_WIDTH;
    df->height = DEFAULT_DUNGEON_FLOOR_HEIGHT;
    df->tiles = malloc(sizeof(dungeon_tile_t*) * df->height);
    if (!df->tiles) return;

    // memset the tiles
    memset(df->tiles, 0, sizeof(dungeon_tile_t*) * df->height);

    for (int i = 0; i < df->height; i++) {
        df->tiles[i] = malloc(sizeof(dungeon_tile_t) * df->width);
        if (df->tiles[i] == NULL) {
            // malloc failed and we need to free everything up to this point
            for (int j = 0; j < i; j++) { free(df->tiles[j]); }
            return;
        }
    }
    for (int i = 0; i < df->height; i++) {
        for (int j = 0; j < df->width; j++) {
            dungeon_tile_t* current = &df->tiles[i][j];
            dungeon_tile_type_t type = DUNGEON_TILE_TYPE_STONE_WALL_00;
            type = DUNGEON_TILE_TYPE_FLOOR_STONE_00 + (rand() % 11);
            dungeon_tile_init(current, type);
        }
    }
    // lets do an experiment
    // i want to set a perimeter given an x,y and a width and height
    // i want to set the perimeter to be a wall
    df_set_tile_perimeter(df, DUNGEON_TILE_TYPE_STONE_WALL_00, 0, 0, 14, 5);
    df_set_tile(df, DUNGEON_TILE_TYPE_FLOOR_STONE_TRAP_ON_00, 2, 5);
}

void df_set_tile(dungeon_floor_t* const df, dungeon_tile_type_t type, int x, int y) {
    if (!df) return;
    dungeon_tile_t* current = &df->tiles[y][x];
    dungeon_tile_init(current, type);
}

void df_set_tile_perimeter(dungeon_floor_t* const df, dungeon_tile_type_t type, int x, int y, int w, int h) {
    if (!df) return;
    for (int i = 0; i <= w; i++) {
        dungeon_tile_t* current = &df->tiles[y][x + i];
        dungeon_tile_init(current, type);
    }
    for (int i = 0; i <= h; i++) {
        dungeon_tile_t* current = &df->tiles[y + i][x];
        dungeon_tile_init(current, type);
    }
    for (int i = 0; i <= w; i++) {
        dungeon_tile_t* current = &df->tiles[y + h][x + i];
        dungeon_tile_init(current, type);
    }
    for (int i = 0; i <= h; i++) {
        dungeon_tile_t* current = &df->tiles[y + i][x + w];
        dungeon_tile_init(current, type);
    }
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

bool dungeon_floor_add_at(dungeon_floor_t* const df, entityid id, int x, int y) {
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

bool dungeon_floor_remove_at(dungeon_floor_t* const df, entityid id, int x, int y) {
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

void dungeon_floor_set_pressure_plate(dungeon_floor_t* const df,
                                      const int x,
                                      const int y,
                                      const int up_tx_key,
                                      const int dn_tx_key,
                                      const int event) {
    if (!df || x < 0 || x >= df->width || y < 0 || y >= df->height) return;
    dungeon_tile_set_pressure_plate(&df->tiles[y][x], true);
    dungeon_tile_set_pressure_plate_up_tx_key(&df->tiles[y][x], up_tx_key);
    dungeon_tile_set_pressure_plate_down_tx_key(&df->tiles[y][x], dn_tx_key);
    dungeon_tile_set_pressure_plate_event(&df->tiles[y][x], event);
}

void dungeon_floor_set_wall_switch(dungeon_floor_t* const df, int x, int y, int up_key, int dn_key, int event) {
    if (!df || x < 0 || x >= df->width || y < 0 || y >= df->height) return;
    dungeon_tile_set_wall_switch(&df->tiles[y][x], true);
    dungeon_tile_set_wall_switch_up_tx_key(&df->tiles[y][x], up_key);
    dungeon_tile_set_wall_switch_down_tx_key(&df->tiles[y][x], dn_key);
    dungeon_tile_set_wall_switch_event(&df->tiles[y][x], event);
}

void df_set_all_tiles(dungeon_floor_t* const df, dungeon_tile_type_t type) {
    if (!df) return;
    // rewrite this function using df_set_tile_area
    df_set_tile_area(df, type, 0, 0, df->width, df->height);
}

bool df_tile_is_wall(const dungeon_floor_t* const df, int x, int y) {
    if (!df) return false;
    dungeon_tile_t* tile = dungeon_floor_tile_at(df, x, y);
    return dungeon_tile_is_wall(tile->type);
}
