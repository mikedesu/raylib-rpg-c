#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "dungeon_tile_type.h"
#include "entityid.h"
#include "massert.h"
#include "raylib.h"
#include "vec3.h"
#include <cstdlib>
#include <cstring>


using std::make_shared;
using std::shared_ptr;
using std::unordered_map;
using std::vector;


void df_init_test(shared_ptr<dungeon_floor_t> df);
void df_set_tile_perimeter_range(shared_ptr<dungeon_floor_t> const df, tiletype_t begin, tiletype_t end, int x, int y, int w, int h);
void df_set_tile_perimeter(shared_ptr<dungeon_floor_t> const df, tiletype_t type, int x, int y, int w, int h);
void df_set_tile(shared_ptr<dungeon_floor_t> const df, tiletype_t type, int x, int y);
void df_set_all_tiles_range(shared_ptr<dungeon_floor_t> const df, tiletype_t begin, tiletype_t end);
void df_set_all_tiles(shared_ptr<dungeon_floor_t> const df, tiletype_t type);
void df_set_tile_area(shared_ptr<dungeon_floor_t> const df, tiletype_t type, int x, int y, int w, int h);
void df_set_tile_area_range2(shared_ptr<dungeon_floor_t> const df, Rectangle r, tiletype_t begin, tiletype_t end);
void df_init_rect2(shared_ptr<dungeon_floor_t> df, Rectangle r, tiletype_t t1, tiletype_t t2);
void df_init_rect(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h, tiletype_t t1, tiletype_t t2);
void df_assign_stairs(shared_ptr<dungeon_floor_t> df);
void df_assign_downstairs(shared_ptr<dungeon_floor_t> df);
void df_assign_upstairs(shared_ptr<dungeon_floor_t> df);
bool df_assign_upstairs_in_area(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h);
bool df_assign_downstairs_in_area(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h);
int df_get_possible_upstairs_count(shared_ptr<dungeon_floor_t> df);
int df_get_possible_downstairs_count(shared_ptr<dungeon_floor_t> df);
vec3* df_get_possible_upstairs_locs(shared_ptr<dungeon_floor_t> df, int* external_count);
vec3* df_get_possible_downstairs_locs(shared_ptr<dungeon_floor_t> df, int* external_count);
vec3* df_get_possible_upstairs_locs_in_area(shared_ptr<dungeon_floor_t> df, int* external_count, int x, int y, int w, int h);
vec3* df_get_possible_downstairs_locs_in_area(shared_ptr<dungeon_floor_t> df, int* external_count, int x, int y, int w, int h);
int df_get_possible_upstairs_count_in_area(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h);
int df_get_possible_downstairs_count_in_area(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h);
void df_make_room(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h);


int df_get_possible_downstairs_count_in_area(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h) {
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
            //tile_t* const tile = df_tile_at(df, (vec3){x0, y0, -1});

            shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x0, y0, -1});
            massert(tile, "tile is NULL at (%d, %d)", x0, y0);
            // check if the tile is possible downstairs
            if (tile_is_possible_downstairs(tile->type)) count++;
        }
    }
    return count;
}

int df_get_possible_upstairs_count_in_area(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h) {
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
            //tile_t* const tile = df_tile_at(df, (vec3){x0, y0, -1});
            shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x0, y0, -1});
            if (tile_is_possible_upstairs(tile->type)) count++;
        }
    }
    return count;
}

void df_assign_stairs(shared_ptr<dungeon_floor_t> df) {
    massert(df, "dungeon floor is NULL");
    df_assign_upstairs_in_area(df, 0, 0, df->width, df->height);
    df_assign_downstairs_in_area(df, 0, 0, df->width, df->height);
}

int df_get_possible_upstairs_count(shared_ptr<dungeon_floor_t> df) { return df_get_possible_upstairs_count_in_area(df, 0, 0, df->width, df->height); }

int df_get_possible_downstairs_count(shared_ptr<dungeon_floor_t> df) { return df_get_possible_downstairs_count_in_area(df, 0, 0, df->width, df->height); }

vec3* df_get_possible_upstairs_locs(shared_ptr<dungeon_floor_t> df, int* external_count) {
    massert(df, "dungeon floor is NULL");
    massert(external_count, "external_count is NULL");
    int count = -1;
    vec3* locs = df_get_possible_upstairs_locs_in_area(df, &count, 0, 0, df->width, df->height);
    massert(locs, "failed to get possible upstairs locations");
    massert(count > 0, "no possible upstairs locations");
    *external_count = count;
    return locs;
}

vec3* df_get_possible_downstairs_locs(shared_ptr<dungeon_floor_t> df, int* external_count) {
    massert(df, "dungeon floor is NULL");
    massert(external_count, "external_count is NULL");
    int count = -1;
    vec3* locs = df_get_possible_downstairs_locs_in_area(df, &count, 0, 0, df->width, df->height);
    massert(locs, "failed to get possible downstairs locations");
    massert(count > 0, "no possible downstairs locations");
    *external_count = count;
    return locs;
}

vec3* df_get_possible_upstairs_locs_in_area(shared_ptr<dungeon_floor_t> df, int* external_count, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    int count = df_get_possible_upstairs_count_in_area(df, x, y, w, h);
    vec3* locations = (vec3*)malloc(sizeof(vec3) * count);
    massert(locations, "failed to malloc locations");
    int count2 = 0;
    // now we can loop thru the dungeon floor again and fill the array with the locations
    for (int y0 = y; y0 < df->height; y0++) {
        for (int x0 = x; x0 < df->width; x0++) {
            //tile_t* const tile = df_tile_at(df, (vec3){x0, y0, -1});
            shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x0, y0, -1});
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

vec3* df_get_possible_downstairs_locs_in_area(shared_ptr<dungeon_floor_t> df, int* external_count, int x, int y, int w, int h) {
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
            //tile_t* const tile = df_tile_at(df, (vec3){x0, y0, -1});
            shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x0, y0, -1});
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

bool df_assign_upstairs_in_area(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h) {
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
    //tile_t* const tile = df_tile_at(df, up_loc);
    shared_ptr<tile_t> tile = df_tile_at(df, up_loc);
    massert(tile, "failed to get tile");
    tile_init(tile, TILE_UPSTAIRS);
    tile->visible = true; // make sure the upstairs tile is visible
    free(locations);
    df->upstairs_loc = up_loc;
    return true;
}

void df_assign_upstairs(shared_ptr<dungeon_floor_t> df) {
    massert(df, "dungeon floor is NULL");
    df_assign_upstairs_in_area(df, 0, 0, df->width, df->height);
}

void df_assign_downstairs(shared_ptr<dungeon_floor_t> df) {
    massert(df, "dungeon floor is NULL");
    df_assign_downstairs_in_area(df, 0, 0, df->width, df->height);
}

bool df_assign_downstairs_in_area(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h) {
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
    //tile_t* const tile = df_tile_at(df, down_loc);
    shared_ptr<tile_t> tile = df_tile_at(df, down_loc);
    massert(tile, "failed to get tile");
    tile_init(tile, TILE_DOWNSTAIRS);
    tile->visible = true; // make sure the upstairs tile is visible
    free(locations);
    df->downstairs_loc = down_loc;
    return true;
}

int df_center_x(const shared_ptr<dungeon_floor_t> df) {
    massert(df, "dungeon floor is NULL");
    return df->width / 2;
}

int df_center_y(const shared_ptr<dungeon_floor_t> df) {
    massert(df, "dungeon floor is NULL");
    return df->height / 2;
}

shared_ptr<dungeon_floor_t> df_create(int floor, int width, int height) {
    massert(width > 0, "width must be greater than zero");
    massert(height > 0, "height must be greater than zero");
    massert(floor >= 0, "floor must be greater than or equal to zero");

    //dungeon_floor_t* df = (dungeon_floor_t*)malloc(sizeof(dungeon_floor_t));
    shared_ptr<dungeon_floor_t> df = make_shared<dungeon_floor_t>();
    massert(df, "failed to malloc dungeon floor");

    df->floor = floor;
    df->width = width;
    df->height = height;

    //df->tiles = make_shared<vector<tile_id>>(width * height, TILE_NONE);
    df->tiles = make_shared<vector<tile_id>>();
    massert(df->tiles, "failed to create tiles vector");

    df->tile_map = make_shared<unordered_map<tile_id, shared_ptr<tile_t>>>();
    massert(df->tile_map, "failed to create tile map");

    for (tile_id i = 0; i < width * height; i++) {
        df->tiles->push_back(i);

        shared_ptr<tile_t> tile = make_shared<tile_t>();

        massert(tile, "failed to create tile");

        tile_init(tile, TILE_FLOOR_STONE_00);

        tile->id = i;
        ////tile->type = TILE_NONE;
        // for each tile in the dungeon floor, select a random tile between TILE_FLOOR_STONE_00 and 11
        tile->type = (tiletype_t)(TILE_FLOOR_STONE_00 + (rand() % (TILE_FLOOR_STONE_11 - TILE_FLOOR_STONE_00 + 1)));
        //tile->type = TILE_FLOOR_STONE_00;
        tile->visible = true;

        df->tile_map->insert({i, tile});
    }

    df->upstairs_loc = (vec3){-1, -1, -1};
    df->downstairs_loc = (vec3){-1, -1, -1};

    //df_init(df);
    msuccess("Created dungeon floor %d with dimensions %dx%d", floor, width, height);
    return df;
}

void df_set_tile_area(shared_ptr<dungeon_floor_t> const df, tiletype_t type, int x, int y, int w, int h) {
    minfo("df_set_tile_area: Setting tile area at (%d, %d) with size %dx%d to type %d", x, y, w, h, type);
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    massert(w > 0, "w is less than zero");
    massert(h > 0, "h is less than zero");
    massert(x + w <= df->width, "x + w is out of bounds");
    massert(y + h <= df->height, "y + h is out of bounds");

    minfo("entering loop...");
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            //tile_t* current = df_tile_at(df, (vec3){j, i, -1});
            shared_ptr<tile_t> current = df_tile_at(df, (vec3){j, i, -1});
            tile_init(current, type);
        }
    }
}

//void df_init(shared_ptr<dungeon_floor_t> df) {
//    minfo("df_init: Initializing dungeon floor %d with dimensions %dx%d", df->floor, df->width, df->height);
//    massert(df, "dungeon floor is NULL");
//df_set_all_tiles(df, TILE_NONE);
// at this point, we are free to customize the dungeon floor to our liking
//}

void df_set_tile(shared_ptr<dungeon_floor_t> const df, tiletype_t type, int x, int y) {
    massert(df, "dungeon floor is NULL");
    shared_ptr<tile_t> current = df_tile_at(df, (vec3){x, y, -1});
    tile_init(current, type);
}

void df_set_tile_perimeter(shared_ptr<dungeon_floor_t> const df, tiletype_t type, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    for (int i = 0; i <= w; i++) {
        shared_ptr<tile_t> current = df_tile_at(df, (vec3){x + i, y, -1});
        tile_init(current, type);
    }
    for (int i = 0; i <= h; i++) {
        shared_ptr<tile_t> current = df_tile_at(df, (vec3){x, y + i, -1});
        tile_init(current, type);
    }
    for (int i = 0; i <= w; i++) {
        shared_ptr<tile_t> current = df_tile_at(df, (vec3){x + i, y + h, -1});
        tile_init(current, type);
    }
    for (int i = 0; i <= h; i++) {
        shared_ptr<tile_t> current = df_tile_at(df, (vec3){x + w, y + i, -1});
        tile_init(current, type);
    }
}

void df_set_tile_perimeter_range(shared_ptr<dungeon_floor_t> const df, tiletype_t begin, tiletype_t end, int x, int y, int w, int h) {
    massert(df, "dungeon floor is NULL");
    tiletype_t begin_type = begin;
    tiletype_t end_type = end;
    if (end_type < begin_type) {
        tiletype_t temp = begin_type;
        begin_type = end_type;
        end_type = temp;
    }
    int num_types = end_type - begin_type + 1;
    //tile_t* t = NULL;
    shared_ptr<tile_t> t = nullptr;
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

void df_free(shared_ptr<dungeon_floor_t> df) {
    massert(df, "dungeon floor is NULL");

    if (df->tiles) {
        df->tiles->clear();
    }

    if (df->tile_map) {
        df->tile_map->clear();
    }

    //free(df);
}

//bool df_add_at(shared_ptr<dungeon_floor_t> const df, entityid id, int x, int y) {
entityid df_add_at(shared_ptr<dungeon_floor_t> const df, entityid id, int x, int y) {
    massert(df, "df is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x, y, -1});
    massert(tile, "tile is NULL at (%d, %d)", x, y);
    entityid result = tile_add(tile, id);
    return result;
}

bool df_remove_at(shared_ptr<dungeon_floor_t> const df, entityid id, int x, int y) {
    massert(df, "df is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    //const entityid r = tile_remove(&df->tiles[y][x], id);
    shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x, y, -1});
    massert(tile, "tile is NULL at (%d, %d)", x, y);
    entityid r = tile_remove(tile, id);
    return r != -1 && r == id;
}

void df_set_all_tiles(shared_ptr<dungeon_floor_t> const df, tiletype_t type) {
    minfo("df_set_all_tiles: Setting all tiles to type %d", type);
    massert(df, "dungeon floor is NULL");
    df_set_tile_area(df, type, 0, 0, df->width, df->height);
}

void df_set_all_tiles_range(shared_ptr<dungeon_floor_t> const df, tiletype_t begin, tiletype_t end) {
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
            //tile_t* current = df_tile_at(df, (vec3){j, i, -1});
            shared_ptr<tile_t> current = df_tile_at(df, (vec3){j, i, -1});
            massert(current, "tile is NULL at (%d, %d)", j, i);
            tiletype_t type = (tiletype_t)(begin_type + (rand() % num_types));
            tile_init(current, type);
        }
    }
}

void df_set_tile_area_range2(shared_ptr<dungeon_floor_t> const df, Rectangle r, tiletype_t begin, tiletype_t end) {
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
            //tile_t* current = df_tile_at(df, (vec3){j, i, -1});
            shared_ptr<tile_t> current = df_tile_at(df, (vec3){j, i, -1});
            tiletype_t type = (tiletype_t)(begin_type + (rand() % num_types));
            tile_init(current, type);
        }
    }
}

void df_set_tile_area_range(shared_ptr<dungeon_floor_t> const df, int x, int y, int w, int h, tiletype_t begin, tiletype_t end) {
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
            //tile_t* current = df_tile_at(df, (vec3){j, i, -1});
            shared_ptr<tile_t> current = df_tile_at(df, (vec3){j, i, -1});
            tiletype_t type = (tiletype_t)(begin_type + (rand() % num_types));
            tile_init(current, type);
        }
    }
}

void df_init_rect(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h, tiletype_t t1, tiletype_t t2) {
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");
    massert(w > 0, "w is less than zero");
    massert(h > 0, "h is less than zero");
    df_set_tile_area_range(df, x, y, w, h, t1, t2);
}

void df_init_rect2(shared_ptr<dungeon_floor_t> df, Rectangle r, tiletype_t t1, tiletype_t t2) {
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

vec3 df_get_upstairs(shared_ptr<dungeon_floor_t> const df) {
    massert(df, "dungeon floor is NULL");
    if (df->upstairs_loc.x != -1 && df->upstairs_loc.y != -1) {
        return df->upstairs_loc;
    }
    vec3 loc = {-1, -1, -1};
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            //tile_t* const tile = df_tile_at(df, (vec3){x, y, -1});
            shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x, y, -1});
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

vec3 df_get_downstairs(shared_ptr<dungeon_floor_t> const df) {
    massert(df, "dungeon floor is NULL");
    if (df->downstairs_loc.x != -1 && df->downstairs_loc.y != -1) return df->downstairs_loc;
    vec3 loc = {-1, -1, -1};
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            //tile_t* const tile = df_tile_at(df, (vec3){x, y, -1});
            shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x, y, -1});
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

int df_count_walkable(const shared_ptr<dungeon_floor_t> df) {
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

int df_count_empty(const shared_ptr<dungeon_floor_t> df) {
    massert(df, "df is NULL");
    int count = 0;
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            shared_ptr<tile_t> t = df_tile_at(df, (vec3){x, y, -1});
            if (!t) continue;
            if (t->entities->size() == 0) count++;
        }
    }
    return count;
}

int df_count_non_walls_in_area(const shared_ptr<dungeon_floor_t> df, int x0, int y0, int w, int h) {
    //massert(df, "df is NULL");
    int count = 0;
    if (!df) return 0;
    for (int y = 0; y < h && y + y0 < df->height; y++) {
        for (int x = 0; x < w && x + x0 < df->width; x++) {
            int newx = x + x0;
            int newy = y + y0;
            //tile_t* const t = df_tile_at(df, (vec3){newx, newy, -1});
            shared_ptr<tile_t> t = df_tile_at(df, (vec3){newx, newy, -1});
            massert(t, "tile is NULL at (%d, %d)", newx, newy);
            if (tile_is_walkable(t->type)) count++;
        }
    }
    return count;
}

int df_count_non_walls(const shared_ptr<dungeon_floor_t> df) { return df_count_non_walls_in_area(df, 0, 0, df->width, df->height); }


int df_count_empty_non_walls_in_area(const shared_ptr<dungeon_floor_t> df, int x0, int y0, int w, int h) {
    int count = 0;
    if (!df) return 0;
    for (int y = 0; y < h && y + y0 < df->height; y++) {
        for (int x = 0; x < w && x + x0 < df->width; x++) {
            shared_ptr<tile_t> t = df_tile_at(df, (vec3){x + x0, y + y0, -1});
            if (!t) continue;
            if (t->entities->size() == 0 && tile_is_walkable(t->type)) count++;
        }
    }
    return count;
}

int df_count_empty_non_walls(const shared_ptr<dungeon_floor_t> df) { return df_count_empty_non_walls_in_area(df, 0, 0, df->width, df->height); }


void df_make_room(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h) {
    df_set_tile_area_range(df, x, y, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11);
}
