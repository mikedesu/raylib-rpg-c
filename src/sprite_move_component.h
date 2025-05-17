#pragma once
#include "entityid.h"
#include "location.h"
typedef struct sprite_move_component_t {
    entityid id;
    loc_t loc;
} sprite_move_component;
