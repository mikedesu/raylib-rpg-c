#pragma once

#include "roll.h"
#include "stats_slot.h"
#include <stdbool.h>

typedef enum potion_type_t
{
    POTION_NONE,
    POTION_HP_SMALL,
    POTION_HP_MEDIUM,
    POTION_HP_LARGE,
    POTION_TYPE_COUNT
} potiontype;

typedef struct potion_effect_t {
    potiontype type;
    stats_slot stat;
    bool curative;
    //roll calc;
    //roll duration;
} potioneffect;

// example "effect" based on attack: 1d3 damage
// (potioneffect){POTION_INSTANT, STATS_HP, false, (roll){1, 3, 0}, (roll){0, 0, 0}}
// example "effect" based on light healing: 1d4 hp heal
// (potioneffect){POTION_INSTANT, STATS_HP, true, (roll){1, 4, 0}, (roll){0, 0, 0}}
// example "effect" based on poison: 1d1 damage every turn for 1d4+1 turns
// (potioneffect){POTION_INSTANT, STATS_HP, false, (roll){1, 1, 0}, (roll){1, 4, 1}}
