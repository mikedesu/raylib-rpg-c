/** @file get_entity_location.h
 *  @brief Convenience accessor for an entity's dungeon-space location.
 */

#pragma once

#include "gamestate.h"

/** @brief Return the entity location for `id`, or the invalid location sentinel. */
static inline const vec3 get_entity_location(gamestate& g, const entityid id) {
    return g.ct.get<location>(id).value_or((vec3){-1, -1, -1});
}
