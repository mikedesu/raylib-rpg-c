#pragma once
#include "entityid.h"
#include "equipment_slot.h"
typedef struct equipment_component_t {
    entityid id;
    entityid equipment[MAX_EQUIPMENT_SLOTS];
} equipment_component;
