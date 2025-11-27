#include "ComponentTraits.h"
#include "libdraw_dungeon_floor.h"
//#include "libdraw_dungeon_floor_tile.h"
#include "get_txkey_for_tiletype.h"
#include "libdraw_sprite.h"
#include "textureinfo.h"


extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];


bool draw_dungeon_floor_tile(shared_ptr<gamestate> g, textureinfo* txinfo, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    massert(txinfo, "txinfo is null");
    massert(x >= 0, "x is less than 0");
    massert(y >= 0, "y is less than 0");

    shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, z);
    massert(df, "dungeon_floor is NULL");
    massert(x < df->width, "x is out of bounds");
    massert(y < df->height, "y is out of bounds");
    massert(df, "dungeon_floor is NULL");

    shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x, y, z});
    massert(tile, "tile is NULL");

    if (tile->type == TILE_NONE) {
        //merror("Tile at (%d, %d) is TILE_NONE", x, y);
        return true;
    }

    if (!tile->visible) {
        //merror("Tile at (%d, %d) is not visible", x, y);
        return true;
    }

    if (!tile->explored) {
        return true;
    }

    //if (tile->type == TILE_STONE_WALL_00) {
    //    return true;
    //}

    // Get hero's total light radius
    int light_dist = g->ct.get<light_radius>(g->hero_id).value_or(1);

    optional<vec3> maybe_loc = g->ct.get<location>(g->hero_id);
    if (maybe_loc.has_value()) {
        vec3 hero_loc = maybe_loc.value();
        // Calculate Manhattan distance from hero to this tile (diamond pattern)
        int distance = abs(x - hero_loc.x) + abs(y - hero_loc.y);
        // Get tile texture
        int txkey = get_txkey_for_tiletype(tile->type);
        if (txkey < 0) {
            return false;
        }
        Texture2D* texture = &txinfo[txkey].texture;
        if (texture->id <= 0) {
            return false;
        }
        // Calculate drawing position
        int offset_x = -12;
        int offset_y = -12;
        int px = x * DEFAULT_TILE_SIZE + offset_x;
        int py = y * DEFAULT_TILE_SIZE + offset_y;
        Rectangle src = {0, 0, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};
        Rectangle dest = {(float)px, (float)py, (float)DEFAULT_TILE_SIZE_SCALED, (float)DEFAULT_TILE_SIZE_SCALED};
        // Draw tile with fade if beyond light dist
        Color draw_color = distance > light_dist ? Fade(WHITE, 0.4f) : WHITE; // Faded for out-of-range tiles
        DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, draw_color);
    }
    return true;
}


bool draw_dungeon_floor_wall(shared_ptr<gamestate> g, textureinfo* txinfo, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    massert(txinfo, "txinfo is null");
    massert(x >= 0, "x is less than 0");
    massert(y >= 0, "y is less than 0");
    shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, z);
    massert(df, "dungeon_floor is NULL");
    massert(x < df->width, "x is out of bounds");
    massert(y < df->height, "y is out of bounds");
    massert(df, "dungeon_floor is NULL");
    shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x, y, z});
    massert(tile, "tile is NULL");

    if (tile->type == TILE_NONE) {
        //merror("Tile at (%d, %d) is TILE_NONE", x, y);
        return true;
    }

    if (!tile->visible) {
        //merror("Tile at (%d, %d) is not visible", x, y);
        return true;
    }
    if (!tile->explored) {
        return true;
    }

    if (tile->type != TILE_STONE_WALL_00) {
        return true;
    }

    // Get hero's total light radius
    int light_dist = g->ct.get<light_radius>(g->hero_id).value_or(1);

    optional<vec3> maybe_loc = g->ct.get<location>(g->hero_id);
    if (maybe_loc.has_value()) {
        vec3 hero_loc = maybe_loc.value();
        // Calculate Manhattan distance from hero to this tile (diamond pattern)
        int distance = abs(x - hero_loc.x) + abs(y - hero_loc.y);
        // Get tile texture
        int txkey = get_txkey_for_tiletype(tile->type);
        if (txkey < 0) {
            return false;
        }
        Texture2D* texture = &txinfo[txkey].texture;
        if (texture->id <= 0) {
            return false;
        }
        // Calculate drawing position
        int offset_x = -12;
        int offset_y = -12;
        int px = x * DEFAULT_TILE_SIZE + offset_x;
        int py = y * DEFAULT_TILE_SIZE + offset_y;
        Rectangle src = {0, 0, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};
        Rectangle dest = {(float)px, (float)py, (float)DEFAULT_TILE_SIZE_SCALED, (float)DEFAULT_TILE_SIZE_SCALED};
        // Draw tile with fade if beyond light dist
        Color draw_color = distance > light_dist ? Fade(WHITE, 0.4f) : WHITE; // Faded for out-of-range tiles
        DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, draw_color);
    }
    return true;
}


bool libdraw_draw_dungeon_floor(const shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    //dungeon_floor_t* const df = d_get_current_floor(g->d);
    shared_ptr<dungeon_floor_t> df = d_get_current_floor(g->dungeon);
    massert(df, "dungeon_floor is NULL");
    const int z = g->dungeon->current_floor;


    /*
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            draw_dungeon_floor_tile(g, txinfo, x, y, z);
            const vec3 loc = {x, y, z};
            shared_ptr<tile_t> tile = df_tile_at(df, loc);
            if (!tile) continue;
            if (tile_is_wall(tile->type)) continue;
            // Do not draw entities on invisible tiles
            if (!tile->visible) continue;
            if (tile->is_empty) continue;
            // Get hero's vision distance and location
            const int vision_dist = g->ct.get<vision_distance>(g->hero_id).value_or(0);
            const int light_rad = g->ct.get<light_radius>(g->hero_id).value_or(0);
            const int light_dist = light_rad + 1;
            optional<vec3> maybe_hero_loc = g->ct.get<location>(g->hero_id);
            if (maybe_hero_loc.has_value()) {
                vec3 hero_loc = maybe_hero_loc.value();
                const int dist_to_check = std::max(vision_dist, light_dist);
                // Calculate Manhattan distance from hero to this tile (diamond pattern)
                const int distance = abs(loc.x - hero_loc.x) + abs(loc.y - hero_loc.y);
                // Only draw entities within vision distance
                if (distance <= vision_dist && distance <= dist_to_check) {
                    for (size_t i = 0; i < tile->entities->size(); i++) {
                        const entityid id = tile_get_entity(tile, i);
                        draw_sprite_and_shadow(g, id);
                    }
                }
            }
            draw_dungeon_floor_wall(g, txinfo, x, y, z);
        }
    }
    */


    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            draw_dungeon_floor_tile(g, txinfo, x, y, z);
        }
    }

    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            //draw_entities_2d_at(g, df, false, (vec3){x, y, z});
            const vec3 loc = {x, y, z};
            shared_ptr<tile_t> tile = df_tile_at(df, loc);
            if (!tile) continue;
            // Do not draw entities on invisible tiles
            if (!tile->visible) continue;
            if (tile_is_wall(tile->type)) continue;
            if (tile->is_empty) continue;
            // Get hero's vision distance and location
            const int vision_dist = g->ct.get<vision_distance>(g->hero_id).value_or(0);
            const int light_rad = g->ct.get<light_radius>(g->hero_id).value_or(0);
            const int light_dist = light_rad + 1;
            optional<vec3> maybe_hero_loc = g->ct.get<location>(g->hero_id);
            if (maybe_hero_loc.has_value()) {
                vec3 hero_loc = maybe_hero_loc.value();
                const int dist_to_check = std::max(vision_dist, light_dist);
                // Calculate Manhattan distance from hero to this tile (diamond pattern)
                const int distance = abs(loc.x - hero_loc.x) + abs(loc.y - hero_loc.y);
                // Only draw entities within vision distance
                if (distance <= vision_dist && distance <= dist_to_check) {
                    for (size_t i = 0; i < tile->entities->size(); i++) {
                        const entityid id = tile_get_entity(tile, i);
                        draw_sprite_and_shadow(g, id);
                    }
                }
            }
        }
    }

    //for (int y = 0; y < df->height; y++) {
    //    for (int x = 0; x < df->width; x++) {
    //        draw_dungeon_floor_wall(g, txinfo, x, y, z);
    //    }
    //}

    return true;
}
