#include "biome.h"
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


void df_set_tile(shared_ptr<dungeon_floor_t> df, tiletype_t type, int x, int y);
void df_set_all_tiles(shared_ptr<dungeon_floor_t> df, tiletype_t type);
void df_assign_stairs(shared_ptr<dungeon_floor_t> df);
void df_assign_downstairs(shared_ptr<dungeon_floor_t> df);
void df_assign_upstairs(shared_ptr<dungeon_floor_t> df);
bool df_assign_upstairs_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r);
bool df_assign_downstairs_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r);
int df_get_possible_upstairs_count(shared_ptr<dungeon_floor_t> df);
int df_get_possible_downstairs_count(shared_ptr<dungeon_floor_t> df);
shared_ptr<vector<vec3>> df_get_possible_downstairs_locs(shared_ptr<dungeon_floor_t> df);
shared_ptr<vector<vec3>> df_get_possible_downstairs_locs_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r);
shared_ptr<vector<vec3>> df_get_possible_upstairs_locs(shared_ptr<dungeon_floor_t> df);
shared_ptr<vector<vec3>> df_get_possible_upstairs_locs_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r);
int df_get_possible_upstairs_count_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r);
int df_get_possible_downstairs_count_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r);
void df_set_area(shared_ptr<dungeon_floor_t> df, tiletype_t a, tiletype_t b, Rectangle r);
void df_set_perimeter(shared_ptr<dungeon_floor_t> df, tiletype_t a, tiletype_t b, Rectangle r);
tiletype_t random_tiletype(tiletype_t a, tiletype_t b);


tiletype_t random_tiletype(tiletype_t a, tiletype_t b) {
    return (tiletype_t)GetRandomValue(a, b);
}


shared_ptr<dungeon_floor_t> df_init(int floor, biome_t t, int width, int height) {
    massert(width > 0, "width must be greater than zero");
    massert(height > 0, "height must be greater than zero");
    massert(floor >= 0, "floor must be greater than or equal to zero");

    // creating a new dungeon floor
    // this was likely called by d_add_floor
    shared_ptr<dungeon_floor_t> df = make_shared<dungeon_floor_t>();
    massert(df, "failed to malloc dungeon floor");

    // init floor vars
    df->floor = floor;
    df->width = width;
    df->height = height;
    df->biome = t;

    // alloc the tile vector
    df->tiles = make_shared<vector<tile_id>>();
    massert(df->tiles, "failed to create tiles vector");

    // alloc the tile map
    df->tile_map = make_shared<unordered_map<tile_id, shared_ptr<tile_t>>>();
    massert(df->tile_map, "failed to create tile map");

    // create all the tiles and add to the tile vector and tile map
    for (tile_id i = 0; i < width * height; i++) {
        df->tiles->push_back(i);
        auto tile = make_shared<tile_t>();
        massert(tile, "failed to create tile");
        tile->id = i;
        tile_init(tile, TILE_NONE);
        df->tile_map->insert({i, tile});
    }

    // locate "center"
    // in the 24x24 case,
    // this is hard-coded atm but we will move it into a function
    const float x = width / 3.0;
    const float y = height / 3.0;
    const int w = 8;
    const int h = 8;
    // init the inner w x h area
    // create a new room with walls
    df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_09, (Rectangle){x, y, w, h});
    df_set_perimeter(df, TILE_STONE_WALL_01, TILE_STONE_WALL_01, (Rectangle){x - 1, y - 1, w + 2, h + 2});
    df_set_area(df, TILE_STONE_WALL_01, TILE_STONE_WALL_01, (Rectangle){x + w / 2.0f, y, 1, h});
    df_set_area(df, TILE_STONE_WALL_01, TILE_STONE_WALL_01, (Rectangle){x, y + h / 2.0f, w, 1});
    df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w / 2.0f, y + 1, 1, 1});
    df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w / 2.0f + 2, y + 4, 1, 2});
    df_set_area(df, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_00, (Rectangle){x + w / 2.0f, y + 6, 1, 2});

    const vec3 loc_u = {9, 9, floor};
    df_set_tile(df, TILE_UPSTAIRS, loc_u.x, loc_u.y);
    df->upstairs_loc = loc_u;

    const vec3 loc_d = {9, 14, floor};
    df_set_tile(df, TILE_DOWNSTAIRS, loc_d.x, loc_d.y);
    df->downstairs_loc = loc_d;


    df_set_can_have_door(df, (vec3){12, 9, 0});
    if (df_tile_at(df, (vec3){12, 9, 0})->can_have_door) {
        msuccess("door has can have door set");
        //DEBUG_BREAK();
    } else {
        merror("door does not have can have door set");
        DEBUG_BREAK();
    }

    df_set_can_have_door(df, (vec3){12, 9, 1});
    df_set_can_have_door(df, (vec3){12, 9, 2});
    df_set_can_have_door(df, (vec3){12, 9, 3});

    df_set_can_have_door(df, (vec3){14, 12, 0});
    df_set_can_have_door(df, (vec3){14, 12, 1});
    df_set_can_have_door(df, (vec3){14, 12, 2});
    df_set_can_have_door(df, (vec3){14, 12, 3});


    msuccess("Created dungeon floor %d with dimensions %dx%d", floor, width, height);
    return df;
}


void df_xform(function<void()> mLambda) {
    //massert(width > 0, "width must be greater than zero");
    //massert(height > 0, "height must be greater than zero");
    //massert(floor >= 0, "floor must be greater than or equal to zero");

    // creating a new dungeon floor
    // this was likely called by d_add_floor
    //auto df = df_init(floor, t, width, height);
    //massert(df, "failed to malloc dungeon floor");

    // init floor vars
    //df->floor = floor;
    //df->width = width;
    //df->height = height;
    //df->type = t;

    // alloc the tile vector
    //df->tiles = make_shared<vector<tile_id>>();
    //massert(df->tiles, "failed to create tiles vector");

    // alloc the tile map
    //df->tile_map = make_shared<unordered_map<tile_id, shared_ptr<tile_t>>>();
    //massert(df->tile_map, "failed to create tile map");

    // create all the tiles and add to the tile vector and tile map
    //for (tile_id i = 0; i < width * height; i++) {
    //    df->tiles->push_back(i);
    //    auto tile = make_shared<tile_t>();
    //    massert(tile, "failed to create tile");
    //    tile->id = i;
    //    tile_init(tile, TILE_NONE);
    //    df->tile_map->insert({i, tile});
    //}

    mLambda();

    //return df;
}


void df_set_area(shared_ptr<dungeon_floor_t> df, tiletype_t a, tiletype_t b, Rectangle r) {
    for (int x = r.x; x < r.x + r.width; x++) {
        for (int y = r.y; y < r.y + r.height; y++) {
            const tiletype_t t = random_tiletype(a, b);
            df_set_tile(df, t, x, y);
        }
    }
}


void df_set_perimeter(shared_ptr<dungeon_floor_t> df, tiletype_t a, tiletype_t b, Rectangle r) {
    for (int x = r.x; x < r.x + r.width; x++) {
        const tiletype_t t = random_tiletype(a, b);

        df_set_tile(df, t, x, r.y);
        df_set_tile(df, t, x, r.y + r.height - 1);
    }

    for (int y = r.y; y < r.y + r.height; y++) {
        const tiletype_t t = random_tiletype(a, b);
        df_set_tile(df, t, r.x, y);
        df_set_tile(df, t, r.x + r.width - 1, y);
    }
}


//void df_set_perimeter(shared_ptr<dungeon_floor_t> df, int x, int y, int w, int h) {
//    for (int x0 = x; x0 < x + w; x0++) {
//        df_set_tile(df, TILE_STONE_WALL_00, x0, y);
//        df_set_tile(df, TILE_STONE_WALL_00, x0, y + h);
//    }
//
//    for (int y0 = y; y0 < y + h; y0++) {
//        df_set_tile(df, TILE_STONE_WALL_00, x, y0);
//        df_set_tile(df, TILE_STONE_WALL_00, x + w, y0);
//    }
//}


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
    df_assign_upstairs_in_area(df, (Rectangle){0, 0, (float)df->width, (float)df->height});
    df_assign_downstairs_in_area(df, (Rectangle){0, 0, (float)df->width, (float)df->height});
}


int df_get_possible_upstairs_count(shared_ptr<dungeon_floor_t> df) {
    return df_get_possible_upstairs_count_in_area(df, (Rectangle){0, 0, (float)df->width, (float)df->height});
}


int df_get_possible_downstairs_count(shared_ptr<dungeon_floor_t> df) {
    return df_get_possible_downstairs_count_in_area(df, (Rectangle){0, 0, (float)df->width, (float)df->height});
}


shared_ptr<vector<vec3>> df_get_possible_upstairs_locs(shared_ptr<dungeon_floor_t> df) {
    massert(df, "dungeon floor is NULL");

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


bool df_assign_upstairs_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r) {
    massert(df, "dungeon floor is NULL");
    massert(r.x >= 0, "x is less than zero");
    massert(r.x < df->width, "x is out of bounds");
    massert(r.y >= 0, "y is less than zero");
    massert(r.y < df->height, "y is out of bounds");
    massert(r.width > 0, "w is less than zero");
    massert(r.height > 0, "h is less than zero");
    massert(r.x + r.width <= df->width, "x + w is out of bounds");
    massert(r.y + r.height <= df->height, "y + h is out of bounds");
    //int count = -1;

    auto locations = df_get_possible_upstairs_locs_in_area(df, (Rectangle){r.x, r.y, r.width, r.height});
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
    df_assign_upstairs_in_area(df, (Rectangle){0, 0, (float)df->width, (float)df->height});
}


void df_assign_downstairs(shared_ptr<dungeon_floor_t> df) {
    df_assign_downstairs_in_area(df, (Rectangle){0, 0, (float)df->width, (float)df->height});
}


bool df_assign_downstairs_in_area(shared_ptr<dungeon_floor_t> df, Rectangle r) {
    massert(df, "dungeon floor is NULL");
    massert(r.x >= 0, "x is less than zero");
    massert(r.x < df->width, "x is out of bounds");
    massert(r.y >= 0, "y is less than zero");
    massert(r.y < df->height, "y is out of bounds");
    massert(r.width > 0, "w is less than zero");
    massert(r.height > 0, "h is less than zero");
    massert(r.x + r.width <= df->width, "x + w is out of bounds");
    massert(r.y + r.height <= df->height, "y + h is out of bounds");
    int count = -1;
    auto locations = df_get_possible_downstairs_locs_in_area(df, (Rectangle){(float)r.x, (float)r.y, (float)r.width, (float)r.height});
    // now that we have a list of possible locations for the upstairs to appear
    // we can randomly select one of them
    int downstairs_index = rand() % count;
    vec3 down_loc = locations->at(downstairs_index);
    // now we can set the upstairs tile
    shared_ptr<tile_t> tile = df_tile_at(df, down_loc);
    massert(tile, "failed to get tile");
    tile_init(tile, TILE_DOWNSTAIRS);
    tile->visible = true; // make sure the upstairs tile is visible
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


//void df_set_tile_area(shared_ptr<dungeon_floor_t> const df, tiletype_t type, Rectangle r) {
//    massert(df, "dungeon floor is NULL");
//    massert(r.x >= 0, "x is less than zero");
//    massert(r.x < df->width, "x is out of bounds");
//    massert(r.y >= 0, "y is less than zero");
//    massert(r.y < df->height, "y is out of bounds");
//    massert(r.width > 0, "w is less than zero");
//    massert(r.height > 0, "h is less than zero");
//    massert(r.x + r.width <= df->width, "x + w is out of bounds");
//    massert(r.y + r.height <= df->height, "y + h is out of bounds");
//
//    minfo("entering loop...");
//
//    for (int i = r.y; i < r.y + r.height; i++) {
//        for (int j = r.x; j < r.x + r.width; j++) {
//            tile_init(df_tile_at(df, (vec3){j, i, -1}), type);
//        }
//    }
//}


void df_set_tile(shared_ptr<dungeon_floor_t> df, tiletype_t type, int x, int y) {
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


void df_set_all_tiles(shared_ptr<dungeon_floor_t> df, tiletype_t type) {
    minfo("df_set_all_tiles: Setting all tiles to type %d", type);
    massert(df, "dungeon floor is NULL");
    df_set_area(df, type, type, (Rectangle){0, 0, (float)df->width, (float)df->height});
}


//vec3 df_get_upstairs(shared_ptr<dungeon_floor_t> const df) {
//    massert(df, "dungeon floor is NULL");
//    if (df->upstairs_loc.x != -1 && df->upstairs_loc.y != -1) {
//        return df->upstairs_loc;
//    }
//    vec3 loc = {-1, -1, -1};
//    for (int y = 0; y < df->height; y++) {
//        for (int x = 0; x < df->width; x++) {
//            shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x, y, -1});
//            if (tile->type == TILE_UPSTAIRS) {
//                loc.x = x;
//                loc.y = y;
//                df->upstairs_loc = loc;
//                return loc;
//            }
//        }
//    }
//    return loc;
//}


//vec3 df_get_downstairs(shared_ptr<dungeon_floor_t> const df) {
//    massert(df, "dungeon floor is NULL");
//    if (df->downstairs_loc.x != -1 && df->downstairs_loc.y != -1) {
//        return df->downstairs_loc;
//    }
//    vec3 loc = {-1, -1, -1};
//    for (int y = 0; y < df->height; y++) {
//        for (int x = 0; x < df->width; x++) {
//            shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x, y, -1});
//            if (tile->type == TILE_DOWNSTAIRS) {
//                loc.x = x;
//                loc.y = y;
//                df->downstairs_loc = loc;
//                return loc;
//            }
//        }
//    }
//    return loc;
//}
