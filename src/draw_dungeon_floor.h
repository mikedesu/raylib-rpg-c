#pragma once

#include "calculate_linear_path.h"
#include "draw_sprite.h"
#include "dungeon_floor.h"
#include "entitytype.h"
#include "gamestate.h"
#include "get_txkey_for_tiletype.h"
#include "libdraw_player_target_box.h"
#include "libgame_defines.h"
#include "textureinfo.h"


extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

constexpr static inline int manhattan_distance(vec3 a, vec3 b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}




//static inline bool is_loc_too_far_to_draw(gamestate& g, vec3 loc, vec3 hero_loc) {
static inline bool is_loc_too_far_to_draw(gamestate& g, vec3 loc, vec3 hero_loc, int vision_dist, int light_rad) {
    // Get hero's vision distance and location

    //const int vision_dist = g.ct.get<vision_distance>(g.hero_id).value_or(0);
    //const int light_rad = g.ct.get<light_radius>(g.hero_id).value_or(0);

    massert(!vec3_invalid(hero_loc), "hero_loc invalid");
    massert(!vec3_invalid(loc), "loc invalid");
    const int dist_to_check = std::max(vision_dist, light_rad);
    // Calculate Manhattan distance from hero to this tile (diamond pattern)
    const int dist = manhattan_distance(loc, hero_loc);
    // Only draw entities within vision distance
    // we might want to enforce a drawing order with the introduction of doors...
    return dist > dist_to_check;
}




static inline bool is_loc_path_blocked(gamestate& g, shared_ptr<dungeon_floor> df, vec3 loc, vec3 hero_loc) {
    vector<vec3> path = calculate_path_with_thickness(loc, hero_loc);
    bool blocked = false;

    if (path.size() == 0) {
        return blocked;
    }

    for (auto v0 : path) {
        // skip v0 if it is equal to hero_loc
        if (vec3_equal(v0, hero_loc)) {
            continue;
        }

        auto v0_tile = df->tile_at(v0);
        auto v0_tiletype = v0_tile->get_type();

        if (tiletype_is_none(v0_tiletype) || tiletype_is_wall(v0_tiletype)) {
            blocked = true;
            break;
        }

        if (v0_tile->get_entities()->size() == 0) {
            continue;
        }

        for (entityid id : *v0_tile->get_entities()) {
            auto type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
            // check if tile has a DOOR
            if (type == ENTITY_DOOR) {
                auto is_open = g.ct.get<door_open>(id).value_or(false);
                if (!is_open) {
                    blocked = true;
                    break;
                }
            }
            // Check if tile has a box
            else if (type == ENTITY_BOX) {
                blocked = true;
                break;
            }
            // Check if tile has an NPC
            else if (type == ENTITY_NPC) {
                // Check if NPC is dead or alive
                massert(g.ct.has<dead>(id), "id %d doesn't have a dead component", id);
                const bool is_dead = g.ct.get<dead>(id).value();
                if (!is_dead) {
                    blocked = true;
                    break;
                }
            }
        }

        if (blocked) {
            break;
        }
    }



    return blocked;
}




static inline bool draw_dungeon_floor_tile(gamestate& g, int x, int y, int z) {
    //minfo3("BEGIN draw dungeon floor tile");
    //massert(txinfo, "txinfo is null");
    massert(z >= 0 && static_cast<size_t>(z) < g.d.get_floor_count(), "z is oob");

    // Calculate drawing position
    const float px = x * DEFAULT_TILE_SIZE + DEFAULT_OFFSET;
    const float py = y * DEFAULT_TILE_SIZE + DEFAULT_OFFSET;
    const Rectangle src = {0, 0, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};
    const Rectangle dest = {px, py, DEFAULT_TILE_SIZE_FLOAT, DEFAULT_TILE_SIZE_FLOAT};

    //const float cx = g.cam2d.target.x;
    //const float cy = g.cam2d.target.y;
    //const float cw = cx + DEFAULT_TARGET_WIDTH;
    //const float ch = cy + DEFAULT_TARGET_HEIGHT;

    //const float tile_right_x = px + DEFAULT_TILE_SIZE_FLOAT;
    //const float tile_left_x = px;

    //const float tile_top_y = py;
    //const float tile_bottom_y = py + DEFAULT_TILE_SIZE_FLOAT;

    //if (px + DEFAULT_TILE_SIZE_FLOAT < 0 || px > DEFAULT_TARGET_WIDTH) {
    //if (tile_right_x < cx || tile_left_x > cw || tile_top_y < cy || tile_bottom_y > ch) {
    // tile is outside the camera view
    //    return false;
    //}
    //}



    auto df = g.d.get_floor(z);

    massert(df, "dungeon_floor is NULL");
    massert(x >= 0 && x < df->get_width(), "x is oob");
    massert(y >= 0 && y < df->get_height(), "y is oob");

    const vec3 loc = {x, y, z};

    massert(!vec3_invalid(loc), "loc is invalid");

    auto tile = df->tile_at(vec3{x, y, z});

    massert(tile, "tile is NULL");

    if (tile->get_type() == TILE_NONE || !tile->get_visible() || !tile->get_explored()) {
        return true;
    }

    // Get tile texture
    const int txkey = get_txkey_for_tiletype(tile->get_type());
    massert(txkey >= 0, "txkey is invalid");

    Texture2D* texture = &txinfo[txkey].texture;
    massert(texture->id > 0, "texture->id is <= 0");

    // Get hero's total light radius
    const int light_dist = g.ct.get<light_radius>(g.hero_id).value_or(1);

    auto maybe_hero_loc = g.ct.get<location>(g.hero_id);

    if (!maybe_hero_loc.has_value()) {
        //minfo3("END draw dungeon floor tile 1");
        return false;
    }

    const vec3 hero_loc = maybe_hero_loc.value();



    // Calculate Manhattan distance from hero to this tile (diamond pattern)
    const int distance = manhattan_distance(loc, hero_loc);


    // Draw tile with fade ALSO if path between tile and hero is blocked
    // Check for blocking walls/doors in path
    const bool blocking = is_loc_path_blocked(g, df, loc, hero_loc);

    // Apply fade if blocked
    const unsigned char a = blocking ? 31 : distance > light_dist ? 102 : 255;
    const Color draw_color = Color{255, 255, 255, a};

    DrawTexturePro(*texture, src, dest, Vector2{0, 0}, 0, draw_color);

    //minfo3("END draw dungeon floor tile 5");
    return true;
}




static inline void
draw_dungeon_floor_entitytype(gamestate& g, entitytype_t type_0, int vision_dist, int light_rad, function<bool(gamestate&, entityid)> extra_check) {
    auto df = g.d.get_current_floor();
    const int df_w = df->get_width();
    const int df_h = df->get_height();
    //const int num_tiles = df_w * df_h;

    auto hero_loc = g.ct.get<location>(g.hero_id).value_or(vec3{-1, -1, -1});
    //massert(!vec3_invalid(hero_loc), "hero loc is invalid");
    //const int vision_dist = g.ct.get<vision_distance>(g.hero_id).value_or(0);
    //const int light_rad = g.ct.get<light_radius>(g.hero_id).value_or(0);

    //for (int i = 0; i < num_tiles; i++) {
    for (int y = 0; y < df_h; y++) {
        //const int y = i / df_w;
        for (int x = 0; x < df_w; x++) {
            //const int x = i - (y * df_w);
            const vec3 loc = {x, y, g.d.current_floor};
            auto tile = df->tile_at(loc);
            auto tiletype = tile->get_type();
            if (tiletype_is_none(tiletype) || tiletype_is_wall(tiletype)) {
                continue;
            }

            if (is_loc_too_far_to_draw(g, loc, hero_loc, vision_dist, light_rad)) {
                continue;
            }

            // bugfix for tall walls so entities do not draw on top: check to see if the tile directly beneath this tile is a wall
            //if (tile2.type == TILE_STONE_WALL_00) continue;
            // render all entities if not blocked
            if (is_loc_path_blocked(g, df, loc, hero_loc)) {
                continue;
            }
            if (tile->get_entities()->size() == 0) {
                continue;
                //break;
            }
            auto entities_begin = tile->get_entities()->cbegin();
            auto entities_end = tile->get_entities()->cend();
            //auto entities = tile->get_entities();
            entityid id = INVALID;
            entitytype_t type = ENTITY_NONE;
            for (auto it = entities_begin; it != entities_end; it++) {
                id = *it;
                //const entitytype_t type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
                type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
                if (type_0 == type && extra_check(g, id)) {
                    draw_sprite_and_shadow(g, id);
                }
            }
        }
    }
}




static inline bool draw_dungeon_floor(gamestate& g) {
    shared_ptr<dungeon_floor> df = g.d.get_current_floor();
    const int z = g.d.current_floor;

    const int vision_dist = g.ct.get<vision_distance>(g.hero_id).value_or(0);
    const int light_rad = g.ct.get<light_radius>(g.hero_id).value_or(0);

    // render tiles
    for (int y = 0; y < df->get_height(); y++) {
        // example simple loop unrolling
        //for (int x = 0; x < df->get_width(); x += 4) {
        for (int x = 0; x < df->get_width(); x += 2) {
            draw_dungeon_floor_tile(g, x, y, z);
            draw_dungeon_floor_tile(g, x + 1, y, z);
            //draw_dungeon_floor_tile(g, x + 1, y, z);
            //draw_dungeon_floor_tile(g, x + 2, y, z);
            //draw_dungeon_floor_tile(g, x + 3, y, z);
        }
    }

    auto mydefault = [](gamestate& g, entityid id) { return true; };

    auto alive_check = [](gamestate& g, entityid id) {
        auto maybe_dead = g.ct.get<dead>(id);
        if (maybe_dead.has_value()) {
            return !maybe_dead.value();
        }
        return false;
    };

    auto dead_check = [](gamestate& g, entityid id) {
        auto maybe_dead = g.ct.get<dead>(id);
        if (maybe_dead.has_value()) {
            return maybe_dead.value();
        }
        return false;
    };

    draw_dungeon_floor_entitytype(g, ENTITY_DOOR, vision_dist, light_rad, mydefault);
    draw_dungeon_floor_entitytype(g, ENTITY_PROP, vision_dist, light_rad, mydefault);

    libdraw_draw_player_target_box(g);

    draw_dungeon_floor_entitytype(g, ENTITY_SPELL, vision_dist, light_rad, mydefault);
    draw_dungeon_floor_entitytype(g, ENTITY_NPC, vision_dist, light_rad, dead_check);
    draw_dungeon_floor_entitytype(g, ENTITY_BOX, vision_dist, light_rad, mydefault);
    draw_dungeon_floor_entitytype(g, ENTITY_ITEM, vision_dist, light_rad, mydefault);
    draw_dungeon_floor_entitytype(g, ENTITY_NPC, vision_dist, light_rad, alive_check);
    draw_dungeon_floor_entitytype(g, ENTITY_PLAYER, vision_dist, light_rad, mydefault);

    return true;
}
