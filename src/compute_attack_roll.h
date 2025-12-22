#pragma once

#include "compute_armor_class.h"
#include "gamestate.h"
#include "stat_bonus.h"

static inline bool compute_attack_roll(gamestate& g, entityid attacker, entityid target) {
    const int str = g.ct.get<strength>(attacker).value_or(10);
    const int bonus = get_stat_bonus(str);
    const int roll = GetRandomValue(1, 20) + bonus;
    const int ac = compute_armor_class(g, target);
    return roll >= ac;
}
