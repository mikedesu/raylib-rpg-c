#pragma once
#include "direction.h"
#include "entityid.h"
typedef struct direction_component_t {
    entityid id;
    direction_t dir;
} direction_component;
