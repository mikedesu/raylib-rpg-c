/** @file is_loc_path_blocked.h
 *  @brief Line-of-path obstruction helper for dungeon-space checks.
 */

#pragma once

#include "calculate_linear_path.h"
#include "gamestate.h"
#include "vec3.h"
#include <vector>

using std::vector;

/**
 * @brief Return whether the path from `loc` to `hero_loc` is blocked by geometry or entities.
 *
 * Closed doors, walls, boxes, and living NPCs count as blockers.
 */
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
        tile_t& v0_tile = df->tile_at(v0);
        auto v0_tiletype = v0_tile.get_type();
        if (tiletype_is_none(v0_tiletype) || tiletype_is_wall(v0_tiletype)) {
            blocked = true;
            break;
        }
        if (v0_tile.entity_count() == 0) {
            continue;
        }
        // check if tile has a DOOR
        entityid door_id = v0_tile.get_cached_door();
        if (door_id != INVALID) {
            auto is_open = g.ct.get<door_open>(door_id).value_or(false);
            if (!is_open) {
                blocked = true;
                break;
            }
        }
        entityid box_id = v0_tile.get_cached_box();
        if (box_id != INVALID) {
            blocked = true;
            break;
        }
        entityid npc_id = v0_tile.get_cached_live_npc();
        if (npc_id != INVALID) {
            massert(g.ct.has<dead>(npc_id), "id %d doesn't have a dead component", npc_id);
            const bool is_dead = g.ct.get<dead>(npc_id).value();
            if (!is_dead) {
                blocked = true;
                break;
            }
        }
    }
    return blocked;
}
