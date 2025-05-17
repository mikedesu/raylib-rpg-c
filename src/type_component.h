#pragma once
#include "entityid.h"
#include "entitytype.h"
typedef struct type_component_t {
    entityid id;
    entitytype_t type;
} type_component;
