#pragma once

#include "gamestate.h"

static inline entitytype_t get_entity_type(gamestate& g, entityid id) {
    return g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
}
