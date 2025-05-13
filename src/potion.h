#pragma once

#include "roll.h"

typedef enum potion_type_t
{
    POTION_NONE,
    POTION_HEALING,
    POTION_TYPE_COUNT
} potion_type;

typedef struct potion_effect_t {
    potion_type type;

    roll effect;

    int bonus;
    int duration;
} potion_effect;

// (potion_effect_t) {POTION_NONE, (roll){0, 0, 0}, 0, 0};
