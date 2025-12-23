#pragma once

#include "gamestate.h"
#include "get_npc_xp.h"

static inline void update_npc_xp(gamestate& g, entityid id, entityid target_id) {
    const int old_xp = get_npc_xp(g, id);
    const int reward_xp = 1;
    const int new_xp = old_xp + reward_xp;
    g.ct.set<xp>(id, new_xp);
}
