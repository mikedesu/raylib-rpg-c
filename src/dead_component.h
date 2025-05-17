#pragma once
#include "entityid.h"
typedef struct dead_component_t {
    entityid id;
    bool dead;
} dead_component;
