#pragma once
#include "entityid.h"
#include "spell_effect.h"
typedef struct spell_effect_component_t {
    entityid id;
    spell_effect effect;
} spell_effect_component;
