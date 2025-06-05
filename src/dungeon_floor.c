#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "dungeon_tile_type.h"
#include "entityid.h"
#include "libgame_defines.h"
#include "massert.h"
//#include "mprint.h"
#include "range.h"
#include "raylib.h"
#include "vec3.h"
#include <stdlib.h>
#include <string.h>

static void df_create_trap_event(dungeon_floor_t* df, int x, int y, tiletype_t on, tiletype_t off, df_event_id id);
static void df_place_wall_switch(dungeon_floor_t* df, int x, int y, int up, int down, df_event_id trigger_id);
static void df_reset_plates(dungeon_floor_t* const df);
static void df_reset_events(dungeon_floor_t* const df);
static void df_set_event(dungeon_floor_t* const df, int x, int y, int event_id, tiletype_t on_type, tiletype_t off_type);
static void df_set_wall_switch(dungeon_floor_t* const df, int x, int y, int up_key, int dn_key, int event);
static void df_set_pressure_plate(dungeon_floor_t* const df, const int x, const int y, const int up_tx_key, const int dn_tx_key, const int event);
static void df_init_test(dungeon_floor_t* df);
static void df_set_tile_perimeter_range(dungeon_floor_t* const df, tiletype_t begin, tiletype_t end, int x, int y, int w, int h);
static void df_set_tile_perimeter(dungeon_floor_t* const df, tiletype_t type, int x, int y, int w, int h);
void df_set_tile(dungeon_floor_t* const df, tiletype_t type, int x, int y);
static void df_set_all_tiles_range(dungeon_floor_t* const df, tiletype_t begin, tiletype_t end);
static void df_set_all_tiles(dungeon_floor_t* const df, tiletype_t type);
static void df_set_tile_area(dungeon_floor_t* const df, tiletype_t type, int x, int y, int w, int h);
static void df_set_tile_area_range(dungeon_floor_t* const df, int x, int y, int w, int h, tiletype_t begin, tiletype_t end);
static void df_set_tile_area_range2(dungeon_floor_t* const df, Rectangle r, tiletype_t begin, tiletype_t end);
static void df_init_rect2(dungeon_floor_t* df, Rectangle r, tiletype_t t1, tiletype_t t2);
static void df_init_rect(dungeon_floor_t* df, int x, int y, int w, int h, tiletype_t t1, tiletype_t t2);
static bool df_malloc_tiles(dungeon_floor_t* const df);
static void df_init_test_complex4(dungeon_floor_t* df, int hallway_length);
static void df_init_test_complex5(dungeon_floor_t* df, range hallway_length);
static void df_init_test_complex6(dungeon_floor_t* df, range room_length, range room_width);
static void df_init_test_complex7(dungeon_floor_t* df, range room_width, range room_length);
static void df_init_test_complex8(dungeon_floor_t* df, int w, int h);
static void df_init_test_complex9(dungeon_floor_t* df, int grid_cell_w, int grid_cell_h);
static void df_assign_stairs(dungeon_floor_t* df);
bool df_add_room(dungeon_floor_t* const df, int x, int y, int w, int h, tiletype_t begin, tiletype_t end, const char* room_name);
static void df_assign_downstairs(dungeon_floor_t* df);
static void df_assign_upstairs(dungeon_floor_t* df);
bool df_assign_upstairs_in_area(dungeon_floor_t* df, int x, int y, int w, int h);
bool df_assign_downstairs_in_area(dungeon_floor_t* df, int x, int y, int w, int h);
static int df_get_possible_upstairs_count(dungeon_floor_t* df);
static int df_get_possible_downstairs_count(dungeon_floor_t* df);
static vec3* df_get_possible_upstairs_locs(dungeon_floor_t* df, int* external_count);
static vec3* df_get_possible_downstairs_locs(dungeon_floor_t* df, int* external_count);
static vec3* df_get_possible_upstairs_locs_in_area(dungeon_floor_t* df, int* external_count, int x, int y, int w, int h);
static vec3* df_get_possible_downstairs_locs_in_area(dungeon_floor_t* df, int* external_count, int x, int y, int w, int h);
static int df_get_possible_upstairs_count_in_area(dungeon_floor_t* df, int x, int y, int w, int h);
static int df_get_possible_downstairs_count_in_area(dungeon_floor_t* df, int x, int y, int w, int h);
static void df_make_room(dungeon_floor_t* df, int x, int y, int w, int h);
static void df_make_corridor_h(dungeon_floor_t* df, int x, int y, int len);
static void df_make_corridor_v(dungeon_floor_t* df, int x, int y, int len);
//static void df_connect_rooms(dungeon_floor_t* df, int x1, int y1, int x2, int y2);
static void df_make_diamond_shape_room(dungeon_floor_t* df, int cx, int cy, int w, int h, tiletype_t begin_type, tiletype_t end_type);

static int df_get_possible_downstairs_count_in_area(dungeon_floor_t* df, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    massert(w > 0, "w is less than zero");
    massert(h > 0, "h is less than zero");
    massert(x + w <= df->width, "x + w is out of bounds");
    massert(y + h <= df->height, "y + h is out of bounds");
    int count = 0;
    for (int y0 = y; y0 < df->height; y0++) {
        for (int x0 = x; x0 < df->width; x0++) {
            tile_t* const tile = df_tile_at(df, (vec3){x0, y0, -1});
            if (tile_is_possible_downstairs(tile->type)) count++;
        }
    }
    return count;
}

static int df_get_possible_upstairs_count_in_area(dungeon_floor_t* df, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    massert(w > 0, "w is less than zero");
    massert(h > 0, "h is less than zero");
    massert(x + w <= df->width, "x + w is out of bounds");
    massert(y + h <= df->height, "y + h is out of bounds");
    int count = 0;
    for (int y0 = y; y0 < df->height; y0++) {
        for (int x0 = x; x0 < df->width; x0++) {
            tile_t* const tile = df_tile_at(df, (vec3){x0, y0, -1});
            if (tile_is_possible_upstairs(tile->type)) count++;
        }
    }
    return count;
}

static void df_assign_stairs(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    df_assign_upstairs_in_area(df, 0, 0, df->width, df->height);
    df_assign_downstairs_in_area(df, 0, 0, df->width, df->height);
}

static int df_get_possible_upstairs_count(dungeon_floor_t* df) { return df_get_possible_upstairs_count_in_area(df, 0, 0, df->width, df->height); }

static int df_get_possible_downstairs_count(dungeon_floor_t* df) { return df_get_possible_downstairs_count_in_area(df, 0, 0, df->width, df->height); }

static vec3* df_get_possible_upstairs_locs(dungeon_floor_t* df, int* external_count) {
    massert(df, "dungeon floor is NULL");
    massert(external_count, "external_count is NULL");
    int count = -1;
    vec3* locs = df_get_possible_upstairs_locs_in_area(df, &count, 0, 0, df->width, df->height);
    massert(locs, "failed to get possible upstairs locations");
    massert(count > 0, "no possible upstairs locations");
    *external_count = count;
    return locs;
}

static vec3* df_get_possible_downstairs_locs(dungeon_floor_t* df, int* external_count) {
    massert(df, "dungeon floor is NULL");
    massert(external_count, "external_count is NULL");
    int count = -1;
    vec3* locs = df_get_possible_downstairs_locs_in_area(df, &count, 0, 0, df->width, df->height);
    massert(locs, "failed to get possible downstairs locations");
    massert(count > 0, "no possible downstairs locations");
    *external_count = count;
    return locs;
}

static vec3* df_get_possible_upstairs_locs_in_area(dungeon_floor_t* df, int* external_count, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    int count = df_get_possible_upstairs_count_in_area(df, x, y, w, h);
    vec3* locations = malloc(sizeof(vec3) * count);
    massert(locations, "failed to malloc locations");
    int count2 = 0;
    // now we can loop thru the dungeon floor again and fill the array with the locations
    for (int y0 = y; y0 < df->height; y0++) {
        for (int x0 = x; x0 < df->width; x0++) {
            tile_t* const tile = df_tile_at(df, (vec3){x0, y0, -1});
            if (tile_is_possible_upstairs(tile->type)) {
                // there wont be any entities yet so do not check for them
                // do not write an if statement
                locations[count2].x = x0;
                locations[count2].y = y0;
                count2++;
                massert(count2 <= count, "count2 is greater than count");
            }
        }
    }
    massert(count2 == count, "count2 is greater than count");
    *external_count = count;
    return locations;
}

static vec3* df_get_possible_downstairs_locs_in_area(dungeon_floor_t* df, int* external_count, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    massert(external_count, "external_count is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    int count = df_get_possible_downstairs_count_in_area(df, x, y, w, h);
    vec3* locations = malloc(sizeof(vec3) * count);
    massert(locations, "failed to malloc locations");
    int count2 = 0;
    // now we can loop thru the dungeon floor again and fill the array with the locations
    for (int y0 = y; y0 < df->height; y0++) {
        for (int x0 = x; x0 < df->width; x0++) {
            tile_t* const tile = df_tile_at(df, (vec3){x0, y0, -1});
            if (tile_is_possible_downstairs(tile->type)) {
                // there wont be any entities yet so do not check for them
                // do not write an if statement
                locations[count2].x = x0;
                locations[count2].y = y0;
                count2++;
                massert(count2 <= count, "count2 is greater than count");
            }
        }
    }
    massert(count2 == count, "count2 and count are unequal: count2=%d count=%d", count2, count);
    *external_count = count;
    return locations;
}

bool df_assign_upstairs_in_area(dungeon_floor_t* df, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    massert(w > 0, "w is less than zero");
    massert(h > 0, "h is less than zero");
    massert(x + w <= df->width, "x + w is out of bounds");
    massert(y + h <= df->height, "y + h is out of bounds");
    int count = -1;
    vec3* locations = df_get_possible_upstairs_locs_in_area(df, &count, x, y, w, h);
    massert(count > 0, "no possible upstairs locations");
    // now that we have a list of possible locations for the upstairs to appear
    // we can randomly select one of them
    int upstairs_index = rand() % count;
    vec3 up_loc = locations[upstairs_index];
    // now we can set the upstairs tile
    tile_t* const tile = df_tile_at(df, up_loc);
    massert(tile, "failed to get tile");
    tile_init(tile, TILE_UPSTAIRS);
    free(locations);
    df->upstairs_loc = up_loc;
    return true;
}

static void df_assign_upstairs(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    df_assign_upstairs_in_area(df, 0, 0, df->width, df->height);
}

static void df_assign_downstairs(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    df_assign_downstairs_in_area(df, 0, 0, df->width, df->height);
}

bool df_assign_downstairs_in_area(dungeon_floor_t* df, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    massert(w > 0, "w is less than zero");
    massert(h > 0, "h is less than zero");
    massert(x + w <= df->width, "x + w is out of bounds");
    massert(y + h <= df->height, "y + h is out of bounds");
    int count = -1;
    vec3* locations = df_get_possible_downstairs_locs_in_area(df, &count, x, y, w, h);
    // now that we have a list of possible locations for the upstairs to appear
    // we can randomly select one of them
    int downstairs_index = rand() % count;
    vec3 down_loc = locations[downstairs_index];
    // now we can set the upstairs tile
    tile_t* const tile = df_tile_at(df, down_loc);
    massert(tile, "failed to get tile");
    tile_init(tile, TILE_DOWNSTAIRS);
    free(locations);
    df->downstairs_loc = down_loc;
    return true;
}

int df_center_x(const dungeon_floor_t* const df) {
    massert(df, "dungeon floor is NULL");
    return df->width / 2;
}

int df_center_y(const dungeon_floor_t* const df) {
    massert(df, "dungeon floor is NULL");
    return df->height / 2;
}

//static void df_init_test_simple(dungeon_floor_t* df) {
//    massert(df, "dungeon floor is NULL");
//    int x = df->width / 2;
//    int y = df->height / 2;
//    int w = 8;
//    int h = 8;
//    tiletype_t begin_type = TILE_FLOOR_STONE_00;
//    tiletype_t end_type = TILE_FLOOR_STONE_11;
//    df_set_tile_area_range(df, x, y, w, h, begin_type, end_type);
//    df_assign_stairs(df);
//}

//static void df_init_test_simple2(dungeon_floor_t* df) {
//    massert(df, "dungeon floor is NULL");
//    int x = df_center_x(df), y = df_center_y(df);
//    int w = 4, h = 4;
//    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    df_assign_downstairs_in_area(df, x, y, w, h);
//    x += w + 1;
//    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    df_assign_upstairs_in_area(df, x, y, w, h);
//}

//static void df_init_test_simple3(dungeon_floor_t* df) {
//    massert(df, "dungeon floor is NULL");
//    int x = df_center_x(df), y = df_center_y(df);
//    int w = 4, h = 4;
//    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    df_assign_downstairs_in_area(df, x, y, w, h);
//    x += w + 1;
//    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    df_assign_upstairs_in_area(df, x, y, w, h);
//    x = df_center_x(df) + w;
//    y = df_center_y(df) + h / 2;
//    df_set_tile(df, TILE_FLOOR_STONE_00, x, y);
//}

//static void df_init_test_simple4(dungeon_floor_t* df) {
//    massert(df, "dungeon floor is NULL");
//    int x = df_center_x(df), y = df_center_y(df);
//    int w = 4, h = 4;
//    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    x += w + 1;
//    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    // now we need to attach a single tile to connect the rooms
//    // we will use the tile to the right of the first room
//    // we will set the tile to the right of the first room to be a stone floor 00
//    x = df_center_x(df) + w;
//    y = df_center_y(df) + h / 2;
//    df_set_tile(df, TILE_FLOOR_STONE_00, x, y);
//    // now we will create two tiles to do a 'trap'
//    // this involves
//    // 1. creating a wall tile
//    // 2. attaching a switch to the wall
//    // 3. creating a trap tile
//    // 4. attaching the trap to the switch
//    int id = 1;
//    int t_x = x, t_y = y;
//    x = df_center_x(df) + w, y = df_center_y(df);
//    int s_x = x + rand() % w, s_y = y + rand() % h;
//    df_place_wall_switch(df, s_x, s_y, TX_WALL_SWITCH_UP_00, TX_WALL_SWITCH_DOWN_00, id);
//    df_create_trap_event(df, t_x, t_y, TILE_FLOOR_STONE_TRAP_ON_00, TILE_FLOOR_STONE_TRAP_OFF_00, id);
//    df_set_tile(df, TILE_FLOOR_STONE_TRAP_ON_00, t_x, t_y);
//    df_assign_stairs(df);
//}

//static void df_init_test_simple5(dungeon_floor_t* df) {
//    massert(df, "dungeon floor is NULL");
//    int x = df_center_x(df), y = df_center_y(df);
//    int w = 4, h = 4;
//    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    df_assign_downstairs_in_area(df, x, y, w, h);
//    int separating_distance = 10;
//    x += w + separating_distance;
//    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    df_assign_upstairs_in_area(df, x, y, w, h);
//    x = df_center_x(df) + w;
//    y = df_center_y(df) + h / 2;
//    df_set_tile_area_range(df, x, y, separating_distance, 1, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//}

//static void df_init_test_simple6(dungeon_floor_t* df) {
//    massert(df, "dungeon floor is NULL");
//    int x = df_center_x(df), y = df_center_y(df);
//    int w = 4, h = 4;
//    int dist = 10;
//    // Left room with downstairs
//    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    df_assign_downstairs_in_area(df, x, y, w, h);
//    // Right room with upstairs
//    x += w + dist;
//    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    df_assign_upstairs_in_area(df, x, y, w, h);
//    // Horizontal hallway between rooms
//    x = df_center_x(df) + w;
//    y = df_center_y(df) + h / 2;
//    df_set_tile_area_range(df, x, y, dist, 1, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    // Vertical hallway centered on horizontal hallway
//    int vx = x + dist / 2;
//    int vy = y - dist / 2;
//    df_set_tile_area_range(df, vx, vy, 1, dist, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    // Top vertical room
//    df_set_tile_area_range(df, vx - w / 2, vy - h, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    // Bottom vertical room
//    int bot_y = vy + dist;
//    df_set_tile_area_range(df, vx - w / 2, bot_y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//}

//static void df_init_test_simple7(dungeon_floor_t* df) {
//    massert(df, "dungeon floor is NULL");
//    int cx = df_center_x(df), cy = df_center_y(df);
//    int w = 4, h = 4;
//    int gap = 3;
//    // Central big room
//    df_set_tile_area_range(df, cx - w, cy - h, w * 2, h * 2, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    df_assign_downstairs_in_area(df, cx - w, cy - h, w * 2, h * 2);
//    // Four small rooms on each side connected by single-tile doorways
//    // Top room
//    int tx = cx - w / 2, ty = cy - h * 2 - gap;
//    df_set_tile_area_range(df, tx, ty, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    df_set_tile(df, TILE_FLOOR_STONE_00, cx, ty + h); // connect door top-center
//    // Bottom room
//    int bx = cx - w / 2, by = cy + h * 2 + gap;
//    df_set_tile_area_range(df, bx, by, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    df_set_tile(df, TILE_FLOOR_STONE_00, cx, by - 1); // connect door bottom-center
//    // Left room
//    int lx = cx - w * 2 - gap, ly = cy - h / 2;
//    df_set_tile_area_range(df, lx, ly, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    df_set_tile(df, TILE_FLOOR_STONE_00, lx + w, cy); // connect door left-center
//    // Right room
//    int rx = cx + w * 2 + gap, ry = cy - h / 2;
//    df_set_tile_area_range(df, rx, ry, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    df_set_tile(df, TILE_FLOOR_STONE_00, rx - 1, cy); // connect door right-center
//    // Horizontal corridors to connect rooms to center
//    df_set_tile_area_range(df, lx + w, cy, cx - (lx + w), 1, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11); // left corridor
//    df_set_tile_area_range(df, cx + w, cy, rx - (cx + w), 1, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11); // right corridor
//    // Vertical corridors to connect rooms to center
//    df_set_tile_area_range(df, cx, ty + h, 1, cy - (ty + h), TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11); // top corridor
//    df_set_tile_area_range(df, cx, cy + h, 1, by - (cy + h), TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11); // bottom corridor
//    df_assign_upstairs_in_area(df, rx, ry, w, h);
//}

//static void df_init_test_simple8(dungeon_floor_t* df) {
//    massert(df, "dungeon floor is NULL");
//    int cx = df_center_x(df), cy = df_center_y(df);
//    int w = 4, h = 4;
//    int gap = 3;
// Central big room with downstairs
//    df_make_room(df, cx - w, cy - h, w * 2, h * 2);
//    df_assign_downstairs_in_area(df, cx - w, cy - h, w * 2, h * 2);
// Four small rooms
//    int tx = cx - w / 2, ty = cy - h * 2 - gap;
//    df_make_room(df, tx, ty, w, h);
//    df_connect_rooms(df, cx, cy - h, cx, ty + h);
//    int bx = cx - w / 2, by = cy + h * 2 + gap;
//    df_make_room(df, bx, by, w, h);
//    df_connect_rooms(df, cx, cy + h, cx, by - 1);
//    int lx = cx - w * 2 - gap, ly = cy - h / 2;
//    df_make_room(df, lx, ly, w, h);
//    df_connect_rooms(df, lx + w, cy, cx - w, cy);
//    int rx = cx + w * 2 + gap, ry = cy - h / 2;
//    df_make_room(df, rx, ry, w, h);
//    df_connect_rooms(df, rx - 1, cy, cx + w, cy);
// Corridors
//    df_make_corridor_h(df, lx + w, cy, (cx - (lx + w)));
//    df_make_corridor_h(df, cx + w, cy, (rx - (cx + w)));
//    df_make_corridor_v(df, cx, ty + h, (cy - (ty + h)));
//    df_make_corridor_v(df, cx, cy + h, (by - (cy + h)));
//    df_assign_upstairs_in_area(df, rx, ry, w, h);
//}

//dungeon_floor_t* df_create(const int width, const int height) {
dungeon_floor_t* df_create(int floor, int width, int height) {
    massert(width > 0, "width must be greater than zero");
    massert(height > 0, "height must be greater than zero");
    massert(floor >= 0, "floor must be greater than or equal to zero");
    dungeon_floor_t* df = malloc(sizeof(dungeon_floor_t));
    massert(df, "failed to malloc dungeon floor");
    df->floor = floor;
    df->width = width;
    df->height = height;
    df_init(df);
    return df;
}

static void df_set_tile_area(dungeon_floor_t* const df, tiletype_t type, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    massert(w > 0, "w is less than zero");
    massert(h > 0, "h is less than zero");
    massert(x + w <= df->width, "x + w is out of bounds");
    massert(y + h <= df->height, "y + h is out of bounds");
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            tile_t* current = df_tile_at(df, (vec3){j, i, -1});
            tile_init(current, type);
        }
    }
}

void df_init(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    df->rooms = NULL;
    df->room_count = 0;
    df->room_capacity = 0;
    //df->width = DEFAULT_DUNGEON_FLOOR_WIDTH;
    //df->height = DEFAULT_DUNGEON_FLOOR_HEIGHT;
    df->upstairs_loc = (vec3){-1, -1, -1};
    df->downstairs_loc = (vec3){-1, -1, -1};
    df_reset_plates(df);
    df_reset_events(df);
    if (!df_malloc_tiles(df)) {
        return;
    }
    df_init_rooms(df);
    df_set_all_tiles(df, TILE_NONE);
    // at this point, we are free to customize the dungeon floor to our liking
    //df_init_test_complex4(df, 3);
    //df_init_test_complex5(df, (range){3, 10});
    //df_init_test_complex6(df, (range){3, 10}, (range){3, 10});
    //df_init_test_complex7(df, (range){3, 5}, (range){3, 5});
    //int max_w = 16;
    //int max_h = 16;
    //int min_w = 4;
    //int min_h = 4;
    //int w = rand() % (max_w - min_w + 1) + min_w;
    //int h = rand() % (max_h - min_h + 1) + min_h;
    //df_init_test_complex8(df, w, h);
    //df_init_test_complex9(df, 4, 4);
}

static void df_set_event(dungeon_floor_t* const df, int x, int y, int event_id, tiletype_t on_type, tiletype_t off_type) {
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    massert(on_type >= 0, "on_type is out of bounds");
    massert(on_type < TILE_COUNT, "on_type is out of bounds");
    massert(off_type >= 0, "off_type is out of bounds");
    massert(off_type < TILE_COUNT, "off_type is out of bounds");
    massert(event_id >= 0, "event_id is less than zero");
    massert(event_id < DEFAULT_DF_EVENTS, "event_id is out of bounds");
    massert(df->events, "events is NULL");
    df->events[event_id].x = x;
    df->events[event_id].y = y;
    df->events[event_id].on_type = on_type;
    df->events[event_id].off_type = off_type;
    df->events[event_id].listen_event = event_id;
}

static void df_init_test(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    // Rooms
    int txwallup = TX_WALL_SWITCH_UP_00;
    int txwalldown = TX_WALL_SWITCH_DOWN_00;
    tiletype_t trap_on = TILE_FLOOR_STONE_TRAP_ON_00;
    tiletype_t trap_off = TILE_FLOOR_STONE_TRAP_OFF_00;
    tiletype_t begin_type = TILE_FLOOR_STONE_00;
    tiletype_t end_type = TILE_FLOOR_STONE_11;
    df_event_id id = 1;
    // First Room and Trap and Switch
    int x = 0;
    int y = 0;
    int r = 0;
    int w = 0;
    int h = 0;
    //x = 0;
    //y = 0;
    x = df->width / 2;
    y = df->height / 2;
    w = 3;
    h = 3;
    //df_init_rect2(df, (Rectangle){x, y, w, h}, begin_type, end_type);
    //df_place_wall_switch(df, x + rand() % w, y + rand() % h, txwallup, txwalldown, id);
    //df_create_trap_event(df, x + w, y + rand() % h, trap_on, trap_off, id);
    int count = 0;
    int total_rooms = 1;
    //int prev_r = -1;
    // create space for a list of Rectangles
    // we will generate each rectangle one at a time
    // after the first rectangle, we will make sure each new rectangle does not intersect or overlap
    // with any of the previously generated rectangles
    Rectangle rectangles[total_rooms];
    // initialize the rectangles to zero
    for (int i = 0; i < total_rooms; i++) {
        rectangles[i] = (Rectangle){0, 0, 0, 0};
    }
    int min_room_width = 2;
    int min_room_height = 2;
    int max_room_width = 10;
    int max_room_height = 10;
    // generate the first rectangle
    int rand_w = rand() % (max_room_width - min_room_width + 1) + min_room_width;
    int rand_h = rand() % (max_room_height - min_room_height + 1) + min_room_height;
    Rectangle rect = {x, y, rand_w, rand_h};
    rectangles[count] = rect;
    // now that we have our list of non-intersecting rectangles,
    // we can loop thru them and create the rooms
    for (int j = 0; j < total_rooms; j++) {
        Rectangle r = rectangles[j];
        df_init_rect2(df, r, begin_type, end_type);
        //df_place_wall_switch(df, r.x + rand() % (int)r.width, r.y + rand() % (int)r.height, txwallup, txwalldown, id);
        //df_create_trap_event(df, r.x + r.width, r.y + rand() % (int)r.height, trap_on, trap_off, id);
        id++;
    }
    while (count < total_rooms) {
        df_init_rect2(df, (Rectangle){x, y, w, h}, begin_type, end_type);
        //df_place_wall_switch(df, x + rand() % w, y + rand() % h, txwallup, txwalldown, id);
        r = rand() % 2;
        //while (r == prev_r) { r = rand() % 4; }
        if (r == 0) {
            df_create_trap_event(df, x + w, y + rand() % h, trap_on, trap_off, id);
            x = x + w + 1;
        } else if (r == 1) {
            df_create_trap_event(df, x + rand() % w, y + h, trap_on, trap_off, id);
            y = y + h + 1;
        }
        //else if (r == 2) {
        //    df_create_trap_event(df, x - 1, y + rand() % h, trap_on, trap_off, id);
        //    x = x - 2;
        //} else if (r == 3) {
        //    df_create_trap_event(df, x + rand() % w, y - 1, trap_on, trap_off, id);
        //    y = y - 2;
        //} else {
        //    continue;
        //}
        //prev_r = r;
        id++;
        count++;
        //w++;
        //h++;
    }
    //
    //    // after our rooms have been constructed and we're happy with the layout
    //    // we need to assign an upstairs and downstairs tile
    //
    // we will do this similar to the intermediate orc spawn test in liblogic
    // basically we will count the total possible tiles first
    // then we will create a list and add each possible tile location to the list
    // then we will be able to randomly select an upstairs and a downstairs from there
    int count2 = 0;
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* const tile = df_tile_at(df, (vec3){x, y, -1}); // we have no knowledge of what floor this is
            if (tile_is_possible_upstairs(tile->type)) {
                count2++;
            }
        }
    }
    vec3* locations = malloc(sizeof(vec3) * count2);
    massert(locations, "df_init_test: failed to malloc locations");
    int count3 = 0;
    // now we can loop thru the dungeon floor again and fill the array with the locations
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* const tile = df_tile_at(df, (vec3){x, y, -1}); // we have no knowledge of what floor this is
            if (tile_is_possible_upstairs(tile->type)) {
                // there wont be any entities yet so do not check for them
                // do not write an if statement
                locations[count3].x = x;
                locations[count3].y = y;
                count3++;
                massert(count3 <= count2, "liblogic_init: count2 is greater than count");
            }
        }
    }
    massert(count3 == count2, "liblogic_init: count2 is greater than count");
    // now that we have a list of possible locations for the upstairs to appear
    // we can randomly select one of them
    int upstairs_index = rand() % count3;
    // lazily set the downstairs
    int downstairs_index = rand() % count3;
    while (downstairs_index == upstairs_index) {
        downstairs_index = rand() % count3;
    }
    vec3 up_loc = locations[upstairs_index];
    vec3 down_loc = locations[downstairs_index];
    // now we can set the upstairs tile
    tile_t* const tile = df_tile_at(df, up_loc);
    if (!tile) {
        return;
    }
    tile_init(tile, TILE_UPSTAIRS);
    // now we can set the downstairs tile
    tile_t* const tile2 = df_tile_at(df, down_loc);
    if (!tile2) {
        return;
    }
    tile_init(tile2, TILE_DOWNSTAIRS);
    free(locations);
}

void df_set_tile(dungeon_floor_t* const df, tiletype_t type, int x, int y) {
    massert(df, "dungeon floor is NULL");
    tile_t* current = &df->tiles[y][x];
    tile_init(current, type);
}

static void df_set_tile_perimeter(dungeon_floor_t* const df, tiletype_t type, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    for (int i = 0; i <= w; i++) {
        tile_t* current = &df->tiles[y][x + i];
        tile_init(current, type);
    }
    for (int i = 0; i <= h; i++) {
        tile_t* current = &df->tiles[y + i][x];
        tile_init(current, type);
    }
    for (int i = 0; i <= w; i++) {
        tile_t* current = &df->tiles[y + h][x + i];
        tile_init(current, type);
    }
    for (int i = 0; i <= h; i++) {
        tile_t* current = &df->tiles[y + i][x + w];
        tile_init(current, type);
    }
}

static void df_set_tile_perimeter_range(dungeon_floor_t* const df, tiletype_t begin, tiletype_t end, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    tiletype_t begin_type = begin;
    tiletype_t end_type = end;
    if (end_type < begin_type) {
        tiletype_t temp = begin_type;
        begin_type = end_type;
        end_type = temp;
    }
    int num_types = end_type - begin_type + 1;
    tile_t* t = NULL;
    tiletype_t type = TILE_NONE;
    for (int i = 0; i <= w; i++) {
        t = df_tile_at(df, (vec3){x + i, y, -1});
        type = begin_type + (rand() % num_types);
        tile_init(t, type);
        t = df_tile_at(df, (vec3){x + i, y + h, -1});
        type = begin_type + (rand() % num_types);
        tile_init(t, type);
    }
    for (int i = 0; i <= h; i++) {
        t = df_tile_at(df, (vec3){x, y + i, -1});
        type = begin_type + (rand() % num_types);
        tile_init(t, type);
        t = df_tile_at(df, (vec3){x + w, y + i, -1});
        type = begin_type + (rand() % num_types);
        tile_init(t, type);
    }
}

void df_free(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    for (int i = 0; i < df->height; i++) {
        tile_t* t = df->tiles[i];
        if (t) free(t);
    }
    if (df->tiles) {
        free(df->tiles);
        df->tiles = NULL;
    }
    if (df->rooms) {
        free(df->rooms);
        df->rooms = NULL;
        df->room_count = 0;
        df->room_capacity = 0;
    }
    free(df);
    //msuccess("Freed dungeon floor");
}

bool df_add_at(dungeon_floor_t* const df, entityid id, int x, int y) {
    massert(df, "df is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    bool retval = false;
    retval = tile_add(&df->tiles[y][x], id) != -1;
    massert(retval != ENTITYID_INVALID, "tile_add failed");
    return retval;
}

bool df_remove_at(dungeon_floor_t* const df, entityid id, int x, int y) {
    massert(df, "df is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    const entityid r = tile_remove(&df->tiles[y][x], id);
    return r != -1 && r == id;
}

static void df_set_pressure_plate(dungeon_floor_t* const df, const int x, const int y, const int up_tx_key, const int dn_tx_key, const int event) {
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    tile_set_pressure_plate(&df->tiles[y][x], true);
    tile_set_pressure_plate_up_tx_key(&df->tiles[y][x], up_tx_key);
    tile_set_pressure_plate_down_tx_key(&df->tiles[y][x], dn_tx_key);
    tile_set_pressure_plate_event(&df->tiles[y][x], event);
}

static void df_set_wall_switch(dungeon_floor_t* const df, int x, int y, int up_key, int dn_key, int event) {
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) return;
    tile_t* const t = df_tile_at(df, (vec3){x, y, -1});
    if (!t) return;
    if (!tile_is_wall(t->type)) return;
    tile_set_wall_switch(t, true);
    tile_set_wall_switch_up_tx_key(t, up_key);
    tile_set_wall_switch_down_tx_key(t, dn_key);
    tile_set_wall_switch_event(t, event);
}

static void df_set_all_tiles(dungeon_floor_t* const df, tiletype_t type) {
    massert(df, "dungeon floor is NULL");
    df_set_tile_area(df, type, 0, 0, df->width, df->height);
}

static void df_set_all_tiles_range(dungeon_floor_t* const df, tiletype_t begin, tiletype_t end) {
    massert(df, "dungeon floor is NULL");
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
            tile_t* current = df_tile_at(df, (vec3){j, i, -1});
            tiletype_t type = begin_type + (rand() % num_types);
            tile_init(current, type);
        }
    }
}

static void df_set_tile_area_range2(dungeon_floor_t* const df, Rectangle r, tiletype_t begin, tiletype_t end) {
    massert(df, "dungeon floor is NULL");
    tiletype_t begin_type = begin;
    tiletype_t end_type = end;
    if (end_type < begin_type) {
        tiletype_t temp = begin_type;
        begin_type = end_type;
        end_type = temp;
    }
    int num_types = end_type - begin_type + 1;
    for (int i = r.y; i < r.y + r.height; i++) {
        for (int j = r.x; j < r.x + r.width; j++) {
            tile_t* current = df_tile_at(df, (vec3){j, i, -1});
            tiletype_t type = begin_type + (rand() % num_types);
            tile_init(current, type);
        }
    }
}

static void df_set_tile_area_range(dungeon_floor_t* const df, int x, int y, int w, int h, tiletype_t begin, tiletype_t end) {
    massert(df, "dungeon floor is NULL");
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
            tile_t* current = df_tile_at(df, (vec3){j, i, -1});
            tiletype_t type = begin_type + (rand() % num_types);
            tile_init(current, type);
        }
    }
}

static void df_reset_plates(dungeon_floor_t* const df) {
    massert(df, "dungeon floor is NULL");
    memset(df->plates, 0, sizeof(bool) * DEFAULT_DF_PLATES);
}

static void df_reset_events(dungeon_floor_t* const df) {
    massert(df, "dungeon floor is NULL");
    memset(df->events, 0, sizeof(df_event_t) * DEFAULT_DF_EVENTS);
    //for (int i = 0; i < DEFAULT_DF_EVENTS; i++) {
    //    df->events[i].listen_event = -1;
    //    df->events[i].x = -1;
    //    df->events[i].y = -1;
    //    df->events[i].on_type = TILE_NONE;
    //    df->events[i].off_type = TILE_NONE;
    //}
}

static bool df_malloc_tiles(dungeon_floor_t* const df) {
    massert(df, "dungeon floor is NULL");
    massert(df->width > 0, "width is less than zero");
    massert(df->height > 0, "height is less than zero");
    df->tiles = malloc(sizeof(tile_t*) * df->height);
    massert(df->tiles, "failed to malloc tiles");
    memset(df->tiles, 0, sizeof(tile_t*) * df->height);
    for (int i = 0; i < df->height; i++) {
        df->tiles[i] = malloc(sizeof(tile_t) * df->width);
        // malloc failed and we need to free everything up to this point
        if (df->tiles[i] == NULL) {
            for (int j = 0; j < i; j++) free(df->tiles[j]);
            return false;
        }
    }
    return true;
}

static void df_init_rect(dungeon_floor_t* df, int x, int y, int w, int h, tiletype_t t1, tiletype_t t2) {
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    massert(w > 0, "w is less than zero");
    massert(h > 0, "h is less than zero");
    df_set_tile_area_range(df, x, y, w, h, t1, t2);
}

static void df_init_rect2(dungeon_floor_t* df, Rectangle r, tiletype_t t1, tiletype_t t2) {
    massert(df, "dungeon floor is NULL");
    massert(r.x >= 0, "x is less than zero");
    massert(r.x < df->width, "x is out of bounds");
    massert(r.y >= 0, "y is less than zero");
    massert(r.y < df->height, "y is out of bounds");
    massert(r.width > 0, "w is less than zero");
    massert(r.height > 0, "h is less than zero");
    massert(t1 >= TILE_NONE, "t1 is out of bounds");
    massert(t1 < TILE_COUNT, "t1 is out of bounds");
    massert(t2 >= TILE_NONE, "t2 is out of bounds");
    massert(t2 < TILE_COUNT, "t2 is out of bounds");
    if (r.width <= 0 || r.height <= 0) return;
    df_set_tile_area_range2(df, r, t1, t2);
}

static void df_create_trap_event(dungeon_floor_t* df, int x, int y, tiletype_t on, tiletype_t off, df_event_id id) {
    massert(df, "dungeon floor is NULL");
    if (id >= DEFAULT_DF_EVENTS) return;
    df->events[id] = (df_event_t){.listen_event = id, .x = x, .y = y, .on_type = on, .off_type = off};
    df_set_tile(df, off, x, y);
}

static void df_place_wall_switch(dungeon_floor_t* df, int x, int y, int up, int down, df_event_id trigger_id) {
    massert(df, "dungeon floor is NULL");
    tiletype_t type = TILE_STONE_WALL_02;
    df_set_tile(df, type, x, y);
    df_set_wall_switch(df, x, y, up, down, trigger_id);
}

vec3 df_get_upstairs(dungeon_floor_t* const df) {
    massert(df, "dungeon floor is NULL");
    if (df->upstairs_loc.x != -1 && df->upstairs_loc.y != -1) {
        return df->upstairs_loc;
    }
    vec3 loc = {-1, -1, -1};
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* const tile = df_tile_at(df, (vec3){x, y, -1});
            if (tile->type == TILE_UPSTAIRS) {
                loc.x = x;
                loc.y = y;
                df->upstairs_loc = loc;
                return loc;
            }
        }
    }
    return loc;
}

vec3 df_get_downstairs(dungeon_floor_t* const df) {
    massert(df, "dungeon floor is NULL");
    if (df->downstairs_loc.x != -1 && df->downstairs_loc.y != -1) return df->downstairs_loc;
    vec3 loc = {-1, -1, -1};
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* const tile = df_tile_at(df, (vec3){x, y, -1});
            if (tile->type == TILE_DOWNSTAIRS) {
                loc.x = x;
                loc.y = y;
                df->downstairs_loc = loc;
                return loc;
            }
        }
    }
    return loc;
}

int df_count_walkable(const dungeon_floor_t* const df) {
    massert(df, "df is NULL");
    int count = 0;
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tiletype_t type = df_type_at(df, x, y);
            if (tile_is_walkable(type)) count++;
        }
    }
    return count;
}

int df_count_empty(const dungeon_floor_t* const df) {
    massert(df, "df is NULL");
    int count = 0;
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* const t = df_tile_at(df, (vec3){x, y, -1});
            if (tile_entity_count(t) == 0) count++;
        }
    }
    return count;
}

int df_count_non_walls_in_area(const dungeon_floor_t* const df, int x0, int y0, int w, int h) {
    massert(df, "df is NULL");
    int count = 0;
    for (int y = 0; y < h && y + y0 < df->height; y++) {
        for (int x = 0; x < w && x + x0 < df->width; x++) {
            int newx = x + x0;
            int newy = y + y0;
            tile_t* const t = df_tile_at(df, (vec3){newx, newy, -1});
            if (tile_is_walkable(t->type)) count++;
        }
    }
    return count;
}

int df_count_non_walls(const dungeon_floor_t* const df) {
    massert(df, "df is NULL");
    return df_count_non_walls_in_area(df, 0, 0, df->width, df->height);
}

int df_count_empty_non_walls_in_area(const dungeon_floor_t* const df, int x0, int y0, int w, int h) {
    massert(df, "df is NULL");
    int count = 0;
    for (int y = 0; y < h && y + y0 < df->height; y++) {
        for (int x = 0; x < w && x + x0 < df->width; x++) {
            int newx = x + x0;
            int newy = y + y0;
            tile_t* const t = df_tile_at(df, (vec3){newx, newy, -1});
            if (tile_entity_count(t) == 0 && tile_is_walkable(t->type)) count++;
        }
    }
    return count;
}

int df_count_empty_non_walls(const dungeon_floor_t* const df) {
    massert(df, "df is NULL");
    return df_count_empty_non_walls_in_area(df, 0, 0, df->width, df->height);
}

static void df_make_room(dungeon_floor_t* df, int x, int y, int w, int h) { df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11); }
static void df_make_corridor_h(dungeon_floor_t* df, int x, int y, int len) { df_set_tile_area_range(df, x, y, len, 1, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11); }
static void df_make_corridor_v(dungeon_floor_t* df, int x, int y, int len) { df_set_tile_area_range(df, x, y, 1, len, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11); }

//static void df_connect_rooms(dungeon_floor_t* df, int x1, int y1, int x2, int y2) {
// Connect by a 1-tile floor at midpoint to simulate door/corridor connection
//    int mx = (x1 + x2) / 2;
//    int my = (y1 + y2) / 2;
//    df_set_tile(df, TILE_FLOOR_STONE_00, mx, my);
//}

static void df_make_diamond_shape_room(dungeon_floor_t* df, int cx, int cy, int w, int h, tiletype_t begin_type, tiletype_t end_type) {
    massert(df, "dungeon floor is NULL");
    massert(w > 0 && h > 0, "width and height must be positive");
    massert(w >= 3 && h >= 3, "width and height must be at least 3 for diamond shape");
    massert(cx >= 0 && cx < df->width, "cx out of bounds");
    massert(cy >= 0 && cy < df->height, "cy out of bounds");
    massert(begin_type >= 0 && begin_type < TILE_COUNT, "begin_type out of bounds");
    massert(end_type >= 0 && end_type < TILE_COUNT, "end_type out of bounds");
    int ry = h / 2;
    int max_w = (w % 2) ? w : w - 1; // ensure odd width for perfect diamond
    for (int y = cy - ry; y <= cy + ry; y++) {
        int dy = abs(y - cy);
        int row_w = max_w - 2 * dy; // width shrinks by 2 per step from center line
        int start_x = cx - row_w / 2;
        for (int x = start_x; x < start_x + row_w; x++) {
            tiletype_t t = begin_type + rand() % (end_type - begin_type + 1);
            df_set_tile(df, t, x, y);
        }
    }
}

//static void df_init_test_simple9(dungeon_floor_t* df) {
//    massert(df, "dungeon floor is NULL");
//    int cx = df_center_x(df);
//    int cy = df_center_y(df);
//    int w = 5;
//    int h = 5;
//    // Diamond-shaped central room
//    df_make_diamond_shape_room(df, cx, cy, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
//    df_assign_downstairs_in_area(df, cx - w / 2, cy - h / 2, w, h);
//    df_assign_upstairs_in_area(df, cx - w / 2, cy - h / 2, w, h);
//}

//static void df_init_test_simple10(dungeon_floor_t* df) {
//    massert(df, "dungeon floor is NULL");
//    int cx = df_center_x(df);
//    int cy = df_center_y(df);
//    int outer_w = 15;
//    int outer_h = 15;
//    int inner_w = 7;
//    int inner_h = 7;
//    // Create outer wall diamond
//    df_make_diamond_shape_room(df, cx, cy, outer_w, outer_h, TILE_STONE_WALL_00, TILE_STONE_WALL_03);
//    // Create inner floor diamond
//    //tiletype_t begin_type = TILE_FLOOR_GRASS_00;
//    tiletype_t begin_type = TILE_FLOOR_STONE_00;
//    tiletype_t end_type = TILE_FLOOR_STONE_DIRT_DR_05;
//    df_make_diamond_shape_room(df, cx, cy, inner_w, inner_h, begin_type, end_type);
//    // Place stairs inside the room
//    df_assign_downstairs_in_area(df, cx - inner_w / 2, cy - inner_h / 2, inner_w, inner_h);
//    df_assign_upstairs_in_area(df, cx - inner_w / 2, cy - inner_h / 2, inner_w, inner_h);
//}

//static void df_init_test_complex1(dungeon_floor_t* df) {
//    massert(df, "dungeon floor is NULL");
//    // Configuration
//    int room_size = 3;
//    int gap_size = 1;
//    int grid_size = 5; // 3x3 grid of rooms
//    int total_span = grid_size * room_size + (grid_size - 1) * gap_size;
//    // Center of the grid
//    int start_x = df_center_x(df) - total_span / 2;
//    int start_y = df_center_y(df) - total_span / 2;
//    // Create walls for the entire area first
//    for (int grid_y = 0; grid_y < grid_size; grid_y++) {
//        for (int grid_x = 0; grid_x < grid_size; grid_x++) {
//            // Calculate top-left corner of this room's outer wall area
//            int room_left = start_x + grid_x * (room_size + gap_size) - 1;
//            int room_top = start_y + grid_y * (room_size + gap_size) - 1;
//            // Create wall perimeter (room_size + 2 to include walls)
//            df_set_tile_area_range(df, room_left, room_top, room_size + 2, room_size + 2, TILE_STONE_WALL_00, TILE_STONE_WALL_03);
//        }
//    }
//    // Create rooms and corridors
//    for (int grid_y = 0; grid_y < grid_size; grid_y++) {
//        for (int grid_x = 0; grid_x < grid_size; grid_x++) {
//            // Calculate top-left corner of this room
//            int room_x = start_x + grid_x * (room_size + gap_size);
//            int room_y = start_y + grid_y * (room_size + gap_size);
//            // Create the room
//            tiletype_t floor_start, floor_end;
//            if ((grid_x + grid_y) % 2 == 0) {
//                floor_start = TILE_FLOOR_STONE_00;
//                floor_end = TILE_FLOOR_STONE_11;
//            } else {
//                floor_start = TILE_FLOOR_STONE_DIRT_UL_00;
//                floor_end = TILE_FLOOR_STONE_DIRT_DR_05;
//            }
//            df_set_tile_area_range(df, room_x, room_y, room_size, room_size, floor_start, floor_end);
//            // Connect to room on the right if not the last column
//            if (grid_x < grid_size - 1) {
//                int corridor_x = room_x + room_size;
//                int corridor_y = room_y + room_size / 2;
//                df_set_tile(df, TILE_FLOOR_STONE_00, corridor_x, corridor_y);
//            }
//            // Connect to room below if not the last row
//            if (grid_y < grid_size - 1) {
//                int corridor_x = room_x + room_size / 2;
//                int corridor_y = room_y + room_size;
//                df_set_tile(df, TILE_FLOOR_STONE_00, corridor_x, corridor_y);
//            }
//        }
//    }
//    // Place stairs
//    int top_left_x = start_x;
//    int top_left_y = start_y;
//    df_assign_downstairs_in_area(df, top_left_x, top_left_y, room_size, room_size);
//    int bottom_right_x = start_x + (grid_size - 1) * (room_size + gap_size);
//    int bottom_right_y = start_y + (grid_size - 1) * (room_size + gap_size);
//    df_assign_upstairs_in_area(df, bottom_right_x, bottom_right_y, room_size, room_size);
//}

//static void df_init_test_complex2(dungeon_floor_t* df, int hallway_length) {
//    massert(df, "dungeon floor is NULL");
//    // Default hallway length if not specified
//    if (hallway_length <= 0) hallway_length = 3;
//    // Configuration
//    int room_size = 5;
//    int grid_size = 3; // 3x3 grid of rooms
//    // Adjust gap size to accommodate hallway length
//    int gap_size = hallway_length;
//    int total_span = grid_size * room_size + (grid_size - 1) * gap_size;
//    // Center of the grid
//    int start_x = df_center_x(df) - total_span / 2;
//    int start_y = df_center_y(df) - total_span / 2;
//    // Create walls for the entire area first
//    for (int grid_y = 0; grid_y < grid_size; grid_y++) {
//        for (int grid_x = 0; grid_x < grid_size; grid_x++) {
//            // Calculate top-left corner of this room's outer wall area
//            int room_left = start_x + grid_x * (room_size + gap_size) - 1;
//            int room_top = start_y + grid_y * (room_size + gap_size) - 1;
//            // Create wall perimeter (room_size + 2 to include walls)
//            df_set_tile_area_range(df, room_left, room_top, room_size + 2, room_size + 2, TILE_STONE_WALL_00, TILE_STONE_WALL_03);
//        }
//    }
//    // Create rooms and corridors
//    for (int grid_y = 0; grid_y < grid_size; grid_y++) {
//        for (int grid_x = 0; grid_x < grid_size; grid_x++) {
//            // Calculate top-left corner of this room
//            int room_x = start_x + grid_x * (room_size + gap_size);
//            int room_y = start_y + grid_y * (room_size + gap_size);
//            // Create the room
//            tiletype_t floor_start, floor_end;
//            if ((grid_x + grid_y) % 2 == 0) {
//                floor_start = TILE_FLOOR_STONE_00;
//                floor_end = TILE_FLOOR_STONE_11;
//            } else {
//                floor_start = TILE_FLOOR_STONE_DIRT_UL_00;
//                floor_end = TILE_FLOOR_STONE_DIRT_DR_05;
//            }
//            df_set_tile_area_range(df, room_x, room_y, room_size, room_size, floor_start, floor_end);
//            // Connect to room on the right if not the last column
//            if (grid_x < grid_size - 1) {
//                // Horizontal corridor
//                int corridor_start_x = room_x + room_size;
//                int corridor_y = room_y + room_size / 2;
//                // Create hallway
//                for (int i = 0; i < hallway_length; i++) {
//                    // Create floor tile for hallway
//                    df_set_tile(df, TILE_FLOOR_STONE_00, corridor_start_x + i, corridor_y);
//                    // Create wall tiles above and below hallway
//                    df_set_tile(df, corridor_start_x + i, corridor_y - 1, TILE_STONE_WALL_00);
//                    df_set_tile(df, corridor_start_x + i, corridor_y + 1, TILE_STONE_WALL_00);
//                }
//            }
//            // Connect to room below if not the last row
//            if (grid_y < grid_size - 1) {
//                // Vertical corridor
//                int corridor_x = room_x + room_size / 2;
//                int corridor_start_y = room_y + room_size;
//                // Create hallway
//                for (int i = 0; i < hallway_length; i++) {
//                    // Create floor tile for hallway
//                    df_set_tile(df, TILE_FLOOR_STONE_00, corridor_x, corridor_start_y + i);
//                    // Create wall tiles to the left and right of hallway
//                    df_set_tile(df, corridor_x - 1, corridor_start_y + i, TILE_STONE_WALL_00);
//                    df_set_tile(df, corridor_x + 1, corridor_start_y + i, TILE_STONE_WALL_00);
//                }
//            }
//        }
//    }
//    // Place stairs
//    int top_left_x = start_x;
//    int top_left_y = start_y;
//    df_assign_downstairs_in_area(df, top_left_x, top_left_y, room_size, room_size);
//    int bottom_right_x = start_x + (grid_size - 1) * (room_size + gap_size);
//    int bottom_right_y = start_y + (grid_size - 1) * (room_size + gap_size);
//    df_assign_upstairs_in_area(df, bottom_right_x, bottom_right_y, room_size, room_size);
//}

//static void df_init_test_complex3(dungeon_floor_t* df) {
//    massert(df, "dungeon floor is NULL");
//    // Configuration
//    int min_room_size = 3;
//    int max_room_size = 6;
//    int min_hallway_length = 2;
//    int max_hallway_length = 5;
//    int grid_size = 5; // 5x5 grid of potential room locations
//    float room_chance = 0.8f; // Probability a grid cell contains a room
//    float hallway_chance = 0.7f; // Probability a hallway exists between adjacent rooms
//    // Generate random seed based on time if desired
//    // srand(time(NULL));
//    // First, decide which grid cells will contain rooms and their sizes
//    int room_exists[grid_size][grid_size];
//    int room_width[grid_size][grid_size];
//    int room_height[grid_size][grid_size];
//    int hallway_length_h[grid_size][grid_size]; // Horizontal hallways (to the right)
//    int hallway_length_v[grid_size][grid_size]; // Vertical hallways (to the bottom)
//    bool hallway_exists_h[grid_size][grid_size]; // Does horizontal hallway exist?
//    bool hallway_exists_v[grid_size][grid_size]; // Does vertical hallway exist?
//    // Initialize with no rooms or hallways
//    memset(room_exists, 0, sizeof(room_exists));
//    memset(hallway_exists_h, 0, sizeof(hallway_exists_h));
//    memset(hallway_exists_v, 0, sizeof(hallway_exists_v));
//    // Ensure at least one room exists (top-left)
//    room_exists[0][0] = 1;
//    // Randomly decide room existence and sizes
//    for (int y = 0; y < grid_size; y++) {
//        for (int x = 0; x < grid_size; x++) {
//            if (x == 0 && y == 0) continue; // Skip first room, already set
//            // Random chance for room to exist
//            room_exists[y][x] = ((float)rand() / RAND_MAX) < room_chance ? 1 : 0;
//            // If room exists, determine its size
//            if (room_exists[y][x]) {
//                room_width[y][x] = min_room_size + rand() % (max_room_size - min_room_size + 1);
//                room_height[y][x] = min_room_size + rand() % (max_room_size - min_room_size + 1);
//            }
//        }
//    }
//    // Room size for first room
//    room_width[0][0] = min_room_size + rand() % (max_room_size - min_room_size + 1);
//    room_height[0][0] = min_room_size + rand() % (max_room_size - min_room_size + 1);
//    // Determine hallway existence and lengths
//    for (int y = 0; y < grid_size; y++) {
//        for (int x = 0; x < grid_size; x++) {
//            if (!room_exists[y][x]) continue;
//            // Horizontal hallway (to the right)
//            if (x < grid_size - 1 && room_exists[y][x + 1]) {
//                hallway_exists_h[y][x] = ((float)rand() / RAND_MAX) < hallway_chance;
//                if (hallway_exists_h[y][x]) { hallway_length_h[y][x] = min_hallway_length + rand() % (max_hallway_length - min_hallway_length + 1); }
//            }
//            // Vertical hallway (to the bottom)
//            if (y < grid_size - 1 && room_exists[y + 1][x]) {
//                hallway_exists_v[y][x] = ((float)rand() / RAND_MAX) < hallway_chance;
//                if (hallway_exists_v[y][x]) { hallway_length_v[y][x] = min_hallway_length + rand() % (max_hallway_length - min_hallway_length + 1); }
//            }
//        }
//    }
//    // Ensure dungeon connectivity using a simplified approach:
//    // Make sure every room (except the first) is connected to at least one other room
//    for (int y = 0; y < grid_size; y++) {
//        for (int x = 0; x < grid_size; x++) {
//            if (!room_exists[y][x] || (x == 0 && y == 0)) continue;
//            // Check if this room has any connections
//            bool has_connection = (x > 0 && room_exists[y][x - 1] && hallway_exists_h[y][x - 1]) ||
//                                  (y > 0 && room_exists[y - 1][x] && hallway_exists_v[y - 1][x]) ||
//                                  (x < grid_size - 1 && room_exists[y][x + 1] && hallway_exists_h[y][x]) ||
//                                  (y < grid_size - 1 && room_exists[y + 1][x] && hallway_exists_v[y][x]);
//            // If no connections, add one
//            if (!has_connection) {
//                // Try to connect to a random adjacent room
//                int attempts = 0;
//                bool connected = false;
//                while (!connected && attempts < 4) {
//                    int direction = rand() % 4; // 0: left, 1: up, 2: right, 3: down
//                    switch (direction) {
//                    case 0: // Left
//                        if (x > 0 && room_exists[y][x - 1]) {
//                            hallway_exists_h[y][x - 1] = true;
//                            hallway_length_h[y][x - 1] = min_hallway_length + rand() % (max_hallway_length - min_hallway_length + 1);
//                            connected = true;
//                        }
//                        break;
//                    case 1: // Up
//                        if (y > 0 && room_exists[y - 1][x]) {
//                            hallway_exists_v[y - 1][x] = true;
//                            hallway_length_v[y - 1][x] = min_hallway_length + rand() % (max_hallway_length - min_hallway_length + 1);
//                            connected = true;
//                        }
//                        break;
//                    case 2: // Right
//                        if (x < grid_size - 1 && room_exists[y][x + 1]) {
//                            hallway_exists_h[y][x] = true;
//                            hallway_length_h[y][x] = min_hallway_length + rand() % (max_hallway_length - min_hallway_length + 1);
//                            connected = true;
//                        }
//                        break;
//                    case 3: // Down
//                        if (y < grid_size - 1 && room_exists[y + 1][x]) {
//                            hallway_exists_v[y][x] = true;
//                            hallway_length_v[y][x] = min_hallway_length + rand() % (max_hallway_length - min_hallway_length + 1);
//                            connected = true;
//                        }
//                        break;
//                    }
//                    attempts++;
//                }
//                // If still not connected, may need to force a connection
//                if (!connected) {
//                    // This is a simple fallback, in a real situation you might
//                    // want to use a more sophisticated approach like connecting to the nearest room
//                    if (x > 0 && room_exists[y][x - 1]) {
//                        hallway_exists_h[y][x - 1] = true;
//                        hallway_length_h[y][x - 1] = min_hallway_length;
//                    } else if (y > 0 && room_exists[y - 1][x]) {
//                        hallway_exists_v[y - 1][x] = true;
//                        hallway_length_v[y - 1][x] = min_hallway_length;
//                    }
//                }
//            }
//        }
//    }
// Calculate total dungeon size needed
//    int max_total_width = 0;
//    int max_total_height = 0;
//    for (int y = 0; y < grid_size; y++) {
//        for (int x = 0; x < grid_size; x++) {
//            if (room_exists[y][x]) {
//                int room_right = (x + 1) * max_room_size + x * max_hallway_length + room_width[y][x];
//                int room_bottom = (y + 1) * max_room_size + y * max_hallway_length + room_height[y][x];
//                max_total_width = room_right > max_total_width ? room_right : max_total_width;
//                max_total_height = room_bottom > max_total_height ? room_bottom : max_total_height;
//            }
//        }
//    }
//    // Center of the dungeon
//    int start_x = df_center_x(df) - max_total_width / 2;
//    int start_y = df_center_y(df) - max_total_height / 2;
//    // Calculate actual room positions
//    int room_x[grid_size][grid_size];
//    int room_y[grid_size][grid_size];
//    memset(room_x, 0, sizeof(room_x));
//    memset(room_y, 0, sizeof(room_y));
//    // First room position
//    room_x[0][0] = start_x;
//    room_y[0][0] = start_y;
//    // Calculate positions of all other rooms based on hallway lengths
//    for (int y = 0; y < grid_size; y++) {
//        for (int x = 0; x < grid_size; x++) {
//            if (!room_exists[y][x]) continue;
//            // If not the first room, position is determined by hallways
//            if (!(x == 0 && y == 0)) {
//                // Initialize with impossible values
//                room_x[y][x] = -1;
//                room_y[y][x] = -1;
//                // Check for connection from the left
//                if (x > 0 && room_exists[y][x - 1] && hallway_exists_h[y][x - 1]) {
//                    room_x[y][x] = room_x[y][x - 1] + room_width[y][x - 1] + hallway_length_h[y][x - 1];
//                    room_y[y][x] = room_y[y][x - 1];
//                }
//                // Check for connection from above
//                else if (y > 0 && room_exists[y - 1][x] && hallway_exists_v[y - 1][x]) {
//                    room_x[y][x] = room_x[y - 1][x];
//                    room_y[y][x] = room_y[y - 1][x] + room_height[y - 1][x] + hallway_length_v[y - 1][x];
//                }
//                // If still not placed, this is disconnected - should not happen with our connectivity logic
//                if (room_x[y][x] == -1) {
//                    // Safety fallback
//                    room_x[y][x] = start_x + x * (max_room_size + max_hallway_length);
//                    room_y[y][x] = start_y + y * (max_room_size + max_hallway_length);
//                }
//            }
//        }
//    }
//    // Now build the actual dungeon
//    // Create walls for the entire area first to ensure enclosed rooms
//    for (int y = 0; y < grid_size; y++) {
//        for (int x = 0; x < grid_size; x++) {
//            if (!room_exists[y][x]) continue;
//            // Create wall perimeter (room size + 2 to include walls)
//            int room_left = room_x[y][x] - 1;
//            int room_top = room_y[y][x] - 1;
//            df_set_tile_area_range(df, room_left, room_top, room_width[y][x] + 2, room_height[y][x] + 2, TILE_STONE_WALL_00, TILE_STONE_WALL_03);
//        }
//    }
//    // Create rooms
//    for (int y = 0; y < grid_size; y++) {
//        for (int x = 0; x < grid_size; x++) {
//            if (!room_exists[y][x]) continue;
//            // Create the room
//            tiletype_t floor_start, floor_end;
//            if ((x + y) % 3 == 0) {
//                floor_start = TILE_FLOOR_STONE_00;
//                floor_end = TILE_FLOOR_STONE_11;
//            } else if ((x + y) % 3 == 1) {
//                floor_start = TILE_FLOOR_STONE_DIRT_UL_00;
//                floor_end = TILE_FLOOR_STONE_DIRT_DR_05;
//            } else {
//                floor_start = TILE_FLOOR_STONE_00;
//                floor_end = TILE_FLOOR_STONE_11;
//            }
//            df_set_tile_area_range(df, room_x[y][x], room_y[y][x], room_width[y][x], room_height[y][x], floor_start, floor_end);
//            // Create horizontal hallway to the right
//            if (x < grid_size - 1 && hallway_exists_h[y][x]) {
//                int hallway_start_x = room_x[y][x] + room_width[y][x];
//                int hallway_y = room_y[y][x] + room_height[y][x] / 2;
//                for (int i = 0; i < hallway_length_h[y][x]; i++) {
//                    // Floor tile for hallway
//                    df_set_tile(df, TILE_FLOOR_STONE_00, hallway_start_x + i, hallway_y);
//                    // Wall tiles above and below
//                    df_set_tile(df, TILE_STONE_WALL_00, hallway_start_x + i, hallway_y - 1);
//                    df_set_tile(df, TILE_STONE_WALL_00, hallway_start_x + i, hallway_y + 1);
//                }
//            }
//
//            // Create vertical hallway downward
//            if (y < grid_size - 1 && hallway_exists_v[y][x]) {
//                int hallway_x = room_x[y][x] + room_width[y][x] / 2;
//                int hallway_start_y = room_y[y][x] + room_height[y][x];
//                for (int i = 0; i < hallway_length_v[y][x]; i++) {
//                    // Floor tile for hallway
//                    df_set_tile(df, TILE_FLOOR_STONE_00, hallway_x, hallway_start_y + i);
//                    // Wall tiles to left and right
//                    df_set_tile(df, TILE_STONE_WALL_00, hallway_x - 1, hallway_start_y + i);
//                    df_set_tile(df, TILE_STONE_WALL_00, hallway_x + 1, hallway_start_y + i);
//                }
//            }
//        }
//    }
//    // Find rooms for stairs
//    int start_room_x = 0, start_room_y = 0; // Default first room
//    int end_room_x = -1, end_room_y = -1;
//    int max_manhattan_distance = -1;
//    // Find the room furthest from the first room to place exit stairs
//    for (int y = 0; y < grid_size; y++) {
//        for (int x = 0; x < grid_size; x++) {
//            if (!room_exists[y][x]) continue;
//            int distance = abs(x - start_room_x) + abs(y - start_room_y);
//            if (distance > max_manhattan_distance) {
//                max_manhattan_distance = distance;
//                end_room_x = x;
//                end_room_y = y;
//            }
//        }
//    }
//    // Place stairs
//    df_assign_downstairs_in_area(df,
//                                 room_x[start_room_y][start_room_x],
//                                 room_y[start_room_y][start_room_x],
//                                 room_width[start_room_y][start_room_x],
//                                 room_height[start_room_y][start_room_x]);
//    df_assign_upstairs_in_area(
//        df, room_x[end_room_y][end_room_x], room_y[end_room_y][end_room_x], room_width[end_room_y][end_room_x], room_height[end_room_y][end_room_x]);
//}

static void df_init_test_complex4(dungeon_floor_t* df, int hallway_length) {
    massert(df, "dungeon floor is NULL");
    // Default hallway length if not specified
    if (hallway_length <= 0) hallway_length = 3;
    // Configuration
    int room_size = 3;
    int grid_size = 5; // 3x3 grid of rooms
    // Adjust gap size to accommodate hallway length
    int gap_size = hallway_length;
    int total_span = grid_size * room_size + (grid_size - 1) * gap_size;
    // Center of the grid
    int start_x = df_center_x(df) - total_span / 2;
    int start_y = df_center_y(df) - total_span / 2;
    // Create walls for the entire area first
    for (int grid_y = 0; grid_y < grid_size; grid_y++) {
        for (int grid_x = 0; grid_x < grid_size; grid_x++) {
            // Calculate top-left corner of this room's outer wall area
            int room_left = start_x + grid_x * (room_size + gap_size) - 1;
            int room_top = start_y + grid_y * (room_size + gap_size) - 1;
            // Create wall perimeter (room_size + 2 to include walls)
            df_set_tile_area_range(df, room_left, room_top, room_size + 2, room_size + 2, TILE_STONE_WALL_00, TILE_STONE_WALL_03);
        }
    }
    // Create rooms and corridors
    for (int grid_y = 0; grid_y < grid_size; grid_y++) {
        for (int grid_x = 0; grid_x < grid_size; grid_x++) {
            // Calculate top-left corner of this room
            int room_x = start_x + grid_x * (room_size + gap_size);
            int room_y = start_y + grid_y * (room_size + gap_size);
            // Create the room
            tiletype_t floor_start, floor_end;
            if ((grid_x + grid_y) % 2 == 0) {
                floor_start = TILE_FLOOR_STONE_00;
                floor_end = TILE_FLOOR_STONE_11;
            } else {
                floor_start = TILE_FLOOR_STONE_DIRT_UL_00;
                floor_end = TILE_FLOOR_STONE_DIRT_DR_05;
            }
            df_set_tile_area_range(df, room_x, room_y, room_size, room_size, floor_start, floor_end);
            // Connect to room on the right if not the last column
            if ((grid_x < grid_size - 1) || (grid_y < grid_size - 1)) {
                int hallways_created = 0;
                if (grid_x < grid_size - 1) {
                    int do_create = rand() % 2;
                    if (do_create) {
                        // Horizontal corridor
                        int corridor_start_x = room_x + room_size;
                        int corridor_y = room_y + room_size / 2;
                        // Create hallway
                        for (int i = 0; i < hallway_length; i++) {
                            // Create floor tile for hallway
                            df_set_tile(df, TILE_FLOOR_STONE_00, corridor_start_x + i, corridor_y);
                            // Create wall tiles above and below hallway
                            df_set_tile(df, corridor_start_x + i, corridor_y - 1, TILE_STONE_WALL_00);
                            df_set_tile(df, corridor_start_x + i, corridor_y + 1, TILE_STONE_WALL_00);
                        }
                        hallways_created++;
                    }
                }
                // Connect to room below if not the last row
                if (grid_y < grid_size - 1) {
                    int do_create = rand() % 2;
                    if (do_create) {
                        // Vertical corridor
                        int corridor_x = room_x + room_size / 2;
                        int corridor_start_y = room_y + room_size;
                        // Create hallway
                        for (int i = 0; i < hallway_length; i++) {
                            // Create floor tile for hallway
                            df_set_tile(df, TILE_FLOOR_STONE_00, corridor_x, corridor_start_y + i);
                            // Create wall tiles to the left and right of hallway
                            df_set_tile(df, corridor_x - 1, corridor_start_y + i, TILE_STONE_WALL_00);
                            df_set_tile(df, corridor_x + 1, corridor_start_y + i, TILE_STONE_WALL_00);
                        }
                        hallways_created++;
                    }
                }
                while (hallways_created < 1) {
                    if (grid_x < grid_size - 1) {
                        int do_create = rand() % 2;
                        if (do_create) {
                            // Horizontal corridor
                            int corridor_start_x = room_x + room_size;
                            int corridor_y = room_y + room_size / 2;
                            // Create hallway
                            for (int i = 0; i < hallway_length; i++) {
                                // Create floor tile for hallway
                                df_set_tile(df, TILE_FLOOR_STONE_00, corridor_start_x + i, corridor_y);
                                // Create wall tiles above and below hallway
                                df_set_tile(df, corridor_start_x + i, corridor_y - 1, TILE_STONE_WALL_00);
                                df_set_tile(df, corridor_start_x + i, corridor_y + 1, TILE_STONE_WALL_00);
                            }
                            hallways_created++;
                        }
                    }
                    if (hallways_created) {
                        break;
                    }
                    // Connect to room below if not the last row
                    if (grid_y < grid_size - 1) {
                        int do_create = rand() % 2;
                        if (do_create) {
                            // Vertical corridor
                            int corridor_x = room_x + room_size / 2;
                            int corridor_start_y = room_y + room_size;
                            // Create hallway
                            for (int i = 0; i < hallway_length; i++) {
                                // Create floor tile for hallway
                                df_set_tile(df, TILE_FLOOR_STONE_00, corridor_x, corridor_start_y + i);
                                // Create wall tiles to the left and right of hallway
                                df_set_tile(df, corridor_x - 1, corridor_start_y + i, TILE_STONE_WALL_00);
                                df_set_tile(df, corridor_x + 1, corridor_start_y + i, TILE_STONE_WALL_00);
                            }
                            hallways_created++;
                        }
                    }
                }
            }
        }
    }
    // Place stairs
    int top_left_x = start_x;
    int top_left_y = start_y;
    df_assign_downstairs_in_area(df, top_left_x, top_left_y, room_size, room_size);
    int bottom_right_x = start_x + (grid_size - 1) * (room_size + gap_size);
    int bottom_right_y = start_y + (grid_size - 1) * (room_size + gap_size);
    df_assign_upstairs_in_area(df, bottom_right_x, bottom_right_y, room_size, room_size);
}

static void df_init_test_complex6(dungeon_floor_t* df, range room_length, range room_width) {
    massert(df, "dungeon floor is NULL");
    massert(room_length.min > 0, "room_length.min must be greater than 0");
    massert(room_length.max > 0, "room_length.max must be greater than 0");
    massert(room_width.min > 0, "room_width.min must be greater than 0");
    massert(room_width.max > 0, "room_width.max must be greater than 0");
    // single room
    int center_x = df_center_x(df);
    int center_y = df_center_y(df);
    int room_length_value = get_random_in_range(room_length);
    int room_width_value = get_random_in_range(room_width);
    range wall_thickness_range = (range){1, 5};
    int wall_thickness = get_random_in_range(wall_thickness_range);
    // Create walls for the entire area first
    df_set_tile_area_range(df,
                           center_x - room_length_value / 2 - (wall_thickness),
                           center_y - room_width_value / 2 - (wall_thickness),
                           room_length_value + (2 * wall_thickness),
                           room_width_value + (2 * wall_thickness),
                           TILE_STONE_WALL_00,
                           TILE_STONE_WALL_03);
    // Create the room
    tiletype_t floor_start, floor_end;
    if (rand() % 2 == 0) {
        floor_start = TILE_FLOOR_STONE_00;
        floor_end = TILE_FLOOR_STONE_11;
    } else {
        floor_start = TILE_FLOOR_STONE_DIRT_UL_00;
        floor_end = TILE_FLOOR_STONE_DIRT_DR_05;
    }
    int x = center_x - room_length_value / 2;
    int y = center_y - room_width_value / 2;
    int w = room_length_value;
    int h = room_width_value;
    df_set_tile_area_range(df, x, y, w, h, floor_start, floor_end);
    df_add_room_info(df, x, y, w, h, "room1");
    df_assign_downstairs(df);
    df_assign_upstairs(df);
}

bool df_add_room(dungeon_floor_t* const df, int x, int y, int w, int h, tiletype_t begin, tiletype_t end, const char* room_name) {
    massert(df, "dungeon floor is NULL");
    df_set_tile_area_range(df, x, y, w, h, begin, end);
    return df_add_room_info(df, x, y, w, h, room_name);
}

// for this test, we will be constructing rooms based on a grid system
// the df is already a grid of tiles, but we will introduce an overlaying grid
// that will be specified by grid_w and grid_h
// individual rooms will be constructed from the center of the overlaying grid
// and each grid section will contain at most 1 room that will be of a random size
// up to 3/4 of grid_w and grid_h
static void df_init_test_complex9(dungeon_floor_t* df, int grid_cell_w, int grid_cell_h) {
    massert(df, "dungeon floor is NULL");
    massert(grid_cell_w >= 4, "grid_cell_w must be greater than 4");
    massert(grid_cell_h >= 4, "grid_cell_h must be greater than 4");
    // Configuration
    int min_room_size = 2;
    int max_room_size = grid_cell_w * 3 / 4;
    // hallways will be as long as they need to be to connect rooms
    int hallway_length = 3;
    // Center of the grid
    int start_x = df_center_x(df) / 2;
    int start_y = df_center_y(df) / 2;
    int grid_rows = df->height / grid_cell_h;
    int grid_cols = df->width / grid_cell_w;
    // Create walls for the entire area first
    df_set_tile_area_range(df, 0, 0, df->width, df->height, TILE_STONE_WALL_00, TILE_STONE_WALL_03);
    int grid_row_index = grid_rows / 2;
    int grid_col_index = grid_cols / 2;
    start_x = grid_col_index * grid_cell_w;
    start_y = grid_row_index * grid_cell_h;

    int local_grid_rows = df->floor + 1;
    int local_grid_cols = df->floor + 1;
    massert(local_grid_rows > 0, "local_grid_rows must be greater than 0");
    massert(local_grid_cols > 0, "local_grid_cols must be greater than 0");

    // Create rooms and corridors
    for (int i = 0; i < local_grid_rows; i++) {
        for (int j = 0; j < local_grid_cols; j++) {
            int x = (grid_col_index + j) * grid_cell_w;
            int y = (grid_row_index + i) * grid_cell_h;
            // Random room size within the grid cell
            int room_w = min_room_size + rand() % (max_room_size - min_room_size + 1);
            int room_h = min_room_size + rand() % (max_room_size - min_room_size + 1);
            // Ensure room fits within the grid cell
            if (room_w > grid_cell_w) room_w = grid_cell_w;
            if (room_h > grid_cell_h) room_h = grid_cell_h;
            // Create the room
            tiletype_t floor_start, floor_end;
            if ((i + j) % 2 == 0) {
                floor_start = TILE_FLOOR_STONE_00;
                floor_end = TILE_FLOOR_STONE_11;
            } else {
                floor_start = TILE_FLOOR_STONE_DIRT_UL_00;
                floor_end = TILE_FLOOR_STONE_DIRT_DR_05;
            }
            df_set_tile_area_range(df, x, y, room_w, room_h, floor_start, floor_end);
            df_add_room_info(df, x, y, room_w, room_h, "room");
        }
    }
    // Create hallways between rooms
    for (int i = 0; i < local_grid_rows; i++) {
        for (int j = 0; j < local_grid_cols; j++) {
            int x = (grid_col_index + j) * grid_cell_w;
            int y = (grid_row_index + i) * grid_cell_h;
            // Horizontal hallway to the right
            if (j < local_grid_cols - 1) {
                int hallway_start_x = x + (rand() % (grid_cell_w - hallway_length));
                int hallway_y = y + grid_cell_h / 2;
                for (int k = 0; k <= hallway_length; k++) {
                    df_set_tile(df, TILE_FLOOR_STONE_00, hallway_start_x + k, hallway_y);
                    //df_set_tile(df, TILE_STONE_WALL_00, hallway_start_x + k, hallway_y - 1);
                    //df_set_tile(df, TILE_STONE_WALL_00, hallway_start_x + k, hallway_y + 1);
                }
            }
            // Vertical hallway downward
            if (i < local_grid_rows - 1) {
                int hallway_x = x + grid_cell_w / 2;
                int hallway_start_y = y + (rand() % (grid_cell_h - hallway_length));
                for (int k = 0; k <= hallway_length; k++) {
                    df_set_tile(df, TILE_FLOOR_STONE_00, hallway_x, hallway_start_y + k);
                    //df_set_tile(df, TILE_STONE_WALL_00, hallway_x - 1, hallway_start_y + k);
                    //df_set_tile(df, TILE_STONE_WALL_00, hallway_x + 1, hallway_start_y + k);
                }
            }
        }
    }
    // Place stairs
    int top_left_x = start_x;
    int top_left_y = start_y;
    df_assign_downstairs_in_area(df, top_left_x, top_left_y, grid_cell_w * local_grid_cols, grid_cell_h * local_grid_rows);
    df_assign_upstairs_in_area(df, top_left_x, top_left_y, grid_cell_w * local_grid_cols, grid_cell_h * local_grid_rows);
}

static void df_init_test_complex8(dungeon_floor_t* df, int w, int h) {
    //static void df_init_test_complex8(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    //int w = 20;
    //int h = 20;
    int cx = df_center_x(df);
    int cy = df_center_y(df);
    int x = cx - w / 2;
    int y = cy - h / 2;
    // Create walls for the entire area first
    df_set_tile_area_range(df, x - w, y - h, 3 * w, 3 * h, TILE_STONE_WALL_00, TILE_STONE_WALL_03);
    // Create the room
    df_add_room(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, "room1");
    df_assign_upstairs_in_area(df, x, y, w, h);
    df_assign_downstairs_in_area(df, x, y, w, h);
}

static void df_init_test_complex7(dungeon_floor_t* df, range room_width, range room_length) {
    massert(df, "dungeon floor is NULL");
    int cx = df_center_x(df);
    int cy = df_center_y(df);
    //int w = 3;
    int w = get_random_in_range(room_width);
    //int h = 3;
    int h = get_random_in_range(room_length);
    // starting point
    int start_x = cx - w;
    int start_y = cy - h;
    int x = start_x;
    int y = start_y;
    df_set_tile_area_range(df, x - 10, y - 10, 40, 40, TILE_STONE_WALL_00, TILE_STONE_WALL_03);
    df_add_room(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, "room1");
    df_assign_upstairs_in_area(df, df->rooms[0].x, df->rooms[0].y, df->rooms[0].w, df->rooms[0].h);
    x += w;
    y += h / 2;
    // single tile hallway between rooms
    w = 3;
    h = 1;
    //df_set_tile_area_range(df, x, y - 1, w, h + 2, TILE_STONE_WALL_00, TILE_STONE_WALL_03);
    df_add_room(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, "hallway1");
    //df_set_tile_area_range(df, x, y + 1, w, h, TILE_STONE_WALL_00, TILE_STONE_WALL_03);
    x += w;
    y = start_y;
    w = get_random_in_range(room_width);
    h = get_random_in_range(room_length);
    //df_set_tile_area_range(df, x - 1, y - 1, w + 2, h + 2, TILE_STONE_WALL_00, TILE_STONE_WALL_03);
    df_add_room(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, "room2");
    x += w;
    y += h / 2;
    df_add_room(df, x, y, 3, 1, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, "hallway2");
    x += 3;
    y = start_y;
    w = get_random_in_range(room_width);
    h = get_random_in_range(room_length);
    //df_set_tile_area_range(df, x - 1, y - 1, w + 2, h + 2, TILE_STONE_WALL_00, TILE_STONE_WALL_03);
    df_add_room(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, "room3");
    df_assign_downstairs_in_area(df, df->rooms[df->room_count - 1].x, df->rooms[df->room_count - 1].y, df->rooms[df->room_count - 1].w, df->rooms[df->room_count - 1].h);
}

static void df_init_test_complex5(dungeon_floor_t* df, range hallway_length) {
    massert(df, "dungeon floor is NULL");
    // Default hallway length if not specified
    if (hallway_length.min <= 0) hallway_length.min = 3;
    if (hallway_length.max <= 0) hallway_length.max = 3;
    // Configuration
    int room_size = 3;
    int grid_size = 5; // 3x3 grid of rooms
    // Adjust gap size to accommodate hallway length
    //int gap_size = hallway_length;
    int gap_size = get_random_in_range(hallway_length);
    int total_span = grid_size * room_size + (grid_size - 1) * gap_size;
    // Center of the grid
    int start_x = df_center_x(df) - total_span / 2;
    int start_y = df_center_y(df) - total_span / 2;
    // Create walls for the entire area first
    for (int grid_y = 0; grid_y < grid_size; grid_y++) {
        for (int grid_x = 0; grid_x < grid_size; grid_x++) {
            // Calculate top-left corner of this room's outer wall area
            int room_left = start_x + grid_x * (room_size + gap_size) - 1;
            int room_top = start_y + grid_y * (room_size + gap_size) - 1;

            // Create wall perimeter (room_size + 2 to include walls)
            df_set_tile_area_range(df, room_left, room_top, room_size + 2, room_size + 2, TILE_STONE_WALL_00, TILE_STONE_WALL_03);
        }
    }
    // Create rooms and corridors
    for (int grid_y = 0; grid_y < grid_size; grid_y++) {
        for (int grid_x = 0; grid_x < grid_size; grid_x++) {
            // Calculate top-left corner of this room
            int room_x = start_x + grid_x * (room_size + gap_size);
            int room_y = start_y + grid_y * (room_size + gap_size);
            // Create the room
            tiletype_t floor_start, floor_end;
            if ((grid_x + grid_y) % 2 == 0) {
                floor_start = TILE_FLOOR_STONE_00;
                floor_end = TILE_FLOOR_STONE_11;
            } else {
                floor_start = TILE_FLOOR_STONE_DIRT_UL_00;
                floor_end = TILE_FLOOR_STONE_DIRT_DR_05;
            }
            df_set_tile_area_range(df, room_x, room_y, room_size, room_size, floor_start, floor_end);
            // Connect to room on the right if not the last column
            if ((grid_x < grid_size - 1) || (grid_y < grid_size - 1)) {
                int hallways_created = 0;
                if (grid_x < grid_size - 1) {
                    int do_create = rand() % 2;
                    if (do_create) {
                        // Horizontal corridor
                        int corridor_start_x = room_x + room_size;
                        int corridor_y = room_y + room_size / 2;
                        // Create hallway
                        //for (int i = 0; i < hallway_length; i++) {
                        for (int i = 0; i < gap_size; i++) {
                            // Create floor tile for hallway
                            df_set_tile(df, TILE_FLOOR_STONE_00, corridor_start_x + i, corridor_y);
                            // Create wall tiles above and below hallway
                            df_set_tile(df, corridor_start_x + i, corridor_y - 1, TILE_STONE_WALL_00);
                            df_set_tile(df, corridor_start_x + i, corridor_y + 1, TILE_STONE_WALL_00);
                        }
                        hallways_created++;
                    }
                }
                // Connect to room below if not the last row
                if (grid_y < grid_size - 1) {
                    int do_create = rand() % 2;
                    if (do_create) {
                        // Vertical corridor
                        int corridor_x = room_x + room_size / 2;
                        int corridor_start_y = room_y + room_size;
                        // Create hallway
                        //for (int i = 0; i < hallway_length; i++) {
                        for (int i = 0; i < gap_size; i++) {
                            // Create floor tile for hallway
                            df_set_tile(df, TILE_FLOOR_STONE_00, corridor_x, corridor_start_y + i);
                            // Create wall tiles to the left and right of hallway
                            df_set_tile(df, corridor_x - 1, corridor_start_y + i, TILE_STONE_WALL_00);
                            df_set_tile(df, corridor_x + 1, corridor_start_y + i, TILE_STONE_WALL_00);
                        }
                        hallways_created++;
                    }
                }
                while (hallways_created < 1) {
                    if (grid_x < grid_size - 1) {
                        int do_create = rand() % 2;
                        if (do_create) {
                            // Horizontal corridor
                            int corridor_start_x = room_x + room_size;
                            int corridor_y = room_y + room_size / 2;
                            // Create hallway
                            //for (int i = 0; i < hallway_length; i++) {
                            for (int i = 0; i < gap_size; i++) {
                                // Create floor tile for hallway
                                df_set_tile(df, TILE_FLOOR_STONE_00, corridor_start_x + i, corridor_y);
                                // Create wall tiles above and below hallway
                                df_set_tile(df, corridor_start_x + i, corridor_y - 1, TILE_STONE_WALL_00);
                                df_set_tile(df, corridor_start_x + i, corridor_y + 1, TILE_STONE_WALL_00);
                            }
                            hallways_created++;
                        }
                    }
                    if (hallways_created) {
                        break;
                    }
                    // Connect to room below if not the last row
                    if (grid_y < grid_size - 1) {
                        int do_create = rand() % 2;
                        if (do_create) {
                            // Vertical corridor
                            int corridor_x = room_x + room_size / 2;
                            int corridor_start_y = room_y + room_size;
                            // Create hallway
                            //for (int i = 0; i < hallway_length; i++) {
                            for (int i = 0; i < gap_size; i++) {
                                // Create floor tile for hallway
                                df_set_tile(df, TILE_FLOOR_STONE_00, corridor_x, corridor_start_y + i);
                                // Create wall tiles to the left and right of hallway
                                df_set_tile(df, corridor_x - 1, corridor_start_y + i, TILE_STONE_WALL_00);
                                df_set_tile(df, corridor_x + 1, corridor_start_y + i, TILE_STONE_WALL_00);
                            }
                            hallways_created++;
                        }
                    }
                }
            }
        }
    }
    // Place stairs
    int top_left_x = start_x;
    int top_left_y = start_y;
    df_assign_downstairs_in_area(df, top_left_x, top_left_y, room_size, room_size);
    int bottom_right_x = start_x + (grid_size - 1) * (room_size + gap_size);
    int bottom_right_y = start_y + (grid_size - 1) * (room_size + gap_size);
    df_assign_upstairs_in_area(df, bottom_right_x, bottom_right_y, room_size, room_size);
}

void df_init_rooms(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    const int default_capacity = 10;
    df->rooms = malloc(sizeof(room_data_t) * default_capacity);
    massert(df->rooms, "Failed to allocate memory for rooms");
    df->room_count = 0;
    df->room_capacity = default_capacity;
}

static bool rects_overlap(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    // Check if one rectangle is to the left of the other
    if (x1 + w1 <= x2 || x2 + w2 <= x1) return false;
    // Check if one rectangle is above the other
    if (y1 + h1 <= y2 || y2 + h2 <= y1) return false;
    // If neither, they overlap
    return true;
}

static bool room_overlaps_any(dungeon_floor_t* df, int x, int y, int w, int h) {
    for (int i = 0; i < df->room_count; i++) {
        room_data_t* r = &df->rooms[i];
        if (rects_overlap(x, y, w, h, r->x, r->y, r->w, r->h)) {
            return true;
        }
    }
    return false;
}

bool df_add_room_info(dungeon_floor_t* df, int x, int y, int w, int h, const char* name) {
    massert(df, "dungeon floor is NULL");
    massert(name, "room name is NULL");
    massert(w > 0 && h > 0, "room dimensions are invalid");
    massert(x >= 0 && y >= 0, "room coordinates are invalid");
    massert(x + w <= df->width && y + h <= df->height, "room exceeds dungeon bounds");
    massert(df->rooms, "room data is NULL");
    massert(df->room_count >= 0, "room count is invalid");
    massert(strlen(name) < sizeof(df->rooms[0].room_name), "room name is too long");
    massert(df->room_capacity > 0, "room capacity is invalid");
    // Check for overlaps with existing rooms
    if (room_overlaps_any(df, x, y, w, h)) {
        return false;
    }
    // Handle capacity
    if (df->room_count == df->room_capacity) {
        int new_cap = df->room_capacity ? df->room_capacity * 2 : 8;
        room_data_t* tmp = realloc(df->rooms, sizeof(room_data_t) * new_cap);
        if (!tmp) {
            return false;
        }
        df->rooms = tmp;
        df->room_capacity = new_cap;
    }
    // Add the new room
    room_data_t* r = &df->rooms[df->room_count++];
    r->x = x;
    r->y = y;
    r->w = w;
    r->h = h;
    strncpy(r->room_name, name, sizeof(r->room_name) - 1);
    r->room_name[sizeof(r->room_name) - 1] = '\0';
    return true;
}

int df_loc_is_in_room(dungeon_floor_t* const df, vec3 loc) {
    massert(df, "dungeon floor is NULL");
    massert(df->rooms, "room data is NULL");
    massert(df->room_count > 0, "room count is invalid");
    massert(loc.x >= 0 && loc.y >= 0, "location coordinates are invalid");
    massert(loc.x < df->width && loc.y < df->height, "location exceeds dungeon bounds");
    for (int i = 0; i < df->room_count; i++) {
        room_data_t* r = &df->rooms[i];
        if (loc.x >= r->x && loc.x < r->x + r->w && loc.y >= r->y && loc.y < r->y + r->h) {
            return i;
        }
    }
    return -1;
}

const char* df_get_room_name(dungeon_floor_t* const df, vec3 loc) {
    massert(df, "dungeon floor is NULL");
    massert(df->rooms, "room data is NULL");
    massert(df->room_count > 0, "room count is invalid");
    massert(loc.x >= 0 && loc.y >= 0, "location coordinates are invalid");
    massert(loc.x < df->width && loc.y < df->height, "location exceeds dungeon bounds");
    int room_index = df_loc_is_in_room(df, loc);
    if (room_index == -1) {
        return NULL;
    }
    return df->rooms[room_index].room_name;
}

vec3* const df_get_all_locs(dungeon_floor_t* const df, int* external_count) {
    massert(df, "dungeon floor is NULL");
    massert(df->rooms, "room data is NULL");
    massert(df->room_count > 0, "room count is invalid");
    massert(external_count, "external count is NULL");
    int count = 0;
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* const t = df_tile_at(df, (vec3){x, y, -1});
            massert(t, "tile is NULL");
            if (t->type == TILE_NONE || t->type == TILE_COUNT) {
                continue;
            }
            count++;
        }
    }
    vec3* locs = malloc(sizeof(vec3) * count);
    massert(locs, "Failed to allocate memory for locs");
    int index = 0;
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* const t = df_tile_at(df, (vec3){x, y, -1});
            massert(t, "tile is NULL");
            if (t->type == TILE_NONE || t->type == TILE_COUNT) continue;
            locs[index++] = (vec3){x, y, -1};
        }
    }
    *external_count = count;
    return locs;
}

vec3* const df_get_all_locs_outside_of_rooms(dungeon_floor_t* const df, int* external_count) {
    massert(df, "dungeon floor is NULL");
    massert(df->rooms, "room data is NULL");
    massert(df->room_count > 0, "room count is invalid");
    massert(external_count, "external count is NULL");
    int all_locs_count = 0;
    vec3* all_locs = df_get_all_locs(df, &all_locs_count);
    massert(all_locs, "Failed to get all locs");
    int count = 0;
    for (int i = 0; i < all_locs_count; i++) {
        vec3 loc = all_locs[i];
        if (df_loc_is_in_room(df, loc) == -1) count++;
    }
    vec3* outside_locs = malloc(sizeof(vec3) * count);
    massert(outside_locs, "Failed to allocate memory for outside locs");
    int index = 0;
    for (int i = 0; i < all_locs_count; i++) {
        vec3 loc = all_locs[i];
        if (df_loc_is_in_room(df, loc) == -1) {
            outside_locs[index].x = loc.x;
            outside_locs[index].y = loc.y;
            index++;
        }
    }
    free(all_locs);
    *external_count = count;
    return outside_locs;
}

room_data_t* const df_get_rooms_with_prefix(dungeon_floor_t* const df, int* external_count, const char* prefix) {
    massert(df, "dungeon floor is NULL");
    massert(df->rooms, "room data is NULL");
    // room_count can be 0
    massert(df->room_count >= 0, "room count is invalid");
    massert(external_count, "external count is NULL");
    massert(prefix, "prefix is NULL");
    massert(strlen(prefix) > 0, "prefix is empty");
    //massert(strlen(prefix) < sizeof(df->rooms[0].room_name), "prefix is too long");
    int count = 0;
    for (int i = 0; i < df->room_count; i++) {
        room_data_t* r = &df->rooms[i];
        if (strncmp(r->room_name, prefix, strlen(prefix)) == 0) {
            count++;
        }
    }
    room_data_t* rooms = malloc(sizeof(room_data_t) * count);
    massert(rooms, "Failed to allocate memory for rooms");
    int index = 0;
    for (int i = 0; i < df->room_count; i++) {
        room_data_t* r = &df->rooms[i];
        if (strncmp(r->room_name, prefix, strlen(prefix)) == 0) {
            rooms[index].x = r->x;
            rooms[index].y = r->y;
            rooms[index].w = r->w;
            rooms[index].h = r->h;
            strncpy(rooms[index].room_name, r->room_name, sizeof(rooms[index].room_name) - 1);
            rooms[index].room_name[sizeof(rooms[index].room_name) - 1] = '\0';
            index++;
        }
    }
    *external_count = count;
    return rooms;
}

size_t df_serialized_size(const dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");

    // Calculate size by exactly matching what's written in df_serialize
    size_t size = 0;

    // Basic fields
    size += sizeof(int) * 2; // width and height
    size += sizeof(vec3) * 2; // upstairs_loc and downstairs_loc

    // Room data
    size += sizeof(int) * 2; // room_count and room_capacity
    size += df->room_count * sizeof(room_data_t); // rooms array

    // Events and plates
    size += sizeof(df_event_t) * DEFAULT_DF_EVENTS;
    size += sizeof(bool) * DEFAULT_DF_PLATES;

    // Tiles
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            size += tile_serialized_size(&df->tiles[y][x]);
        }
    }

    return size;
}

size_t df_serialize(const dungeon_floor_t* df, char* buffer, size_t buffer_size) {
    massert(df, "dungeon floor is NULL");
    massert(buffer, "buffer is NULL");

    size_t required_size = df_serialized_size(df);
    if (buffer_size < required_size) {
        return 0;
    }

    char* ptr = buffer;

    // Serialize basic fields
    memcpy(ptr, &df->width, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, &df->height, sizeof(int));
    ptr += sizeof(int);

    // Serialize locations
    memcpy(ptr, &df->upstairs_loc, sizeof(vec3));
    ptr += sizeof(vec3);
    memcpy(ptr, &df->downstairs_loc, sizeof(vec3));
    ptr += sizeof(vec3);

    // Serialize room data
    memcpy(ptr, &df->room_count, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, &df->room_capacity, sizeof(int));
    ptr += sizeof(int);

    // Serialize rooms array if it exists
    if (df->room_count > 0 && df->rooms) {
        memcpy(ptr, df->rooms, df->room_count * sizeof(room_data_t));
    }
    ptr += df->room_count * sizeof(room_data_t);

    // Serialize events
    memcpy(ptr, df->events, sizeof(df_event_t) * DEFAULT_DF_EVENTS);
    ptr += sizeof(df_event_t) * DEFAULT_DF_EVENTS;

    // Serialize plates
    memcpy(ptr, df->plates, sizeof(bool) * DEFAULT_DF_PLATES);
    ptr += sizeof(bool) * DEFAULT_DF_PLATES;

    // Serialize tiles
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            size_t tile_size = tile_serialized_size(&df->tiles[y][x]);
            size_t written = tile_serialize(&df->tiles[y][x], ptr, tile_size);
            if (written == 0) {
                return 0;
            }
            ptr += written;
        }
    }

    return ptr - buffer;
}

bool df_deserialize(dungeon_floor_t* df, const char* buffer, size_t buffer_size) {
    massert(df, "dungeon floor is NULL");
    massert(buffer, "buffer is NULL");

    // Initialize the dungeon floor with default values
    df->rooms = NULL;
    df->room_count = 0;
    df->room_capacity = 0;
    df->upstairs_loc = (vec3){-1, -1};
    df->downstairs_loc = (vec3){-1, -1};
    df_reset_plates(df);
    df_reset_events(df);

    const char* ptr = buffer;

    // Deserialize basic fields
    memcpy(&df->width, ptr, sizeof(int));
    ptr += sizeof(int);
    memcpy(&df->height, ptr, sizeof(int));
    ptr += sizeof(int);

    // Deserialize locations
    memcpy(&df->upstairs_loc, ptr, sizeof(vec3));
    ptr += sizeof(vec3);
    memcpy(&df->downstairs_loc, ptr, sizeof(vec3));
    ptr += sizeof(vec3);

    // Deserialize room data
    int room_count, room_capacity;
    memcpy(&room_count, ptr, sizeof(int));
    ptr += sizeof(int);
    memcpy(&room_capacity, ptr, sizeof(int));
    ptr += sizeof(int);

    // Allocate and deserialize rooms array
    df->rooms = malloc(room_capacity * sizeof(room_data_t));
    if (!df->rooms) {
        return false;
    }

    if (room_count > 0) {
        memcpy(df->rooms, ptr, room_count * sizeof(room_data_t));
    }
    df->room_count = room_count;
    df->room_capacity = room_capacity;
    ptr += room_count * sizeof(room_data_t);

    // Deserialize events
    memcpy(df->events, ptr, sizeof(df_event_t) * DEFAULT_DF_EVENTS);
    ptr += sizeof(df_event_t) * DEFAULT_DF_EVENTS;

    // Deserialize plates
    memcpy(df->plates, ptr, sizeof(bool) * DEFAULT_DF_PLATES);
    ptr += sizeof(bool) * DEFAULT_DF_PLATES;

    // Allocate memory for tiles
    if (!df_malloc_tiles(df)) {
        return false;
    }

    // Deserialize tiles
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            // Initialize the tile with default values first
            tile_init(&df->tiles[y][x], TILE_NONE);

            size_t tile_size = tile_serialized_size(&df->tiles[y][x]);
            //printf("Deserializing tile at %d,%d with size %zu\n", x, y, tile_size);

            // Debug: Print first few bytes of the buffer
            //printf("Buffer data: ");
            //for (int i = 0; i < 8 && i < tile_size; i++) {
            //    printf("%02X ", (unsigned char)ptr[i]);
            //}
            //printf("\n");

            if (!tile_deserialize(&df->tiles[y][x], ptr, tile_size)) {
                return false;
            }
            ptr += tile_size;
        }
    }

    return true;
}

size_t df_memory_size(const dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    // Calculate the memory size of a dungeon floor
    size_t size = 0;
    // Size of the dungeon_floor_t struct itself
    size += sizeof(dungeon_floor_t);
    // Size of rooms array
    if (df->rooms) size += df->room_capacity * sizeof(room_data_t);
    // Size of tiles array and all tiles
    if (df->tiles) {
        // Size of the tile pointers array
        size += df->height * sizeof(tile_t*);
        // Size of each row of tiles
        for (int y = 0; y < df->height; y++) {
            if (df->tiles[y]) {
                // Size of the row itself
                size += df->width * sizeof(tile_t);
                // Size of each tile's dynamic memory
                for (int x = 0; x < df->width; x++) size += tile_memory_size(&df->tiles[y][x]) - sizeof(tile_t);
            }
        }
    }
    return size;
}
