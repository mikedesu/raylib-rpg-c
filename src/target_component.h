#pragma once
#include "entityid.h"
#include "location.h"
typedef struct target_component_t {
    entityid id;
    loc_t target;
} target_component;
