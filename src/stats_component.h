#pragma once
#include "entityid.h"
#include "stats_slot.h"
typedef struct stats_component_t {
    entityid id;
    int stats[MAX_STATS_SLOTS];
} stats_component;
