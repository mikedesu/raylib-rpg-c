#pragma once

#include "gamestate.h"
#include "stat_bonus.h"

static inline int compute_armor_class(gamestate& g, entityid id) {
    //massert(g, "g is null");
    massert(ENTITYID_INVALID != id, "id is invalid");

    const int base_armor_class = g.ct.get<base_ac>(id).value_or(10);
    const int dex_bonus = get_stat_bonus(g.ct.get<dexterity>(id).value_or(10));

    // here, we would also grab any armor pieces that we are wearing

    const int total_ac = base_armor_class + dex_bonus;
    return total_ac;
}
