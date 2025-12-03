#include "ComponentTraits.h"
#include "dungeon_tile_type.h"
#include "entitytype.h"
#include "get_txkey_for_tiletype.h"
#include "libdraw_dungeon_floor.h"
#include "libdraw_player_target_box.h"
#include "libdraw_sprite.h"
#include "textureinfo.h"
#include <algorithm>


using std::for_each;
using std::sort;


extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];


static vector<vec3> calculate_path_with_thickness(vec3 start, vec3 end) {
    vector<vec3> path;
    int x1 = start.x, y1 = start.y;
    int x2 = end.x, y2 = end.y;
    int z = start.z;
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    while (true) {
        // Skip the start point (we don't need to check visibility with self)
        if (x1 != start.x || y1 != start.y) {
            // Add primary point
            path.push_back({x1, y1, z});
            // Add adjacent points for thickness only when we're not at start/end
            // and only when we're moving diagonally
            if (x1 != x2 && y1 != y2 && x1 != start.x && y1 != start.y) {
                // Add perpendicular points for diagonal movement
                if (dx > dy) {
                    path.push_back({x1, y1 + sy, z});
                    path.push_back({x1, y1 - sy, z});
                } else {
                    path.push_back({x1 + sx, y1, z});
                    path.push_back({x1 - sx, y1, z});
                }
            }
        }
        // Break if we've reached the end location
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
    // Remove duplicate points
    sort(path.begin(), path.end(), [](const vec3& a, const vec3& b) { return a.x < b.x || (a.x == b.x && a.y < b.y); });
    path.erase(unique(path.begin(), path.end(), [](const vec3& a, const vec3& b) { return a.x == b.x && a.y == b.y; }), path.end());
    return path;
}


bool draw_dungeon_floor_tile(shared_ptr<gamestate> g, textureinfo* txinfo, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    massert(txinfo, "txinfo is null");
    massert(x >= 0, "x is less than 0");
    massert(y >= 0, "y is less than 0");
    auto df = d_get_floor(g->dungeon, z);
    massert(df, "dungeon_floor is NULL");
    massert(x < df->width, "x is out of bounds");
    massert(y < df->height, "y is out of bounds");
    massert(df, "dungeon_floor is NULL");
    auto tile = df_tile_at(df, (vec3){x, y, z});
    massert(tile, "tile is NULL");
    if (tile->type == TILE_NONE || !tile->visible || !tile->explored)
        return true;
    // Get hero's total light radius
    const int light_dist = g->ct.get<light_radius>(g->hero_id).value_or(1);
    auto maybe_loc = g->ct.get<location>(g->hero_id);
    if (!maybe_loc.has_value())
        return false;
    const vec3 hero_loc = maybe_loc.value();
    // Calculate Manhattan distance from hero to this tile (diamond pattern)
    const int distance = abs(x - hero_loc.x) + abs(y - hero_loc.y);
    // Get tile texture
    const int txkey = get_txkey_for_tiletype(tile->type);
    if (txkey < 0)
        return false;
    Texture2D* texture = &txinfo[txkey].texture;
    if (texture->id <= 0)
        return false;
    // Calculate drawing position
    const int offset_x = -12;
    const int offset_y = -12;
    const int px = x * DEFAULT_TILE_SIZE + offset_x;
    const int py = y * DEFAULT_TILE_SIZE + offset_y;
    const Rectangle src = {0, 0, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};
    const Rectangle dest = {(float)px, (float)py, (float)DEFAULT_TILE_SIZE_SCALED, (float)DEFAULT_TILE_SIZE_SCALED};
    // Draw tile with fade if beyond light dist
    Color draw_color = distance > light_dist ? Fade(WHITE, 0.4f) : WHITE; // Faded for out-of-range tiles
    if (distance > light_dist) {
        DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, draw_color);
        return true;
    }
    // Draw tile with fade ALSO if path between tile and hero is blocked
    auto path = calculate_path_with_thickness({x, y, z}, hero_loc);
    // Check for blocking walls/doors in path
    bool blocking = false;
    for (const auto& v : path) {
        auto tile = df_tile_at(df, v);
        if (tile && (tile_is_wall(tile->type))) {
            blocking = true;
            break;
        }
        // Check for closed doors
        for_each(tile->entities->cbegin(), tile->entities->cend(), [g, &blocking](const entityid& id) {
            if (g->ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_DOOR && !g->ct.get<door_open>(id).value_or(false))
                blocking = true;
        });
        if (blocking)
            break;
    }
    // Apply fade if blocked
    draw_color = blocking ? Fade(draw_color, 0.3f) : draw_color;
    DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, draw_color);
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
            if (!tile || !tile->visible || tile_is_wall(tile->type) || tile->is_empty)
                continue;

            // bugfix for tall walls so entities do not draw on top:
            // check to see if the tile directly beneath this tile is a wall
            const vec3 loc2 = {x, y + 1, z};
            const auto tile2 = df_tile_at(df, loc2);
            if (tile2 && tile2->type == TILE_STONE_WALL_00)
                continue;

            // Get hero's vision distance and location
            const int vision_dist = g->ct.get<vision_distance>(g->hero_id).value_or(0);
            const int light_rad = g->ct.get<light_radius>(g->hero_id).value_or(0);
            auto maybe_hero_loc = g->ct.get<location>(g->hero_id);
            if (!maybe_hero_loc.has_value())
                continue;

            const vec3 hero_loc = maybe_hero_loc.value();
            const int dist_to_check = std::max(vision_dist, light_rad);
            // Calculate Manhattan distance from hero to this tile (diamond pattern)
            const int dist = abs(loc.x - hero_loc.x) + abs(loc.y - hero_loc.y);
            // Only draw entities within vision distance
            // we might want to enforce a drawing order with the introduction of doors...
            if (dist > dist_to_check)
                continue;

            // further, we need to step from the hero's location+1 to the tile location-1
            // for each tile in this path, we need to check to see if
            // 1. the tiletype is a WALL
            // 2. the tile contains a DOOR entity
            // if either is true, then there is an object blocking our visibility of any entities
            // it will be eventually possible to "see-thru-walls" in the future to overcome this...

            // Calculate path from entity to hero
            auto path = calculate_path_with_thickness(loc, hero_loc);

            // 2. for each item in path
            bool object_blocking = false;

            for (auto v0 : path) {
                auto v0_tile = df_tile_at(df, v0);
                if (tile_is_wall(v0_tile->type)) {
                    object_blocking = true;
                    break;
                }

                // check if tile has a DOOR
                for_each(v0_tile->entities->cbegin(), v0_tile->entities->cend(), [g, &object_blocking](const entityid& id) {
                    if (g->ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_DOOR && !g->ct.get<door_open>(id).value_or(false))
                        object_blocking = true;
                });

                if (object_blocking)
                    break;
            }

            if (object_blocking)
                continue;

            for_each(tile->entities->cbegin(), tile->entities->cend(), [&g, entitytype_0](const entityid& id) {
                if (entitytype_0 == g->ct.get<entitytype>(id).value_or(ENTITY_NONE))
                    draw_sprite_and_shadow(g, id);
            });
        }
    }
}


bool libdraw_draw_dungeon_floor(const shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    shared_ptr<dungeon_floor_t> df = d_get_current_floor(g->dungeon);
    massert(df, "dungeon_floor is NULL");
    const int z = g->dungeon->current_floor;

    // render tiles
    for (int y = 0; y < df->height; y++)
        for (int x = 0; x < df->width; x++)
            draw_dungeon_floor_tile(g, txinfo, x, y, z);

    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_DOOR);
    libdraw_draw_player_target_box(g);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_ITEM);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_NPC);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_PLAYER);

    return true;
}
