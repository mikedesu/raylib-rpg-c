#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "dungeon_tile_type.h"
//#include "libgame_defines.h"
#include "libgame_defines.h"
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

void df_set_tile_area(dungeon_floor_t* const df, tiletype_t type, int x, int y, int w, int h) {
    if (!df) return;
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            //dungeon_tile_t* current = &df->tiles[i][j];
            dungeon_tile_t* current = dungeon_floor_tile_at(df, j, i);
            //tiletype_t type = TILE_FLOOR_STONE_00 + (rand() % 12);
            dungeon_tile_init(current, type);
        }
    }
}

void df_init(dungeon_floor_t* df) {
    if (!df) return;
    df->width = DEFAULT_DUNGEON_FLOOR_WIDTH;
    df->height = DEFAULT_DUNGEON_FLOOR_HEIGHT;
    df_reset_plates(df);
    df_reset_events(df);
    if (!df_malloc_tiles(df)) {
        merror("df_init: failed to malloc tiles");
        return;
    }
    df_set_all_tiles(df, TILE_NONE);
    // at this point, we are free to customize the dungeon floor to our liking
    df_init_test(df);
}

void df_set_event(dungeon_floor_t* const df, int x, int y, int event_id, tiletype_t on_type, tiletype_t off_type) {
    if (!df) return;
    if (event_id < 0 || event_id >= DEFAULT_DF_EVENTS) {
        merrorint("df_set_event: event_id is out of bounds", event_id);
        return;
    }
    df->events[event_id].x = x;
    df->events[event_id].y = y;
    df->events[event_id].on_type = on_type;
    df->events[event_id].off_type = off_type;
    df->events[event_id].listen_event = event_id;
}

void df_init_test(dungeon_floor_t* df) {
    // Rooms
    df_init_rect(df, 0, 0, 4, 4, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_init_rect(df, 0, 5, 4, 4, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_init_rect(df, 5, 0, 4, 4, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_init_rect(df, 5, 5, 4, 4, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    // Trap + Switch
    df_create_trap_event(df, 1, 4, TILE_FLOOR_STONE_TRAP_ON_00, TILE_FLOOR_STONE_TRAP_OFF_00, 1);
    //df_create_trap_event(df, 1, 4, TILE_STONE_WALL_00, TILE_FLOOR_STONE_00, 1);
    df_place_wall_switch(df, 0, 0, TX_WALL_SWITCH_UP_00, TX_WALL_SWITCH_DOWN_00, 1);
}

void df_set_tile(dungeon_floor_t* const df, tiletype_t type, int x, int y) {
    if (!df) return;
    dungeon_tile_t* current = &df->tiles[y][x];
    dungeon_tile_init(current, type);
}

void df_set_tile_perimeter(dungeon_floor_t* const df, tiletype_t type, int x, int y, int w, int h) {
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

void df_set_tile_perimeter_range(dungeon_floor_t* const df,
                                 tiletype_t begin,
                                 tiletype_t end,
                                 int x,
                                 int y,
                                 int w,
                                 int h) {
    if (!df) return;
    tiletype_t begin_type = begin;
    tiletype_t end_type = end;
    if (end_type < begin_type) {
        tiletype_t temp = begin_type;
        begin_type = end_type;
        end_type = temp;
    }
    int num_types = end_type - begin_type + 1;

    dungeon_tile_t* t = NULL;
    tiletype_t type = TILE_NONE;

    for (int i = 0; i <= w; i++) {
        t = dungeon_floor_tile_at(df, x + i, y);
        type = begin_type + (rand() % num_types);
        dungeon_tile_init(t, type);

        t = dungeon_floor_tile_at(df, x + i, y + h);
        type = begin_type + (rand() % num_types);
        dungeon_tile_init(t, type);
    }
    for (int i = 0; i <= h; i++) {
        t = dungeon_floor_tile_at(df, x, y + i);
        type = begin_type + (rand() % num_types);
        dungeon_tile_init(t, type);

        t = dungeon_floor_tile_at(df, x + w, y + i);
        type = begin_type + (rand() % num_types);
        dungeon_tile_init(t, type);
    }
    //for (int i = 0; i <= w; i++) {
    //    t = dungeon_floor_tile_at(df, x + i, y + h);
    //    type = begin_type + (rand() % num_types);
    //    dungeon_tile_init(t, type);
    //}
    //for (int i = 0; i <= h; i++) {
    //    t = dungeon_floor_tile_at(df, x + w, y + i);
    //    type = begin_type + (rand() % num_types);
    //    dungeon_tile_init(t, type);
    //}
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
    dungeon_tile_t* const t = dungeon_floor_tile_at(df, x, y);
    if (!t) return;
    if (!dungeon_tile_is_wall(t->type)) return;
    dungeon_tile_set_wall_switch(t, true);
    dungeon_tile_set_wall_switch_up_tx_key(t, up_key);
    dungeon_tile_set_wall_switch_down_tx_key(t, dn_key);
    dungeon_tile_set_wall_switch_event(t, event);
}

void df_set_all_tiles(dungeon_floor_t* const df, tiletype_t type) {
    if (!df) return;
    // rewrite this function using df_set_tile_area
    df_set_tile_area(df, type, 0, 0, df->width, df->height);
}

void df_set_all_tiles_range(dungeon_floor_t* const df, tiletype_t begin, tiletype_t end) {
    if (!df) return;
    tiletype_t begin_type = begin;
    tiletype_t end_type = end;
    if (end_type < begin_type) {
        tiletype_t temp = begin_type;
        begin_type = end_type;
        end_type = temp;
    }
    int num_types = end_type - begin_type + 1;
    for (int i = 0; i < df->height; i++) {
        for (int j = 0; j < df->width; j++) {
            dungeon_tile_t* current = dungeon_floor_tile_at(df, j, i);
            tiletype_t type = begin_type + (rand() % num_types);
            dungeon_tile_init(current, type);
        }
    }
}

bool df_tile_is_wall(const dungeon_floor_t* const df, int x, int y) {
    if (!df) return false;
    dungeon_tile_t* tile = dungeon_floor_tile_at(df, x, y);
    return dungeon_tile_is_wall(tile->type);
}

void df_set_tile_area_range(dungeon_floor_t* const df, int x, int y, int w, int h, tiletype_t begin, tiletype_t end) {
    if (!df) return;
    tiletype_t begin_type = begin;
    tiletype_t end_type = end;
    if (end_type < begin_type) {
        tiletype_t temp = begin_type;
        begin_type = end_type;
        end_type = temp;
    }
    int num_types = end_type - begin_type + 1;
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            dungeon_tile_t* current = dungeon_floor_tile_at(df, j, i);
            tiletype_t type = begin_type + (rand() % num_types);
            dungeon_tile_init(current, type);
        }
    }
}

void df_reset_plates(dungeon_floor_t* const df) {
    if (!df) return;
    for (int i = 0; i < DEFAULT_DF_PLATES; i++) df->plates[i] = false;
}

void df_reset_events(dungeon_floor_t* const df) {
    if (!df) return;
    for (int i = 0; i < DEFAULT_DF_EVENTS; i++) {
        df->events[i].listen_event = -1;
        df->events[i].x = -1;
        df->events[i].y = -1;
        df->events[i].on_type = TILE_NONE;
        df->events[i].off_type = TILE_NONE;
    }
}

bool df_malloc_tiles(dungeon_floor_t* const df) {
    if (!df) return false;
    df->tiles = malloc(sizeof(dungeon_tile_t*) * df->height);
    if (!df->tiles) return false;
    // memset the tiles
    memset(df->tiles, 0, sizeof(dungeon_tile_t*) * df->height);
    for (int i = 0; i < df->height; i++) {
        df->tiles[i] = malloc(sizeof(dungeon_tile_t) * df->width);
        if (df->tiles[i] == NULL) {
            // malloc failed and we need to free everything up to this point
            for (int j = 0; j < i; j++) { free(df->tiles[j]); }
            return false;
        }
    }
    return true;
}

void df_init_rect(dungeon_floor_t* df, int x, int y, int w, int h, tiletype_t t1, tiletype_t t2) {
    if (!df || w <= 0 || h <= 0) return;
    df_set_tile_area_range(df, x, y, w, h, t1, t2);
}

void df_create_trap_event(dungeon_floor_t* df, int x, int y, tiletype_t on, tiletype_t off, df_event_id id) {
    if (!df || id >= DEFAULT_DF_EVENTS) return;
    df->events[id] = (df_event_t){.listen_event = id, .x = x, .y = y, .on_type = on, .off_type = off};
    df_set_tile(df, on, x, y);
}

void df_place_wall_switch(dungeon_floor_t* df, int x, int y, int up, int down, df_event_id trigger_id) {
    if (!df) return;
    df_set_tile(df, TILE_STONE_WALL_02, x, y);
    dungeon_floor_set_wall_switch(df, x, y, up, down, trigger_id);
}
