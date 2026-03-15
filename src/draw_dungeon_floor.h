/** @file draw_dungeon_floor.h
 *  @brief Dungeon-floor tile and entity rendering helpers.
 */

#pragma once

#include "draw_sprite.h"
#include "get_txkey_for_tiletype.h"
#include "is_loc_path_blocked.h"
#include "libdraw_player_target_box.h"
#include "textureinfo.h"

extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

/** @brief Return the Manhattan distance between two dungeon locations. */
constexpr static inline int manhattan_distance(vec3 a, vec3 b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

/** @brief Draw one dungeon floor tile using its cached visibility and texture id. */
static inline bool draw_dungeon_floor_tile(gamestate& g, int x, int y, int z, int light_dist, vec3 hero_loc, int distance) {
    massert(z >= 0 && static_cast<size_t>(z) < g.d.get_floor_count(), "z is oob");
    // Calculate drawing position
    const float px = x * DEFAULT_TILE_SIZE + DEFAULT_OFFSET;
    const float py = y * DEFAULT_TILE_SIZE + DEFAULT_OFFSET;
    const Rectangle src = {0, 0, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};
    const Rectangle dest = {px, py, DEFAULT_TILE_SIZE_FLOAT, DEFAULT_TILE_SIZE_FLOAT};
    auto df = g.d.get_floor(z);
    massert(df, "dungeon_floor is NULL");
    massert(x >= 0 && x < df->get_width(), "x is oob");
    massert(y >= 0 && y < df->get_height(), "y is oob");
    massert(!vec3_invalid(vec3{x, y, z}), "loc is invalid");
    tile_t& tile = df->tile_at(vec3{x, y, z});
    const bool full_light = df->get_full_light();
    if (tile.get_type() == TILE_NONE) {
        return true;
    }
    if (!full_light && !tile.get_explored()) {
        return true;
    }
    // Get tile texture
    const int txkey = get_txkey_for_tiletype(tile.get_type());
    massert(txkey >= 0, "txkey is invalid");
    Texture2D* texture = &txinfo[txkey].texture;
    massert(texture->id > 0, "texture->id is <= 0");
    const bool tile_visible = full_light || tile.get_visible();
    const unsigned char a = tile_visible ? 255 : 102;
    const Color draw_color = Color{255, 255, 255, a};
    DrawTexturePro(*texture, src, dest, Vector2{0, 0}, 0, draw_color);
    return true;
}

/** @brief Draw floor pressure plates as overlays after floor tiles but before all entities. */
static inline void draw_dungeon_floor_pressure_plates(gamestate& g, int light_rad) {
    auto df = g.d.get_current_floor();
    const int z = g.d.current_floor;
    const vec3 hero_loc = g.ct.get<location>(g.hero_id).value_or(vec3{-1, -1, -1});
    const bool full_light = df->get_full_light();
    const Rectangle src = {0, 0, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};

    for (const floor_pressure_plate_t& plate : g.floor_pressure_plates) {
        if (plate.destroyed || plate.loc.z != z) {
            continue;
        }

        if (!full_light) {
            if (abs(plate.loc.x - hero_loc.x) + abs(plate.loc.y - hero_loc.y) > light_rad) {
                continue;
            }

            tile_t& tile = df->tile_at(plate.loc);
            if (!tile.get_explored() || !tile.get_visible()) {
                continue;
            }
        }

        const int txkey = plate.active ? plate.txkey_down : plate.txkey_up;
        Texture2D* texture = &txinfo[txkey].texture;
        massert(texture->id > 0, "pressure plate texture->id is <= 0");

        const float px = plate.loc.x * DEFAULT_TILE_SIZE + DEFAULT_OFFSET;
        const float py = plate.loc.y * DEFAULT_TILE_SIZE + DEFAULT_OFFSET;
        const Rectangle dest = {px, py, DEFAULT_TILE_SIZE_FLOAT, DEFAULT_TILE_SIZE_FLOAT};
        DrawTexturePro(*texture, src, dest, Vector2{0, 0}, 0, WHITE);
    }
}

/** @brief Draw one entity category across the current floor subject to visibility checks. */
static inline void
draw_dungeon_floor_entitytype(gamestate& g, entitytype_t type_0, int vision_dist, int light_rad, function<bool(gamestate&, entityid)> extra_check) {
    auto df = g.d.get_current_floor();
    auto hero_loc = g.ct.get<location>(g.hero_id).value_or(vec3{-1, -1, -1});
    const bool full_light = df->get_full_light();
    //massert(!vec3_invalid(hero_loc), "hero loc is invalid");
    // we should be only checking tiles
    // within the hero's light radius / vision distance
    // that way, we avoid the is_loc_too_far_to_draw check entirely!
    // the rest of the inner-loop still applies,
    // but i expect that to provide a nice
    // performance boost as well
    //
    //
    //
    // we have a similar double-loop in the gamestate.tick() method,
    const int min_x = full_light ? 0 : std::max(0, hero_loc.x - light_rad);
    const int max_x = full_light ? df->get_width() - 1 : std::min(df->get_width() - 1, hero_loc.x + light_rad);
    const int min_y = full_light ? 0 : std::max(0, hero_loc.y - light_rad);
    const int max_y = full_light ? df->get_height() - 1 : std::min(df->get_height() - 1, hero_loc.y + light_rad);
    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            if (!full_light && abs(x - hero_loc.x) + abs(y - hero_loc.y) > light_rad) {
                continue;
            }
            const vec3 loc = {x, y, g.d.current_floor};
            tile_t& tile = df->tile_at(loc);
            auto tiletype = tile.get_type();
            if (tiletype_is_none(tiletype) || tiletype_is_wall(tiletype)) {
                continue;
            }
            if (!full_light && (!tile.get_visible() || !tile.get_explored())) {
                continue;
            }
            if (tile.entity_count() == 0) {
                continue;
            }
            // render all entities if not blocked
            //if (is_loc_path_blocked(g, df, loc, hero_loc)) {
            //    continue;
            //}

            if (type_0 == ENTITY_NPC) {
                entityid dead_npc_id = tile.get_cached_dead_npc();
                if (dead_npc_id != INVALID && extra_check(g, dead_npc_id)) {
                    draw_sprite_and_shadow(g, dead_npc_id);
                }
                entityid npc_id = tile.get_cached_live_npc();
                if (npc_id != INVALID && extra_check(g, npc_id)) {
                    draw_sprite_and_shadow(g, npc_id);
                }
            }
            else if (type_0 == ENTITY_PLAYER) {
                bool player_present = tile.get_cached_player_present();
                if (player_present && extra_check(g, g.hero_id)) {
                    draw_sprite_and_shadow(g, g.hero_id);
                }
            }
            else if (type_0 == ENTITY_BOX) {
                entityid box_id = tile.get_cached_box();
                if (box_id != INVALID && extra_check(g, box_id)) {
                    draw_sprite_and_shadow(g, box_id);
                }
            }
            else if (type_0 == ENTITY_CHEST) {
                entityid chest_id = tile.get_cached_chest();
                if (chest_id != INVALID && extra_check(g, chest_id)) {
                    draw_sprite_and_shadow(g, chest_id);
                }
            }
            else if (type_0 == ENTITY_PROP) {
                entityid prop_id = tile.get_cached_prop();
                if (prop_id != INVALID && extra_check(g, prop_id)) {
                    draw_sprite_and_shadow(g, prop_id);
                }
            }
            else if (type_0 == ENTITY_ITEM) {
                entityid item_id = tile.get_cached_item();
                if (item_id != INVALID && extra_check(g, item_id)) {
                    draw_sprite_and_shadow(g, item_id);
                }
            }
            else if (type_0 == ENTITY_DOOR) {
                entityid door_id = tile.get_cached_door();
                if (door_id != INVALID && extra_check(g, door_id)) {
                    draw_sprite_and_shadow(g, door_id);
                }
            }
        }
    }
}

/** @brief Draw the current floor's tiles plus entities in renderer layering order. */
static inline bool draw_dungeon_floor(gamestate& g, int vision_dist, int light_rad) {
    shared_ptr<dungeon_floor> df = g.d.get_current_floor();
    const int z = g.d.current_floor;
    auto maybe_hero_loc = g.ct.get<location>(g.hero_id);
    if (!maybe_hero_loc.has_value()) {
        return false;
    }

    const vec3 hero_loc = maybe_hero_loc.value();
    // render tiles
    for (int y = 0; y < df->get_height(); y++) {
        for (int x = 0; x < df->get_width(); x++) {
            const vec3 loc = {x, y, z};
            const int distance = manhattan_distance(loc, hero_loc);
            draw_dungeon_floor_tile(g, x, y, z, light_rad, hero_loc, distance);
        }
    }
    draw_dungeon_floor_pressure_plates(g, light_rad);

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
    draw_dungeon_floor_entitytype(g, ENTITY_CHEST, vision_dist, light_rad, mydefault);
    libdraw_draw_player_target_box(g);
    // Tiles can now cache multiple items, but the floor renderer still draws only the top item as
    // the representative sprite for the stack. That keeps floor rendering stable until stacked-item
    // presentation is designed explicitly.
    draw_dungeon_floor_entitytype(g, ENTITY_ITEM, vision_dist, light_rad, mydefault);
    draw_dungeon_floor_entitytype(g, ENTITY_NPC, vision_dist, light_rad, dead_check);
    draw_dungeon_floor_entitytype(g, ENTITY_BOX, vision_dist, light_rad, mydefault);
    draw_dungeon_floor_entitytype(g, ENTITY_NPC, vision_dist, light_rad, alive_check);
    draw_dungeon_floor_entitytype(g, ENTITY_PLAYER, vision_dist, light_rad, mydefault);
    return true;
}
