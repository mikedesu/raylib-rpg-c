#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "dungeon_tile_type.h"
#include "libgame_defines.h"
#include "location.h"
#include "massert.h"
#include "mprint.h"
#include "raylib.h"
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
static void df_set_tile(dungeon_floor_t* const df, tiletype_t type, int x, int y);
static void df_set_all_tiles_range(dungeon_floor_t* const df, tiletype_t begin, tiletype_t end);
static void df_set_all_tiles(dungeon_floor_t* const df, tiletype_t type);
static void df_set_tile_area(dungeon_floor_t* const df, tiletype_t type, int x, int y, int w, int h);
static void df_set_tile_area_range(dungeon_floor_t* const df, int x, int y, int w, int h, tiletype_t begin, tiletype_t end);
static void df_set_tile_area_range2(dungeon_floor_t* const df, Rectangle r, tiletype_t begin, tiletype_t end);
static void df_init_rect2(dungeon_floor_t* df, Rectangle r, tiletype_t t1, tiletype_t t2);
static void df_init_rect(dungeon_floor_t* df, int x, int y, int w, int h, tiletype_t t1, tiletype_t t2);
static bool df_malloc_tiles(dungeon_floor_t* const df);
static void df_init_test_simple(dungeon_floor_t* df);
static void df_init_test_simple2(dungeon_floor_t* df);
static void df_init_test_simple3(dungeon_floor_t* df);
static void df_init_test_simple4(dungeon_floor_t* df);
static void df_init_test_simple5(dungeon_floor_t* df);

static void df_init_test_simple6(dungeon_floor_t* df); // gpt-4.1-mini
static void df_init_test_simple7(dungeon_floor_t* df); // gpt-4.1-mini
static void df_init_test_simple8(dungeon_floor_t* df); // gpt-4.1-mini
static void df_init_test_simple9(dungeon_floor_t* df); // gpt-4.1-mini

static void df_assign_stairs(dungeon_floor_t* df);

static void df_assign_downstairs(dungeon_floor_t* df);
static void df_assign_upstairs(dungeon_floor_t* df);
static void df_assign_upstairs_in_area(dungeon_floor_t* df, int x, int y, int w, int h);
static void df_assign_downstairs_in_area(dungeon_floor_t* df, int x, int y, int w, int h);

static int df_get_possible_upstairs_count(dungeon_floor_t* df);
static int df_get_possible_downstairs_count(dungeon_floor_t* df);

static loc_t* df_get_possible_upstairs_locs(dungeon_floor_t* df);
static loc_t* df_get_possible_downstairs_locs(dungeon_floor_t* df);
static loc_t* df_get_possible_upstairs_locs_in_area(dungeon_floor_t* df, int x, int y, int w, int h);
static loc_t* df_get_possible_downstairs_locs_in_area(dungeon_floor_t* df, int x, int y, int w, int h);

static int df_get_possible_upstairs_count_in_area(dungeon_floor_t* df, int x, int y, int w, int h);
static int df_get_possible_downstairs_count_in_area(dungeon_floor_t* df, int x, int y, int w, int h);

static void df_make_room(dungeon_floor_t* df, int x, int y, int w, int h);
static void df_make_corridor_h(dungeon_floor_t* df, int x, int y, int len);
static void df_make_corridor_v(dungeon_floor_t* df, int x, int y, int len);
static void df_connect_rooms(dungeon_floor_t* df, int x1, int y1, int x2, int y2);
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
            tile_t* const tile = df_tile_at(df, x0, y0);
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
    //for (int y = 0; y < df->height; y++) {
    for (int y0 = y; y0 < df->height; y0++) {
        for (int x0 = x; x0 < df->width; x0++) {
            tile_t* const tile = df_tile_at(df, x0, y0);
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

static loc_t* df_get_possible_upstairs_locs(dungeon_floor_t* df) { return df_get_possible_upstairs_locs_in_area(df, 0, 0, df->width, df->height); }

static loc_t* df_get_possible_downstairs_locs(dungeon_floor_t* df) { return df_get_possible_downstairs_locs_in_area(df, 0, 0, df->width, df->height); }

static loc_t* df_get_possible_upstairs_locs_in_area(dungeon_floor_t* df, int x, int y, int w, int h) {
    int count = df_get_possible_upstairs_count_in_area(df, x, y, w, h);
    loc_t* locations = malloc(sizeof(loc_t) * count);
    massert(locations, "failed to malloc locations");
    int count2 = 0;
    // now we can loop thru the dungeon floor again and fill the array with the locations
    for (int y0 = y; y0 < df->height; y0++) {
        for (int x0 = x; x0 < df->width; x0++) {
            tile_t* const tile = df_tile_at(df, x0, y0);
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
    return locations;
}

static loc_t* df_get_possible_downstairs_locs_in_area(dungeon_floor_t* df, int x, int y, int w, int h) {
    int count = df_get_possible_downstairs_count_in_area(df, x, y, w, h);
    loc_t* locations = malloc(sizeof(loc_t) * count);
    massert(locations, "failed to malloc locations");
    int count2 = 0;
    // now we can loop thru the dungeon floor again and fill the array with the locations
    for (int y0 = y; y0 < df->height; y0++) {
        for (int x0 = x; x0 < df->width; x0++) {
            tile_t* const tile = df_tile_at(df, x0, y0);
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
    return locations;
}

static void df_assign_upstairs_in_area(dungeon_floor_t* df, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    massert(w > 0, "w is less than zero");
    massert(h > 0, "h is less than zero");
    massert(x + w <= df->width, "x + w is out of bounds");
    massert(y + h <= df->height, "y + h is out of bounds");
    int count = df_get_possible_upstairs_count_in_area(df, x, y, w, h);
    loc_t* locations = df_get_possible_upstairs_locs_in_area(df, x, y, w, h);
    // now that we have a list of possible locations for the upstairs to appear
    // we can randomly select one of them
    int upstairs_index = rand() % count;
    loc_t up_loc = locations[upstairs_index];
    // now we can set the upstairs tile
    tile_t* const tile = df_tile_at(df, up_loc.x, up_loc.y);
    massert(tile, "failed to get tile");
    tile_init(tile, TILE_UPSTAIRS);
    free(locations);
}

static void df_assign_upstairs(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    df_assign_upstairs_in_area(df, 0, 0, df->width, df->height);
}

static void df_assign_downstairs(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    df_assign_downstairs_in_area(df, 0, 0, df->width, df->height);
}

static void df_assign_downstairs_in_area(dungeon_floor_t* df, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    massert(w > 0, "w is less than zero");
    massert(h > 0, "h is less than zero");
    massert(x + w <= df->width, "x + w is out of bounds");
    massert(y + h <= df->height, "y + h is out of bounds");
    int count = df_get_possible_downstairs_count_in_area(df, x, y, w, h);
    loc_t* locations = df_get_possible_downstairs_locs_in_area(df, x, y, w, h);
    // now that we have a list of possible locations for the upstairs to appear
    // we can randomly select one of them
    int downstairs_index = rand() % count;
    loc_t up_loc = locations[downstairs_index];
    // now we can set the upstairs tile
    tile_t* const tile = df_tile_at(df, up_loc.x, up_loc.y);
    massert(tile, "failed to get tile");
    tile_init(tile, TILE_DOWNSTAIRS);
    free(locations);
}

int df_center_x(const dungeon_floor_t* const df) {
    massert(df, "dungeon floor is NULL");
    return df->width / 2;
}

int df_center_y(const dungeon_floor_t* const df) {
    massert(df, "dungeon floor is NULL");
    return df->height / 2;
}

static void df_init_test_simple(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    int x = df->width / 2;
    int y = df->height / 2;
    int w = 8;
    int h = 8;
    tiletype_t begin_type = TILE_FLOOR_STONE_00;
    tiletype_t end_type = TILE_FLOOR_STONE_11;
    df_set_tile_area_range(df, x, y, w, h, begin_type, end_type);
    df_assign_stairs(df);
}

static void df_init_test_simple2(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    int x = df_center_x(df), y = df_center_y(df);
    int w = 4, h = 4;
    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_assign_downstairs_in_area(df, x, y, w, h);

    x += w + 1;
    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_assign_upstairs_in_area(df, x, y, w, h);
}

static void df_init_test_simple3(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    int x = df_center_x(df), y = df_center_y(df);
    int w = 4, h = 4;
    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_assign_downstairs_in_area(df, x, y, w, h);

    x += w + 1;
    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_assign_upstairs_in_area(df, x, y, w, h);

    x = df_center_x(df) + w;
    y = df_center_y(df) + h / 2;
    df_set_tile(df, TILE_FLOOR_STONE_00, x, y);
}

static void df_init_test_simple4(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    int x = df_center_x(df), y = df_center_y(df);
    int w = 4, h = 4;
    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    x += w + 1;
    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    // now we need to attach a single tile to connect the rooms
    // we will use the tile to the right of the first room
    // we will set the tile to the right of the first room to be a stone floor 00
    x = df_center_x(df) + w;
    y = df_center_y(df) + h / 2;
    df_set_tile(df, TILE_FLOOR_STONE_00, x, y);
    // now we will create two tiles to do a 'trap'
    // this involves
    // 1. creating a wall tile
    // 2. attaching a switch to the wall
    // 3. creating a trap tile
    // 4. attaching the trap to the switch
    int id = 1;
    int t_x = x, t_y = y;
    x = df_center_x(df) + w, y = df_center_y(df);
    int s_x = x + rand() % w, s_y = y + rand() % h;
    df_place_wall_switch(df, s_x, s_y, TX_WALL_SWITCH_UP_00, TX_WALL_SWITCH_DOWN_00, id);
    df_create_trap_event(df, t_x, t_y, TILE_FLOOR_STONE_TRAP_ON_00, TILE_FLOOR_STONE_TRAP_OFF_00, id);
    df_set_tile(df, TILE_FLOOR_STONE_TRAP_ON_00, t_x, t_y);
    df_assign_stairs(df);
}

static void df_init_test_simple5(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    int x = df_center_x(df), y = df_center_y(df);
    int w = 4, h = 4;
    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_assign_downstairs_in_area(df, x, y, w, h);

    int separating_distance = 10;

    x += w + separating_distance;
    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_assign_upstairs_in_area(df, x, y, w, h);

    x = df_center_x(df) + w;
    y = df_center_y(df) + h / 2;
    df_set_tile_area_range(df, x, y, separating_distance, 1, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
}

static void df_init_test_simple6(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    int x = df_center_x(df), y = df_center_y(df);
    int w = 4, h = 4;
    int dist = 10;

    // Left room with downstairs
    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_assign_downstairs_in_area(df, x, y, w, h);

    // Right room with upstairs
    x += w + dist;
    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_assign_upstairs_in_area(df, x, y, w, h);

    // Horizontal hallway between rooms
    x = df_center_x(df) + w;
    y = df_center_y(df) + h / 2;
    df_set_tile_area_range(df, x, y, dist, 1, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);

    // Vertical hallway centered on horizontal hallway
    int vx = x + dist / 2;
    int vy = y - dist / 2;
    df_set_tile_area_range(df, vx, vy, 1, dist, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);

    // Top vertical room
    df_set_tile_area_range(df, vx - w / 2, vy - h, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);

    // Bottom vertical room
    int bot_y = vy + dist;
    df_set_tile_area_range(df, vx - w / 2, bot_y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
}

static void df_init_test_simple7(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    int cx = df_center_x(df), cy = df_center_y(df);
    int w = 4, h = 4;
    int gap = 3;

    // Central big room
    df_set_tile_area_range(df, cx - w, cy - h, w * 2, h * 2, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_assign_downstairs_in_area(df, cx - w, cy - h, w * 2, h * 2);

    // Four small rooms on each side connected by single-tile doorways
    // Top room
    int tx = cx - w / 2, ty = cy - h * 2 - gap;
    df_set_tile_area_range(df, tx, ty, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_set_tile(df, TILE_FLOOR_STONE_00, cx, ty + h); // connect door top-center

    // Bottom room
    int bx = cx - w / 2, by = cy + h * 2 + gap;
    df_set_tile_area_range(df, bx, by, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_set_tile(df, TILE_FLOOR_STONE_00, cx, by - 1); // connect door bottom-center

    // Left room
    int lx = cx - w * 2 - gap, ly = cy - h / 2;
    df_set_tile_area_range(df, lx, ly, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_set_tile(df, TILE_FLOOR_STONE_00, lx + w, cy); // connect door left-center

    // Right room
    int rx = cx + w * 2 + gap, ry = cy - h / 2;
    df_set_tile_area_range(df, rx, ry, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_set_tile(df, TILE_FLOOR_STONE_00, rx - 1, cy); // connect door right-center

    // Horizontal corridors to connect rooms to center
    df_set_tile_area_range(df, lx + w, cy, cx - (lx + w), 1, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11); // left corridor
    df_set_tile_area_range(df, cx + w, cy, rx - (cx + w), 1, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11); // right corridor

    // Vertical corridors to connect rooms to center
    df_set_tile_area_range(df, cx, ty + h, 1, cy - (ty + h), TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11); // top corridor
    df_set_tile_area_range(df, cx, cy + h, 1, by - (cy + h), TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11); // bottom corridor

    df_assign_upstairs_in_area(df, rx, ry, w, h);
}

static void df_init_test_simple8(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    int cx = df_center_x(df), cy = df_center_y(df);
    int w = 4, h = 4;
    int gap = 3;

    // Central big room with downstairs
    df_make_room(df, cx - w, cy - h, w * 2, h * 2);
    df_assign_downstairs_in_area(df, cx - w, cy - h, w * 2, h * 2);

    // Four small rooms
    int tx = cx - w / 2, ty = cy - h * 2 - gap;
    df_make_room(df, tx, ty, w, h);
    df_connect_rooms(df, cx, cy - h, cx, ty + h);

    int bx = cx - w / 2, by = cy + h * 2 + gap;
    df_make_room(df, bx, by, w, h);
    df_connect_rooms(df, cx, cy + h, cx, by - 1);

    int lx = cx - w * 2 - gap, ly = cy - h / 2;
    df_make_room(df, lx, ly, w, h);
    df_connect_rooms(df, lx + w, cy, cx - w, cy);

    int rx = cx + w * 2 + gap, ry = cy - h / 2;
    df_make_room(df, rx, ry, w, h);
    df_connect_rooms(df, rx - 1, cy, cx + w, cy);

    // Corridors
    df_make_corridor_h(df, lx + w, cy, (cx - (lx + w)));
    df_make_corridor_h(df, cx + w, cy, (rx - (cx + w)));

    df_make_corridor_v(df, cx, ty + h, (cy - (ty + h)));
    df_make_corridor_v(df, cx, cy + h, (by - (cy + h)));

    df_assign_upstairs_in_area(df, rx, ry, w, h);
}

dungeon_floor_t* df_create(const int width, const int height) {
    dungeon_floor_t* floor = malloc(sizeof(dungeon_floor_t));
    massert(floor, "failed to malloc dungeon floor");
    df_init(floor);
    return floor;
}

static void df_set_tile_area(dungeon_floor_t* const df, tiletype_t type, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            tile_t* current = df_tile_at(df, j, i);
            tile_init(current, type);
        }
    }
}

void df_init(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    df->width = DEFAULT_DUNGEON_FLOOR_WIDTH;
    df->height = DEFAULT_DUNGEON_FLOOR_HEIGHT;
    df_reset_plates(df);
    df_reset_events(df);
    if (!df_malloc_tiles(df)) {
        merror("failed to malloc tiles");
        return;
    }
    df_set_all_tiles(df, TILE_NONE);
    // at this point, we are free to customize the dungeon floor to our liking
    //df_init_test(df);
    //df_init_test_simple(df);
    //df_init_test_simple2(df);
    //df_init_test_simple3(df);
    //df_init_test_simple4(df);
    //df_init_test_simple5(df);
    //df_init_test_simple6(df);
    //df_init_test_simple7(df);
    //df_init_test_simple8(df);
    df_init_test_simple9(df);
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
    for (int i = 0; i < total_rooms; i++) { rectangles[i] = (Rectangle){0, 0, 0, 0}; }
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
        //    merror("df_init_test: invalid random number");
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
            tile_t* const tile = df_tile_at(df, x, y);
            if (tile_is_possible_upstairs(tile->type)) { count2++; }
        }
    }
    loc_t* locations = malloc(sizeof(loc_t) * count2);
    massert(locations, "df_init_test: failed to malloc locations");
    int count3 = 0;
    // now we can loop thru the dungeon floor again and fill the array with the locations
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* const tile = df_tile_at(df, x, y);
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
    while (downstairs_index == upstairs_index) { downstairs_index = rand() % count3; }
    loc_t up_loc = locations[upstairs_index];
    loc_t down_loc = locations[downstairs_index];
    // now we can set the upstairs tile
    tile_t* const tile = df_tile_at(df, up_loc.x, up_loc.y);
    if (!tile) {
        merror("df_init_test: failed to get tile");
        return;
    }
    tile_init(tile, TILE_UPSTAIRS);
    // now we can set the downstairs tile
    tile_t* const tile2 = df_tile_at(df, down_loc.x, down_loc.y);
    if (!tile2) {
        merror("df_init_test: failed to get tile");
        return;
    }
    tile_init(tile2, TILE_DOWNSTAIRS);
    free(locations);
}

static void df_set_tile(dungeon_floor_t* const df, tiletype_t type, int x, int y) {
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
        t = df_tile_at(df, x + i, y);
        type = begin_type + (rand() % num_types);
        tile_init(t, type);
        t = df_tile_at(df, x + i, y + h);
        type = begin_type + (rand() % num_types);
        tile_init(t, type);
    }
    for (int i = 0; i <= h; i++) {
        t = df_tile_at(df, x, y + i);
        type = begin_type + (rand() % num_types);
        tile_init(t, type);
        t = df_tile_at(df, x + w, y + i);
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
    free(df);
    msuccess("Freed dungeon floor");
}

bool df_add_at(dungeon_floor_t* const df, entityid id, int x, int y) {
    massert(df, "df is NULL");
    bool retval = false;
    if (id == -1) {
        merror("id is -1");
        return false;
    }
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        merror("x or y out of bounds");
        return false;
    }
    retval = tile_add(&df->tiles[y][x], id) != -1;
    if (!retval) {
        merror("failed to add entity, tile_add failure");
        return false;
    }
    return retval;
}

bool df_remove_at(dungeon_floor_t* const df, entityid id, int x, int y) {
    massert(df, "df is NULL");
    if (id == -1) {
        merror("id is -1");
        return false;
    }
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        merror("x or y out of bounds");
        return false;
    }
    const entityid r = tile_remove(&df->tiles[y][x], id);
    return r != -1 && r == id;
}

tile_t* df_tile_at(const dungeon_floor_t* const df, const int x, const int y) {
    massert(df, "df is NULL");
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        merror("x or y out of bounds");
        return NULL;
    }
    return &df->tiles[y][x];
}

tiletype_t df_type_at(const dungeon_floor_t* const df, const int x, const int y) {
    massert(df, "df is NULL");
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        merror("x or y out of bounds");
        return TILE_NONE;
    }
    return df->tiles[y][x].type;
}

static void df_set_pressure_plate(dungeon_floor_t* const df, const int x, const int y, const int up_tx_key, const int dn_tx_key, const int event) {
    massert(df, "dungeon floor is NULL");
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) return;
    tile_set_pressure_plate(&df->tiles[y][x], true);
    tile_set_pressure_plate_up_tx_key(&df->tiles[y][x], up_tx_key);
    tile_set_pressure_plate_down_tx_key(&df->tiles[y][x], dn_tx_key);
    tile_set_pressure_plate_event(&df->tiles[y][x], event);
}

static void df_set_wall_switch(dungeon_floor_t* const df, int x, int y, int up_key, int dn_key, int event) {
    massert(df, "dungeon floor is NULL");
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) return;
    tile_t* const t = df_tile_at(df, x, y);
    if (!t) return;
    if (!tile_is_wall(t->type)) return;
    tile_set_wall_switch(t, true);
    tile_set_wall_switch_up_tx_key(t, up_key);
    tile_set_wall_switch_down_tx_key(t, dn_key);
    tile_set_wall_switch_event(t, event);
}

static void df_set_all_tiles(dungeon_floor_t* const df, tiletype_t type) {
    massert(df, "dungeon floor is NULL");
    // rewrite this function using df_set_tile_area
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
            tile_t* current = df_tile_at(df, j, i);
            tiletype_t type = begin_type + (rand() % num_types);
            tile_init(current, type);
        }
    }
}

bool df_tile_is_wall(const dungeon_floor_t* const df, int x, int y) {
    massert(df, "dungeon floor is NULL");
    tile_t* tile = df_tile_at(df, x, y);
    return tile_is_wall(tile->type);
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
            tile_t* current = df_tile_at(df, j, i);
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
            tile_t* current = df_tile_at(df, j, i);
            tiletype_t type = begin_type + (rand() % num_types);
            tile_init(current, type);
        }
    }
}

static void df_reset_plates(dungeon_floor_t* const df) {
    massert(df, "dungeon floor is NULL");
    for (int i = 0; i < DEFAULT_DF_PLATES; i++) df->plates[i] = false;
}

static void df_reset_events(dungeon_floor_t* const df) {
    massert(df, "dungeon floor is NULL");
    for (int i = 0; i < DEFAULT_DF_EVENTS; i++) {
        df->events[i].listen_event = -1;
        df->events[i].x = -1;
        df->events[i].y = -1;
        df->events[i].on_type = TILE_NONE;
        df->events[i].off_type = TILE_NONE;
    }
}

static bool df_malloc_tiles(dungeon_floor_t* const df) {
    massert(df, "dungeon floor is NULL");
    df->tiles = malloc(sizeof(tile_t*) * df->height);
    if (!df->tiles) return false;
    // memset the tiles
    memset(df->tiles, 0, sizeof(tile_t*) * df->height);
    for (int i = 0; i < df->height; i++) {
        df->tiles[i] = malloc(sizeof(tile_t) * df->width);
        if (df->tiles[i] == NULL) {
            // malloc failed and we need to free everything up to this point
            for (int j = 0; j < i; j++) { free(df->tiles[j]); }
            return false;
        }
    }
    return true;
}

static void df_init_rect(dungeon_floor_t* df, int x, int y, int w, int h, tiletype_t t1, tiletype_t t2) {
    massert(df, "dungeon floor is NULL");
    if (w <= 0 || h <= 0) return;
    df_set_tile_area_range(df, x, y, w, h, t1, t2);
}

static void df_init_rect2(dungeon_floor_t* df, Rectangle r, tiletype_t t1, tiletype_t t2) {
    massert(df, "dungeon floor is NULL");
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

loc_t df_get_upstairs(const dungeon_floor_t* const df) {
    massert(df, "dungeon floor is NULL");
    loc_t loc = {-1, -1};
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* const tile = df_tile_at(df, x, y);
            if (tile->type == TILE_UPSTAIRS) {
                loc.x = x;
                loc.y = y;
                return loc;
            }
        }
    }
    return loc;
}

loc_t df_get_downstairs(const dungeon_floor_t* const df) {
    massert(df, "dungeon floor is NULL");
    loc_t loc = {-1, -1};
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* const tile = df_tile_at(df, x, y);
            if (tile->type == TILE_DOWNSTAIRS) {
                loc.x = x;
                loc.y = y;
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

static void df_make_room(dungeon_floor_t* df, int x, int y, int w, int h) { df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11); }

static void df_make_corridor_h(dungeon_floor_t* df, int x, int y, int len) {
    df_set_tile_area_range(df, x, y, len, 1, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
}

static void df_make_corridor_v(dungeon_floor_t* df, int x, int y, int len) {
    df_set_tile_area_range(df, x, y, 1, len, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
}

static void df_connect_rooms(dungeon_floor_t* df, int x1, int y1, int x2, int y2) {
    // Connect by a 1-tile floor at midpoint to simulate door/corridor connection
    int mx = (x1 + x2) / 2;
    int my = (y1 + y2) / 2;
    df_set_tile(df, TILE_FLOOR_STONE_00, mx, my);
}

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

static void df_init_test_simple9(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    int cx = df_center_x(df), cy = df_center_y(df);
    int w = 3, h = 3;
    // Diamond-shaped central room
    df_make_diamond_shape_room(df, cx, cy, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
    df_assign_downstairs_in_area(df, cx - w / 2, cy - h / 2, w, h);
    df_assign_upstairs_in_area(df, cx - w / 2, cy - h / 2, w, h);
}
