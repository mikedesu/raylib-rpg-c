#pragma once
#include "entityid.h"
#include "roll.h"
typedef struct damage_component_t {
    entityid id;
    roll r;
} damage_component;
