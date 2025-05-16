#pragma once
#include "entityid.h"
#include "potion.h"
typedef struct potion_type_component_t {
    entityid id;
    potiontype type;
} potiontype_component;
