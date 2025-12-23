#pragma once

#include "compute_armor_class.h"
#include "gamestate.h"
#include "magic_values.h"
#include "stat_bonus.h"

static inline int compute_attack_damage(gamestate& g, entityid attacker, entityid target) {
    const int str = g.ct.get<strength>(attacker).value_or(10);
    const int bonus = get_stat_bonus(str);
    //const int roll = GetRandomValue(1, 20) + bonus;
    //const int ac = compute_armor_class(g, target);

    const entityid equipped_wpn = g.ct.get<equipped_weapon>(attacker).value_or(ENTITYID_INVALID);
    const vec3 dmg_range = g.ct.get<damage>(equipped_wpn).value_or(MINIMUM_DAMAGE);
    const int dmg = GetRandomValue(dmg_range.x, dmg_range.y);

    return dmg + bonus;
}
