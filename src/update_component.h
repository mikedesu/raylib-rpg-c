#pragma once
#include "entityid.h"
typedef struct update_component_t {
    entityid id;
    bool update;
} update_component;
