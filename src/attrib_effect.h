#pragma once

#include "roll.h"
#include "stats_slot.h"

//typedef enum attrib_effect_type_t
//{
//    ATTRIB_EFFECT_NONE,
//    ATTRIB_EFFECT_HEALING,
//    ATTRIB_EFFECT_COUNT
//} attrib_effect_type;

typedef struct attrib_effect_t {
    //attrib_effect_type_t type;
    stats_slot stat;
    roll value;
} attrib_effect;
