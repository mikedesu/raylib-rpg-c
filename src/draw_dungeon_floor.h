/** @file draw_dungeon_floor.h
 *  @brief Dungeon-floor tile and entity rendering helpers.
 */

#pragma once

#include "gamestate.h"

/** @brief Return the Manhattan distance between two dungeon locations. */
constexpr int manhattan_distance(vec3 a, vec3 b);

/** @brief Draw one dungeon floor tile using its cached visibility and texture id. */
bool draw_dungeon_floor_tile(gamestate& g, int x, int y, int z, int light_dist, vec3 hero_loc, int distance);

/** @brief Draw floor pressure plates as overlays after floor tiles but before all entities. */
void draw_dungeon_floor_pressure_plates(gamestate& g, int light_rad);

/** @brief Draw one entity category across the current floor subject to visibility checks. */
void draw_dungeon_floor_entitytype(gamestate& g, entitytype_t type_0, int vision_dist, int light_rad, function<bool(gamestate&, entityid)> extra_check);

/** @brief Draw the current floor's tiles plus entities in renderer layering order. */
bool draw_dungeon_floor(gamestate& g, int vision_dist, int light_rad);
