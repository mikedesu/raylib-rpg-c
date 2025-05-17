#pragma once
#include "entityid.h"
#include "race.h"
typedef struct race_component_t {
    entityid id;
    race_t race;
} race_component;
