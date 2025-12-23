#pragma once


#include "gamestate.h"

static inline string get_entity_name(gamestate& g, entityid id) {
    return g.ct.get<name>(id).value_or("no-name-entity");
}
