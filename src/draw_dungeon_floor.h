#pragma once

#include "calculate_linear_path.h"
#include "draw_sprite.h"
#include "dungeon_floor.h"
#include "entitytype.h"
#include "gamestate.h"
#include "get_txkey_for_tiletype.h"
#include "libdraw_player_target_box.h"
#include "textureinfo.h"


using std::for_each;
using std::sort;


extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];


constexpr static inline int manhattan_distance(vec3 a, vec3 b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}


static inline bool draw_dungeon_floor_tile(gamestate& g, textureinfo* txinfo, int x, int y, int z) {
    minfo2("BEGIN draw dungeon floor tile");
    massert(txinfo, "txinfo is null");
    massert(z >= 0 && static_cast<size_t>(z) < g.d.get_floor_count(), "z is oob");
    auto df = g.d.get_floor(z);
    massert(df, "dungeon_floor is NULL");
    massert(x >= 0 && x < df->get_width(), "x is oob");
    massert(y >= 0 && y < df->get_height(), "y is oob");
    const vec3 loc = {x, y, z};
    massert(!vec3_invalid(loc), "loc is invalid");
    auto tile = df->tile_at(vec3{x, y, z});
    massert(tile, "tile is NULL");
    if (tile->get_type() == TILE_NONE || !tile->get_visible() || !tile->get_explored()) {
        minfo2("END draw dungeon floor tile 0");
        return true;
    }

    // Get tile texture
    const int txkey = get_txkey_for_tiletype(tile->get_type());
    if (txkey < 0) {
        minfo2("END draw dungeon floor tile 2");
        return false;
    }
    Texture2D* texture = &txinfo[txkey].texture;
    if (texture->id <= 0) {
        minfo2("END draw dungeon floor tile 3");
        return false;
    }


    // Get hero's total light radius
    const int light_dist = g.ct.get<light_radius>(g.hero_id).value_or(1);
    auto maybe_hero_loc = g.ct.get<location>(g.hero_id);
    if (!maybe_hero_loc.has_value()) {
        minfo2("END draw dungeon floor tile 1");
        return false;
    }
    const vec3 hero_loc = maybe_hero_loc.value();
    // Calculate Manhattan distance from hero to this tile (diamond pattern)
    const int distance = manhattan_distance(loc, hero_loc);

    // Calculate drawing position
    constexpr int offset_x = -12;
    constexpr int offset_y = -12;
    const int px = x * DEFAULT_TILE_SIZE + offset_x;
    const int py = y * DEFAULT_TILE_SIZE + offset_y;
    const Rectangle src = {0, 0, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};
    //const Rectangle dest = {(float)px, (float)py, (float)DEFAULT_TILE_SIZE_SCALED, (float)DEFAULT_TILE_SIZE_SCALED};
    const float f_px = px;
    const float f_py = py;
    const Rectangle dest = {f_px, f_py, DEFAULT_TILE_SIZE_FLOAT, DEFAULT_TILE_SIZE_FLOAT};
    // Draw tile with fade ALSO if path between tile and hero is blocked
    auto path = calculate_path_with_thickness({x, y, z}, hero_loc);
    // Check for blocking walls/doors in path
    bool blocking = false;
    for (const auto& v : path) {
        auto tile = df->tile_at(v);
        if (tiletype_is_none(tile->get_type()) || tiletype_is_wall(tile->get_type())) {
            blocking = true;
            break;
        }
        for (const entityid id : *tile->get_entities()) {
            auto type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
            auto is_open = g.ct.get<door_open>(id).value_or(false);
            // Check for closed doors
            if (type == ENTITY_DOOR && !is_open) {
                blocking = true;
                break;
            }
            // Check for boxes
            if (type == ENTITY_BOX) {
                blocking = true;
                break;
            }
        }
        if (blocking)
            break;
    }
    // Apply fade if blocked
    const unsigned char a = blocking ? 31 : distance > light_dist ? 102 : 255;
    const Color draw_color = Color{255, 255, 255, a};
    DrawTexturePro(*texture, src, dest, Vector2{0, 0}, 0, draw_color);
    minfo2("END draw dungeon floor tile 5");
    return true;
}


static inline bool is_loc_too_far_to_draw(gamestate& g, vec3 loc, vec3 hero_loc) {
    // Get hero's vision distance and location
    const int vision_dist = g.ct.get<vision_distance>(g.hero_id).value_or(0);
    const int light_rad = g.ct.get<light_radius>(g.hero_id).value_or(0);
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
    bool object_blocking = false;
    for (auto v0 : path) {
        auto v0_tile = df->tile_at(v0);
        auto v0_tiletype = v0_tile->get_type();
        if (tiletype_is_none(v0_tiletype) || tiletype_is_wall(v0_tiletype)) {
            object_blocking = true;
            break;
        }
        for (const entityid id : *v0_tile->get_entities()) {
            auto type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
            auto is_open = g.ct.get<door_open>(id).value_or(false);
            // check if tile has a DOOR
            if (type == ENTITY_DOOR && !is_open) {
                object_blocking = true;
                break;
            }

            // Check for boxes
            if (type == ENTITY_BOX) {
                object_blocking = true;
                break;
            }
        }
        if (object_blocking)
            break;
    }
    return object_blocking;
}


static inline void libdraw_draw_dungeon_floor_entitytype(gamestate& g, entitytype_t type_0, function<bool(gamestate&, entityid)> extra_check) {
    auto df = g.d.get_current_floor();
    const int df_w = df->get_width(), df_h = df->get_height(), num_tiles = df_w * df_h;
    for (int i = 0; i < num_tiles; i++) {
        const int y = i / df_w, x = i - (y * df_w);
        const vec3 loc = {x, y, g.d.current_floor};
        auto tile = df->tile_at(loc);
        auto tiletype = tile->get_type();
        if (tiletype_is_none(tiletype) || tiletype_is_wall(tiletype))
            continue;
        auto hero_loc = g.ct.get<location>(g.hero_id).value_or(vec3{-1, -1, -1});
        massert(!vec3_invalid(hero_loc), "hero loc is invalid");
        if (is_loc_too_far_to_draw(g, loc, hero_loc))
            continue;
        // bugfix for tall walls so entities do not draw on top:
        // check to see if the tile directly beneath this tile is a wall
        //if (tile2.type == TILE_STONE_WALL_00)
        //    continue;
        // render all entities if not blocked
        if (is_loc_path_blocked(g, df, loc, hero_loc))
            continue;
        auto entities_begin = tile->get_entities()->cbegin();
        auto entities_end = tile->get_entities()->cend();
        for_each(entities_begin, entities_end, [&g, type_0, &extra_check](entityid id) {
            const entitytype_t type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
            //auto sm = g.ct.get<spritemove>(id).value_or((Rectangle){0, 0, 0, 0});
            //spritegroup_t* sg = spritegroups[id];
            //if (sg->move.x != 0 || sg->move.y != 0) {
            //    draw_sprite_and_shadow(g, id);
            //}
            if (type_0 == type && extra_check(g, id))
                draw_sprite_and_shadow(g, id);
        });
    }
}


static inline bool draw_dungeon_floor(gamestate& g) {
    shared_ptr<dungeon_floor> df = g.d.get_current_floor();
    const int z = g.d.current_floor;
    // render tiles
    for (int i = 0; i < df->get_height() * df->get_width(); i++) {
        const int y = i / df->get_width();
        const int x = i - (y * df->get_width());
        draw_dungeon_floor_tile(g, txinfo, x, y, z);
    }
    auto mydefault = [](gamestate& g, entityid id) { return true; };
    auto alive_check = [](gamestate& g, entityid id) {
        auto maybe_dead = g.ct.get<dead>(id);
        if (maybe_dead.has_value())
            return !maybe_dead.value();
        return false;
    };
    auto dead_check = [](gamestate& g, entityid id) {
        auto maybe_dead = g.ct.get<dead>(id);
        if (maybe_dead.has_value())
            return maybe_dead.value();
        return false;
    };
    //minfo2("BEGIN draw dungeon floor --- rendering entities");
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_DOOR, mydefault);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_PROP, mydefault);
    libdraw_draw_player_target_box(g);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_SPELL, mydefault);

    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_NPC, dead_check);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_BOX, mydefault);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_ITEM, mydefault);

    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_NPC, alive_check);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_PLAYER, mydefault);
    //minfo2("END draw dungeon floor --- rendering entities");
    //minfo2("END draw dungeon floor");
    return true;
}
