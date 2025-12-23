#pragma once

#include "gamestate.h"

static inline int get_npc_xp(gamestate& g, entityid id) {
    return g.ct.get<xp>(id).value_or(0);
}
