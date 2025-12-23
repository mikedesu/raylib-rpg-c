#pragma once

#include "gamestate.h"

static inline const vec3 get_entity_location(gamestate& g, const entityid id) {
    return g.ct.get<location>(id).value_or((vec3){-1, -1, -1});
}
