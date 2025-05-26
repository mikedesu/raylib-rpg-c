#pragma once

#include "elemental.h"
#include "entityid.h"
#include "roll.h"

typedef struct spell_effect_t {
    entityid id;
    elemental element;
    int area;
    roll damage;

} spell_effect;
