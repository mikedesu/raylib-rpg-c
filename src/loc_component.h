#pragma once
#include "entityid.h"
#include "location.h"
typedef struct loc_component_t {
    entityid id;
    loc_t loc;
} loc_component;
