#include "dungeon_floor.h"
#include "dungeon_floor_type.h"
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


void df_set_tile(shared_ptr<dungeon_floor_t> const df, tiletype_t type, int x, int y);
void df_set_all_tiles(shared_ptr<dungeon_floor_t> const df, tiletype_t type);
void df_set_tile_area(shared_ptr<dungeon_floor_t> df, tiletype_t type, int x, int y, int w, int h);
void df_assign_stairs(shared_ptr<dungeon_floor_t> df);
void df_assign_downstairs(shared_ptr<dungeon_floor_t> df);
void df_assign_upstairs(shared_ptr<dungeon_floor_t> df);
bool df_assign_upstairs_in_area(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h);
bool df_assign_downstairs_in_area(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h);
int df_get_possible_upstairs_count(shared_ptr<dungeon_floor_t> df);
int df_get_possible_downstairs_count(shared_ptr<dungeon_floor_t> df);


shared_ptr<vector<vec3>> df_get_possible_downstairs_locs(shared_ptr<dungeon_floor_t> df);
shared_ptr<vector<vec3>> df_get_possible_downstairs_locs_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r);

shared_ptr<vector<vec3>> df_get_possible_upstairs_locs(shared_ptr<dungeon_floor_t> df);
shared_ptr<vector<vec3>> df_get_possible_upstairs_locs_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r);

int df_get_possible_upstairs_count_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r);
int df_get_possible_downstairs_count_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r);
void df_set_area(shared_ptr<dungeon_floor_t> df, tiletype_t t, Rectangle r);


shared_ptr<dungeon_floor_t> df_create(int floor, dungeon_floor_type_t t, int width, int height) {
    massert(width > 0, "width must be greater than zero");
    massert(height > 0, "height must be greater than zero");
    massert(floor >= 0, "floor must be greater than or equal to zero");

    // creating a new dungeon floor
    // this was likely called by d_add_floor
    shared_ptr<dungeon_floor_t> df = make_shared<dungeon_floor_t>();
    massert(df, "failed to malloc dungeon floor");

    df->floor = floor;
    df->width = width;
    df->height = height;

    df->tiles = make_shared<vector<tile_id>>();
    massert(df->tiles, "failed to create tiles vector");

    df->tile_map = make_shared<unordered_map<tile_id, shared_ptr<tile_t>>>();
    massert(df->tile_map, "failed to create tile map");

    df->type = t;

    for (tile_id i = 0; i < width * height; i++) {
        df->tiles->push_back(i);
        shared_ptr<tile_t> tile = make_shared<tile_t>();
        massert(tile, "failed to create tile");
        tile->id = i;
        tile_init(tile, TILE_NONE);
        df->tile_map->insert({i, tile});
    }

    // locate "center"
    // in the 24x24 case,
    // this would be x,y = 8,8

    df->upstairs_loc = (vec3){-1, -1, -1};
    df->downstairs_loc = (vec3){-1, -1, -1};

    // this is hard-coded atm but we will move it into a function
    const float x = width / 3.0;
    const float y = height / 3.0;

    // off by 1...want 8 width? set it to 9
    const int w = 4;
    const int h = 4;

    // init the inner MxN area
    // create a new room with walls
    df_set_area(df, TILE_FLOOR_STONE_00, (Rectangle){x, y, w, h});
    // create a new room with walls
    df_set_area(df, TILE_FLOOR_STONE_00, (Rectangle){x + w + 1, y, w, h});
    // create an opening into the new room
    df_set_area(df, TILE_FLOOR_STONE_00, (Rectangle){x + w, y + h - 1, 1, 1});
    //df_set_perimeter(df, x0, y0, w + 1, h + 1);

    msuccess("Created dungeon floor %d with dimensions %dx%d", floor, width, height);
    return df;
}


void df_set_area(shared_ptr<dungeon_floor_t> df, tiletype_t t, Rectangle r) {
    for (int x = r.x; x < r.x + r.width; x++) {
        for (int y = r.y; y < r.y + r.height; y++) {
            df_set_tile(df, t, x, y);
        }
    }
}


void df_set_perimeter(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h) {
    for (int x0 = x; x0 < x + w; x0++) {
        df_set_tile(df, TILE_STONE_WALL_00, x0, y);
        df_set_tile(df, TILE_STONE_WALL_00, x0, y + h);
    }

    for (int y0 = y; y0 < y + h; y0++) {
        df_set_tile(df, TILE_STONE_WALL_00, x, y0);
        df_set_tile(df, TILE_STONE_WALL_00, x + w, y0);
    }
}


int df_get_possible_downstairs_count_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r) {
    massert(df, "dungeon floor is NULL");
    massert(r.x >= 0, "x is less than zero");
    massert(r.x < df->width, "x is out of bounds");
    massert(r.y >= 0, "y is less than zero");
    massert(r.y < df->height, "y is out of bounds");
    massert(r.width > 0, "w is less than zero");
    massert(r.height > 0, "h is less than zero");
    massert(r.x + r.width <= df->width, "x + w is out of bounds");
    massert(r.y + r.height <= df->height, "y + h is out of bounds");

    int count = 0;
    for (int y0 = r.y; y0 < df->height; y0++) {
        for (int x0 = r.x; x0 < df->width; x0++) {
            shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x0, y0, -1});
            massert(tile, "tile is NULL at (%d, %d)", x0, y0);
            // check if the tile is possible downstairs
            if (tile_is_possible_downstairs(tile->type)) {
                count++;
            }
        }
    }

    return count;
}


int df_get_possible_upstairs_count_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r) {
    massert(df, "dungeon floor is NULL");
    massert(r.x >= 0, "x is less than zero");
    massert(r.x < df->width, "x is out of bounds");
    massert(r.y >= 0, "y is less than zero");
    massert(r.y < df->height, "y is out of bounds");
    massert(r.width > 0, "w is less than zero");
    massert(r.height > 0, "h is less than zero");
    massert(r.x + r.width <= df->width, "x + w is out of bounds");
    massert(r.y + r.height <= df->height, "y + h is out of bounds");

    int count = 0;
    for (int y0 = r.y; y0 < df->height; y0++) {
        for (int x0 = r.x; x0 < df->width; x0++) {
            shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x0, y0, -1});
            if (tile_is_possible_upstairs(tile->type)) {
                count++;
            }
        }
    }
    return count;
}


void df_assign_stairs(shared_ptr<dungeon_floor_t> df) {
    massert(df, "dungeon floor is NULL");
    df_assign_upstairs_in_area(df, 0, 0, df->width, df->height);
    df_assign_downstairs_in_area(df, 0, 0, df->width, df->height);
}


int df_get_possible_upstairs_count(shared_ptr<dungeon_floor_t> df) {
    return df_get_possible_upstairs_count_in_area(df, (Rectangle){0, 0, (float)df->width, (float)df->height});
}


int df_get_possible_downstairs_count(shared_ptr<dungeon_floor_t> df) {
    return df_get_possible_downstairs_count_in_area(df, (Rectangle){0, 0, (float)df->width, (float)df->height});
}


shared_ptr<vector<vec3>> df_get_possible_upstairs_locs(shared_ptr<dungeon_floor_t> df) {
    massert(df, "dungeon floor is NULL");
    //massert(external_count, "external_count is NULL");

    //int count = -1;

    //auto locs = df_get_possible_upstairs_locs_in_area(df, 0, 0, df->width, df->height);
    auto locs = df_get_possible_upstairs_locs_in_area(df, (Rectangle){0, 0, (float)df->width, (float)df->height});
    massert(locs, "failed to get possible upstairs locations");

    return locs;
}


shared_ptr<vector<vec3>> df_get_possible_downstairs_locs(shared_ptr<dungeon_floor_t> df) {
    massert(df, "dungeon floor is NULL");

    auto locs = df_get_possible_downstairs_locs_in_area(df, (Rectangle){0, 0, (float)df->width, (float)df->height});
    massert(locs, "failed to get possible downstairs locations");

    return locs;
}


shared_ptr<vector<vec3>> df_get_possible_upstairs_locs_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r) {
    massert(df, "dungeon floor is NULL");
    int count = df_get_possible_upstairs_count_in_area(df, r);

    shared_ptr<vector<vec3>> locations = make_shared<vector<vec3>>();
    massert(locations, "failed to make_shared locations");

    // now we can loop thru the dungeon floor again and fill the array with the locations
    for (int y0 = r.y; y0 < df->height; y0++) {
        for (int x0 = r.x; x0 < df->width; x0++) {
            shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x0, y0, -1});
            if (tile_is_possible_upstairs(tile->type)) {
                // there wont be any entities yet so do not check for them
                locations->push_back((vec3){x0, y0, 0});
                massert(locations->size() <= (size_t)count, "count2 is greater than count");
            }
        }
    }
    massert(locations->size() == (size_t)count, "count2 is greater than count");
    //*external_count = count;
    return locations;
}


shared_ptr<vector<vec3>> df_get_possible_downstairs_locs_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r) {
    massert(df, "dungeon floor is NULL");
    massert(r.x >= 0, "x is less than zero");
    massert(r.x < df->width, "x is out of bounds");
    massert(r.y >= 0, "y is less than zero");
    massert(r.y < df->height, "y is out of bounds");

    shared_ptr<vector<vec3>> locations = make_shared<vector<vec3>>();
    massert(locations, "failed to make_shared locations");

    // now we can loop thru the dungeon floor again and fill the array with the locations
    for (int y0 = r.y; y0 < df->height; y0++) {
        for (int x0 = r.x; x0 < df->width; x0++) {
            shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x0, y0, -1});
            if (tile_is_possible_downstairs(tile->type)) {
                // there wont be any entities yet so do not check for them
                locations->push_back((vec3){x0, y0, 0});
            }
        }
    }

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
    //int count = -1;

    //vec3* locations = df_get_possible_upstairs_locs_in_area(df, &count, x, y, w, h);
    //shared_ptr<vector<vec3>> locations = df_get_possible_upstairs_locs_in_area(df, x, y, w, h);
    auto locations = df_get_possible_upstairs_locs_in_area(df, (Rectangle){(float)x, (float)y, (float)w, (float)h});
    massert(locations->size() > 0, "no possible upstairs locations");

    // now that we have a list of possible locations for the upstairs to appear
    // we can randomly select one of them
    const int upstairs_index = rand() % locations->size();

    const vec3 up_loc = locations->at(upstairs_index);

    // now we can set the upstairs tile
    shared_ptr<tile_t> tile = df_tile_at(df, up_loc);
    massert(tile, "failed to get tile");

    tile_init(tile, TILE_UPSTAIRS);

    // make sure the upstairs tile is visible
    tile->visible = true;

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
    auto locations = df_get_possible_downstairs_locs_in_area(df, (Rectangle){(float)x, (float)y, (float)w, (float)h});
    // now that we have a list of possible locations for the upstairs to appear
    // we can randomly select one of them
    int downstairs_index = rand() % count;
    vec3 down_loc = locations->at(downstairs_index);
    // now we can set the upstairs tile
    shared_ptr<tile_t> tile = df_tile_at(df, down_loc);
    massert(tile, "failed to get tile");
    tile_init(tile, TILE_DOWNSTAIRS);
    tile->visible = true; // make sure the upstairs tile is visible
    //free(locations);
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


void df_set_tile(shared_ptr<dungeon_floor_t> const df, tiletype_t type, int x, int y) {
    massert(df, "dungeon floor is NULL");
    shared_ptr<tile_t> current = df_tile_at(df, (vec3){x, y, -1});
    tile_init(current, type);
}


void df_free(shared_ptr<dungeon_floor_t> df) {
    massert(df, "dungeon floor is NULL");

    if (df->tiles) {
        df->tiles->clear();
    }

    if (df->tile_map) {
        df->tile_map->clear();
    }
}


entityid df_add_at(shared_ptr<dungeon_floor_t> const df, entityid id, int x, int y) {
    massert(df, "df is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");

    shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x, y, -1});
    massert(tile, "tile is NULL at (%d, %d)", x, y);

    return tile_add(tile, id);
}

bool df_remove_at(shared_ptr<dungeon_floor_t> const df, entityid id, int x, int y) {
    massert(df, "df is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(x >= 0, "x is less than zero");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is less than zero");
    massert(y < df->height, "y is out of bounds");

    shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x, y, -1});
    massert(tile, "tile is NULL at (%d, %d)", x, y);

    const entityid r = tile_remove(tile, id);

    if (r == ENTITYID_INVALID) {
        merror("df_remove_at: Failed to remove entity %d at (%d, %d)", id, x, y);
        return false;
    }

    if (r != id) {
        merror("df_remove_at: Removed entity %d but expected to remove %d at (%d, %d)", r, id, x, y);
        return false;
    }

    return true;
}


void df_set_all_tiles(shared_ptr<dungeon_floor_t> const df, tiletype_t type) {
    minfo("df_set_all_tiles: Setting all tiles to type %d", type);
    massert(df, "dungeon floor is NULL");

    df_set_tile_area(df, type, 0, 0, df->width, df->height);
}


void df_set_tile_area_range2(shared_ptr<dungeon_floor_t> const df, int x, int y, int w, int h, tiletype_t begin, tiletype_t end) {
    massert(df, "dungeon floor is NULL");
    tiletype_t begin_type = begin;
    tiletype_t end_type = end;
    if (end_type < begin_type) {
        tiletype_t temp = begin_type;
        begin_type = end_type;
        end_type = temp;
    }
    const int num_types = end_type - begin_type + 1;
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            shared_ptr<tile_t> current = df_tile_at(df, (vec3){j, i, -1});
            tiletype_t type = (tiletype_t)(begin_type + (rand() % num_types));
            tile_init(current, type);
        }
    }
}


vec3 df_get_upstairs(shared_ptr<dungeon_floor_t> const df) {
    massert(df, "dungeon floor is NULL");
    if (df->upstairs_loc.x != -1 && df->upstairs_loc.y != -1) {
        return df->upstairs_loc;
    }
    vec3 loc = {-1, -1, -1};
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
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
    if (df->downstairs_loc.x != -1 && df->downstairs_loc.y != -1) {
        return df->downstairs_loc;
    }
    vec3 loc = {-1, -1, -1};
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
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
            if (tile_is_walkable(type)) {
                count++;
            }
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
            if (!t) {
                continue;
            }
            if (t->entities->size() == 0) {
                count++;
            }
        }
    }
    return count;
}


int df_count_non_walls_in_area(const shared_ptr<dungeon_floor_t> df, int x0, int y0, int w, int h) {
    massert(df, "df is NULL");

    int count = 0;

    for (int y = 0; y < h && y + y0 < df->height; y++) {
        for (int x = 0; x < w && x + x0 < df->width; x++) {
            int newx = x + x0;
            int newy = y + y0;
            shared_ptr<tile_t> t = df_tile_at(df, (vec3){newx, newy, -1});
            massert(t, "tile is NULL at (%d, %d)", newx, newy);
            if (tile_is_walkable(t->type)) {
                count++;
            }
        }
    }
    return count;
}


int df_count_non_walls(const shared_ptr<dungeon_floor_t> df) {
    return df_count_non_walls_in_area(df, 0, 0, df->width, df->height);
}


int df_count_empty_non_walls_in_area(const shared_ptr<dungeon_floor_t> df, int x0, int y0, int w, int h) {
    if (!df) {
        return 0;
    }

    int count = 0;

    for (int y = 0; y < h && y + y0 < df->height; y++) {
        for (int x = 0; x < w && x + x0 < df->width; x++) {
            shared_ptr<tile_t> t = df_tile_at(df, (vec3){x + x0, y + y0, -1});
            if (!t) {
                continue;
            }
            if (t->entities->size() == 0 && tile_is_walkable(t->type)) {
                count++;
            }
        }
    }
    return count;
}


int df_count_empty_non_walls(const shared_ptr<dungeon_floor_t> df) {
    return df_count_empty_non_walls_in_area(df, 0, 0, df->width, df->height);
}
