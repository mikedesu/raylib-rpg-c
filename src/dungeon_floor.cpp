#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "dungeon_tile_type.h"
#include "entityid.h"
#include "libgame_defines.h"
#include "massert.h"
#include "range.h"
#include "raylib.h"
#include "room_data.h"
#include "vec3.h"
#include <stdlib.h>
#include <string.h>

void df_init_test(dungeon_floor_t* df);
void df_set_tile_perimeter_range(dungeon_floor_t* const df, tiletype_t begin, tiletype_t end, int x, int y, int w, int h);
void df_set_tile_perimeter(dungeon_floor_t* const df, tiletype_t type, int x, int y, int w, int h);
void df_set_tile(dungeon_floor_t* const df, tiletype_t type, int x, int y);
void df_set_all_tiles_range(dungeon_floor_t* const df, tiletype_t begin, tiletype_t end);
void df_set_all_tiles(dungeon_floor_t* const df, tiletype_t type);
void df_set_tile_area(dungeon_floor_t* const df, tiletype_t type, int x, int y, int w, int h);
void df_set_tile_area_range(dungeon_floor_t* const df, int x, int y, int w, int h, tiletype_t begin, tiletype_t end);
void df_set_tile_area_range2(dungeon_floor_t* const df, Rectangle r, tiletype_t begin, tiletype_t end);
void df_init_rect2(dungeon_floor_t* df, Rectangle r, tiletype_t t1, tiletype_t t2);
void df_init_rect(dungeon_floor_t* df, int x, int y, int w, int h, tiletype_t t1, tiletype_t t2);
bool df_malloc_tiles(dungeon_floor_t* const df);
void df_assign_stairs(dungeon_floor_t* df);
bool df_add_room(dungeon_floor_t* const df, int x, int y, int w, int h, tiletype_t begin, tiletype_t end, const char* room_name);
void df_assign_downstairs(dungeon_floor_t* df);
void df_assign_upstairs(dungeon_floor_t* df);
bool df_assign_upstairs_in_area(dungeon_floor_t* df, int x, int y, int w, int h);
bool df_assign_downstairs_in_area(dungeon_floor_t* df, int x, int y, int w, int h);
int df_get_possible_upstairs_count(dungeon_floor_t* df);
int df_get_possible_downstairs_count(dungeon_floor_t* df);
vec3* df_get_possible_upstairs_locs(dungeon_floor_t* df, int* external_count);
vec3* df_get_possible_downstairs_locs(dungeon_floor_t* df, int* external_count);
vec3* df_get_possible_upstairs_locs_in_area(dungeon_floor_t* df, int* external_count, int x, int y, int w, int h);
vec3* df_get_possible_downstairs_locs_in_area(dungeon_floor_t* df, int* external_count, int x, int y, int w, int h);
int df_get_possible_upstairs_count_in_area(dungeon_floor_t* df, int x, int y, int w, int h);
int df_get_possible_downstairs_count_in_area(dungeon_floor_t* df, int x, int y, int w, int h);
void df_make_room(dungeon_floor_t* df, int x, int y, int w, int h);
void df_make_corridor_h(dungeon_floor_t* df, int x, int y, int len);
void df_make_corridor_v(dungeon_floor_t* df, int x, int y, int len);
// void df_connect_rooms(dungeon_floor_t* df, int x1, int y1, int x2, int y2);
void df_make_diamond_shape_room(dungeon_floor_t* df, int cx, int cy, int w, int h, tiletype_t begin_type, tiletype_t end_type);

int df_get_possible_downstairs_count_in_area(dungeon_floor_t* df, int x, int y, int w, int h) {
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

int df_get_possible_upstairs_count_in_area(dungeon_floor_t* df, int x, int y, int w, int h) {
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

void df_assign_stairs(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    df_assign_upstairs_in_area(df, 0, 0, df->width, df->height);
    df_assign_downstairs_in_area(df, 0, 0, df->width, df->height);
}

int df_get_possible_upstairs_count(dungeon_floor_t* df) { return df_get_possible_upstairs_count_in_area(df, 0, 0, df->width, df->height); }

int df_get_possible_downstairs_count(dungeon_floor_t* df) { return df_get_possible_downstairs_count_in_area(df, 0, 0, df->width, df->height); }

vec3* df_get_possible_upstairs_locs(dungeon_floor_t* df, int* external_count) {
    massert(df, "dungeon floor is NULL");
    massert(external_count, "external_count is NULL");
    int count = -1;
    vec3* locs = df_get_possible_upstairs_locs_in_area(df, &count, 0, 0, df->width, df->height);
    massert(locs, "failed to get possible upstairs locations");
    massert(count > 0, "no possible upstairs locations");
    *external_count = count;
    return locs;
}

vec3* df_get_possible_downstairs_locs(dungeon_floor_t* df, int* external_count) {
    massert(df, "dungeon floor is NULL");
    massert(external_count, "external_count is NULL");
    int count = -1;
    vec3* locs = df_get_possible_downstairs_locs_in_area(df, &count, 0, 0, df->width, df->height);
    massert(locs, "failed to get possible downstairs locations");
    massert(count > 0, "no possible downstairs locations");
    *external_count = count;
    return locs;
}

vec3* df_get_possible_upstairs_locs_in_area(dungeon_floor_t* df, int* external_count, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    int count = df_get_possible_upstairs_count_in_area(df, x, y, w, h);
    vec3* locations = (vec3*)malloc(sizeof(vec3) * count);
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

vec3* df_get_possible_downstairs_locs_in_area(dungeon_floor_t* df, int* external_count, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    massert(external_count, "external_count is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    int count = df_get_possible_downstairs_count_in_area(df, x, y, w, h);
    vec3* locations = (vec3*)malloc(sizeof(vec3) * count);
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

void df_assign_upstairs(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    df_assign_upstairs_in_area(df, 0, 0, df->width, df->height);
}

void df_assign_downstairs(dungeon_floor_t* df) {
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

dungeon_floor_t* df_create(int floor, int width, int height) {
    massert(width > 0, "width must be greater than zero");
    massert(height > 0, "height must be greater than zero");
    massert(floor >= 0, "floor must be greater than or equal to zero");
    dungeon_floor_t* df = (dungeon_floor_t*)malloc(sizeof(dungeon_floor_t));
    massert(df, "failed to malloc dungeon floor");
    df->floor = floor;
    df->width = width;
    df->height = height;
    df_init(df);
    return df;
}

void df_set_tile_area(dungeon_floor_t* const df, tiletype_t type, int x, int y, int w, int h) {
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

//void df_set_event(dungeon_floor_t* const df, int x, int y, int event_id, tiletype_t on_type, tiletype_t off_type) {
//    massert(df, "dungeon floor is NULL");
//    massert(x >= 0, "x is less than zero");
//    massert(x < df->width, "x is out of bounds");
//    massert(y >= 0, "y is less than zero");
//    massert(y < df->height, "y is out of bounds");
//    massert(on_type >= 0, "on_type is out of bounds");
//    massert(on_type < TILE_COUNT, "on_type is out of bounds");
//    massert(off_type >= 0, "off_type is out of bounds");
//    massert(off_type < TILE_COUNT, "off_type is out of bounds");
//    massert(event_id >= 0, "event_id is less than zero");
//    massert(event_id < DEFAULT_DF_EVENTS, "event_id is out of bounds");
//massert(df->events, "events is NULL");
//df->events[event_id].x = x;
//df->events[event_id].y = y;
//df->events[event_id].on_type = on_type;
//df->events[event_id].off_type = off_type;
//df->events[event_id].listen_event = event_id;
//}

void df_init_test(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    // Rooms
    //int txwallup = TX_WALL_SWITCH_UP_00;
    //int txwalldown = TX_WALL_SWITCH_DOWN_00;
    //tiletype_t trap_on = TILE_FLOOR_STONE_TRAP_ON_00;
    //tiletype_t trap_off = TILE_FLOOR_STONE_TRAP_OFF_00;
    tiletype_t begin_type = TILE_FLOOR_STONE_00;
    tiletype_t end_type = TILE_FLOOR_STONE_11;
    //df_event_id id = 1;
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
    Rectangle rect = {(float)x, (float)y, (float)rand_w, (float)rand_h};
    rectangles[count] = rect;
    // now that we have our list of non-intersecting rectangles,
    // we can loop thru them and create the rooms
    for (int j = 0; j < total_rooms; j++) {
        Rectangle r = rectangles[j];
        df_init_rect2(df, r, begin_type, end_type);
        //df_place_wall_switch(df, r.x + rand() % (int)r.width, r.y + rand() % (int)r.height, txwallup, txwalldown, id);
        //df_create_trap_event(df, r.x + r.width, r.y + rand() % (int)r.height, trap_on, trap_off, id);
        //id++;
    }
    while (count < total_rooms) {
        df_init_rect2(df, (Rectangle){(float)x, (float)y, (float)w, (float)h}, begin_type, end_type);
        //df_place_wall_switch(df, x + rand() % w, y + rand() % h, txwallup, txwalldown, id);
        r = rand() % 2;
        //while (r == prev_r) { r = rand() % 4; }
        if (r == 0) {
            //df_create_trap_event(df, x + w, y + rand() % h, trap_on, trap_off, id);
            x = x + w + 1;
        } else if (r == 1) {
            //df_create_trap_event(df, x + rand() % w, y + h, trap_on, trap_off, id);
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
        //id++;
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
    vec3* locations = (vec3*)malloc(sizeof(vec3) * count2);
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

void df_set_tile_perimeter(dungeon_floor_t* const df, tiletype_t type, int x, int y, int w, int h) {
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

void df_set_tile_perimeter_range(dungeon_floor_t* const df, tiletype_t begin, tiletype_t end, int x, int y, int w, int h) {
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
        type = (tiletype_t)(begin_type + (rand() % num_types));
        tile_init(t, type);
        t = df_tile_at(df, (vec3){x + i, y + h, -1});
        type = (tiletype_t)(begin_type + (rand() % num_types));
        tile_init(t, type);
    }
    for (int i = 0; i <= h; i++) {
        t = df_tile_at(df, (vec3){x, y + i, -1});
        type = (tiletype_t)(begin_type + (rand() % num_types));
        tile_init(t, type);
        t = df_tile_at(df, (vec3){x + w, y + i, -1});
        type = (tiletype_t)(begin_type + (rand() % num_types));
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
    entityid result = tile_add(&df->tiles[y][x], id) != -1;
    massert(result != ENTITYID_INVALID, "tile_add failed");
    return result != ENTITYID_INVALID;
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

void df_set_pressure_plate(dungeon_floor_t* const df, const int x, const int y, const int up_tx_key, const int dn_tx_key, const int event) {
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    //tile_set_pressure_plate(&df->tiles[y][x], true);
    //tile_set_pressure_plate_up_tx_key(&df->tiles[y][x], up_tx_key);
    //tile_set_pressure_plate_down_tx_key(&df->tiles[y][x], dn_tx_key);
    //tile_set_pressure_plate_event(&df->tiles[y][x], event);
}

void df_set_wall_switch(dungeon_floor_t* const df, int x, int y, int up_key, int dn_key, int event) {
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) return;
    tile_t* const t = df_tile_at(df, (vec3){x, y, -1});
    if (!t) return;
    if (!tile_is_wall(t->type)) return;
    //tile_set_wall_switch(t, true);
    //tile_set_wall_switch_up_tx_key(t, up_key);
    //tile_set_wall_switch_down_tx_key(t, dn_key);
    //tile_set_wall_switch_event(t, event);
}

void df_set_all_tiles(dungeon_floor_t* const df, tiletype_t type) {
    massert(df, "dungeon floor is NULL");
    df_set_tile_area(df, type, 0, 0, df->width, df->height);
}

void df_set_all_tiles_range(dungeon_floor_t* const df, tiletype_t begin, tiletype_t end) {
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
            tiletype_t type = (tiletype_t)(begin_type + (rand() % num_types));
            tile_init(current, type);
        }
    }
}

void df_set_tile_area_range2(dungeon_floor_t* const df, Rectangle r, tiletype_t begin, tiletype_t end) {
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
            tiletype_t type = (tiletype_t)(begin_type + (rand() % num_types));
            tile_init(current, type);
        }
    }
}

void df_set_tile_area_range(dungeon_floor_t* const df, int x, int y, int w, int h, tiletype_t begin, tiletype_t end) {
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
            tiletype_t type = (tiletype_t)(begin_type + (rand() % num_types));
            tile_init(current, type);
        }
    }
}

bool df_malloc_tiles(dungeon_floor_t* const df) {
    massert(df, "dungeon floor is NULL");
    massert(df->width > 0, "width is less than zero");
    massert(df->height > 0, "height is less than zero");
    df->tiles = (tile_t**)malloc(sizeof(tile_t*) * df->height);
    massert(df->tiles, "failed to malloc tiles");
    memset(df->tiles, 0, sizeof(tile_t*) * df->height);
    for (int i = 0; i < df->height; i++) {
        df->tiles[i] = (tile_t*)malloc(sizeof(tile_t) * df->width);
        // malloc failed and we need to free everything up to this point
        if (df->tiles[i] == NULL) {
            for (int j = 0; j < i; j++) free(df->tiles[j]);
            return false;
        }
    }
    return true;
}

void df_init_rect(dungeon_floor_t* df, int x, int y, int w, int h, tiletype_t t1, tiletype_t t2) {
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    massert(w > 0, "w is less than zero");
    massert(h > 0, "h is less than zero");
    df_set_tile_area_range(df, x, y, w, h, t1, t2);
}

void df_init_rect2(dungeon_floor_t* df, Rectangle r, tiletype_t t1, tiletype_t t2) {
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

//void df_create_trap_event(dungeon_floor_t* df, int x, int y, tiletype_t on, tiletype_t off, df_event_id id) {
//    massert(df, "dungeon floor is NULL");
//    if (id >= DEFAULT_DF_EVENTS) return;
//    //df->events[id] = (df_event_t){.listen_event = id, .x = x, .y = y, .on_type = on, .off_type = off};
//    df_set_tile(df, off, x, y);
//}

//void df_place_wall_switch(dungeon_floor_t* df, int x, int y, int up, int down, df_event_id trigger_id) {
//    massert(df, "dungeon floor is NULL");
//    tiletype_t type = TILE_STONE_WALL_02;
//    df_set_tile(df, type, x, y);
//    df_set_wall_switch(df, x, y, up, down, trigger_id);
//}

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

void df_make_room(dungeon_floor_t* df, int x, int y, int w, int h) { df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11); }
void df_make_corridor_h(dungeon_floor_t* df, int x, int y, int len) { df_set_tile_area_range(df, x, y, len, 1, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11); }
void df_make_corridor_v(dungeon_floor_t* df, int x, int y, int len) { df_set_tile_area_range(df, x, y, 1, len, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11); }

//static void df_connect_rooms(dungeon_floor_t* df, int x1, int y1, int x2, int y2) {
// Connect by a 1-tile floor at midpoint to simulate door/corridor connection
//    int mx = (x1 + x2) / 2;
//    int my = (y1 + y2) / 2;
//    df_set_tile(df, TILE_FLOOR_STONE_00, mx, my);
//}

void df_make_diamond_shape_room(dungeon_floor_t* df, int cx, int cy, int w, int h, tiletype_t begin_type, tiletype_t end_type) {
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
            tiletype_t t = (tiletype_t)(begin_type + rand() % (end_type - begin_type + 1));
            df_set_tile(df, t, x, y);
        }
    }
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

void df_init_rooms(dungeon_floor_t* df) {
    massert(df, "dungeon floor is NULL");
    const int default_capacity = 10;
    df->rooms = (room_data_t*)malloc(sizeof(room_data_t) * default_capacity);
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
        room_data_t* tmp = (room_data_t*)realloc(df->rooms, sizeof(room_data_t) * new_cap);
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
    vec3* locs = (vec3*)malloc(sizeof(vec3) * count);
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
    vec3* outside_locs = (vec3*)malloc(sizeof(vec3) * count);
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
    room_data_t* rooms = (room_data_t*)malloc(sizeof(room_data_t) * count);
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
