#include "ComponentTraits.h"
#include "dungeon_tile_type.h"
#include "entitytype.h"
#include "get_txkey_for_tiletype.h"
#include "libdraw_dungeon_floor.h"
#include "libdraw_player_target_box.h"
#include "libdraw_sprite.h"
#include "textureinfo.h"

#include <algorithm>


using std::sort;


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
        return true;
    }

    if (!tile->visible) {
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
        const vec3 hero_loc = maybe_loc.value();
        // Calculate Manhattan distance from hero to this tile (diamond pattern)
        const int distance = abs(x - hero_loc.x) + abs(y - hero_loc.y);
        // Get tile texture
        const int txkey = get_txkey_for_tiletype(tile->type);
        if (txkey < 0) {
            return false;
        }
        Texture2D* texture = &txinfo[txkey].texture;
        if (texture->id <= 0) {
            return false;
        }
        // Calculate drawing position
        const int offset_x = -12;
        const int offset_y = -12;
        const int px = x * DEFAULT_TILE_SIZE + offset_x;
        const int py = y * DEFAULT_TILE_SIZE + offset_y;
        const Rectangle src = {0, 0, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};
        const Rectangle dest = {(float)px, (float)py, (float)DEFAULT_TILE_SIZE_SCALED, (float)DEFAULT_TILE_SIZE_SCALED};
        // Draw tile with fade if beyond light dist
        const Color draw_color = distance > light_dist ? Fade(WHITE, 0.4f) : WHITE; // Faded for out-of-range tiles
        DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, draw_color);
    }
    return true;
}


void libdraw_draw_dungeon_floor_entitytype(const shared_ptr<gamestate> g, entitytype_t entitytype_0) {
    shared_ptr<dungeon_floor_t> df = d_get_current_floor(g->dungeon);
    massert(df, "dungeon_floor is NULL");
    const int z = g->dungeon->current_floor;

    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            const vec3 loc = {x, y, z};
            const auto tile = df_tile_at(df, loc);
            if (!tile || !tile->visible || tile_is_wall(tile->type) || tile->is_empty) {
                continue;
            }

            // bugfix for tall walls so entities do not draw on top:
            // check to see if the tile directly beneath this tile is a wall
            const vec3 loc2 = {x, y + 1, z};
            const auto tile2 = df_tile_at(df, loc2);
            if (tile2 && tile2->type == TILE_STONE_WALL_00) {
                continue;
            }

            // Get hero's vision distance and location
            const int vision_dist = g->ct.get<vision_distance>(g->hero_id).value_or(0);
            const int light_rad = g->ct.get<light_radius>(g->hero_id).value_or(0);
            //const int light_dist = light_rad;
            auto maybe_hero_loc = g->ct.get<location>(g->hero_id);
            if (maybe_hero_loc.has_value()) {
                vec3 hero_loc = maybe_hero_loc.value();
                const int dist_to_check = std::max(vision_dist, light_rad);
                // Calculate Manhattan distance from hero to this tile (diamond pattern)
                const int dist = abs(loc.x - hero_loc.x) + abs(loc.y - hero_loc.y);
                // Only draw entities within vision distance
                // we might want to enforce a drawing order with the introduction of doors...
                if (dist > dist_to_check) {
                    continue;
                }

                // further, we need to step from the hero's location+1 to the tile location-1
                // for each tile in this path, we need to check to see if
                // 1. the tiletype is a WALL
                // 2. the tile contains a DOOR entity
                // if either is true, then there is an object blocking our visibility of any entities
                // it will be eventually possible to "see-thru-walls" in the future to overcome this...

                // 1. build a thick path from loc to hero_loc using modified Bresenham's algorithm
                vector<vec3> path;
                int x1 = loc.x, y1 = loc.y;
                int x2 = hero_loc.x, y2 = hero_loc.y;

                int dx = abs(x2 - x1);
                int dy = abs(y2 - y1);
                int sx = (x1 < x2) ? 1 : -1;
                int sy = (y1 < y2) ? 1 : -1;
                int err = dx - dy;

                while (true) {
                    // Skip the start point (we don't need to check visibility with self)
                    if (x1 != loc.x || y1 != loc.y) {
                        // Add primary point
                        path.push_back({x1, y1, z});

                        // Add adjacent points for thickness
                        // Horizontal/vertical lines get perpendicular adjacents
                        if (dx > dy) {
                            path.push_back({x1, y1 + 1, z});
                            path.push_back({x1, y1 - 1, z});
                        } else {
                            path.push_back({x1 + 1, y1, z});
                            path.push_back({x1 - 1, y1, z});
                        }
                    }

                    // Break if we've reached the hero's location
                    if (x1 == x2 && y1 == y2)
                        break;

                    int e2 = 2 * err;
                    if (e2 > -dy) {
                        err -= dy;
                        x1 += sx;
                    }
                    if (e2 < dx) {
                        err += dx;
                        y1 += sy;
                    }
                }

                // Remove duplicate points that might have been added
                sort(path.begin(), path.end(), [](const vec3& a, const vec3& b) { return a.x < b.x || (a.x == b.x && a.y < b.y); });
                path.erase(unique(path.begin(), path.end(), [](const vec3& a, const vec3& b) { return a.x == b.x && a.y == b.y; }), path.end());

                // 2. for each item in path
                bool object_blocking = false;

                for (auto v0 : path) {
                    // get tile
                    auto v0_tile = df_tile_at(df, v0);
                    // check type
                    if (v0_tile->type == TILE_STONE_WALL_00 || v0_tile->type == TILE_STONE_WALL_01) {
                        // reject
                        object_blocking = true;
                        break;
                    }

                    // check if tile has a DOOR
                    for (auto eid : *(v0_tile->entities)) {
                        auto eid_type = g->ct.get<entitytype>(eid);
                        if (eid_type == ENTITY_DOOR) {
                            // reject
                            object_blocking = true;
                            break;
                        }
                    }

                    if (object_blocking) {
                        break;
                    }
                }

                if (object_blocking) {
                    continue;
                }


                for (size_t i = 0; i < tile->entities->size(); i++) {
                    const entityid id = tile_get_entity(tile, i);

                    auto maybe_type = g->ct.get<entitytype>(id);
                    if (!maybe_type.has_value()) {
                        continue;
                    }

                    const auto type = maybe_type.value();

                    if (type == entitytype_0) {
                        draw_sprite_and_shadow(g, id);
                    }
                }
            }
        }
    }
}


bool libdraw_draw_dungeon_floor(const shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    shared_ptr<dungeon_floor_t> df = d_get_current_floor(g->dungeon);
    massert(df, "dungeon_floor is NULL");
    const int z = g->dungeon->current_floor;

    // render tiles
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            draw_dungeon_floor_tile(g, txinfo, x, y, z);
        }
    }

    // render doors
    //libdraw_draw_dungeon_floor_doors(g);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_DOOR);

    libdraw_draw_player_target_box(g);

    // render items
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_ITEM);

    // render NPCs
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_NPC);

    // render Player
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_PLAYER);

    return true;
}
