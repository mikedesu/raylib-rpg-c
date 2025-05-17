#pragma once
#include "entityid.h"
#include <stdbool.h>
typedef struct attacking_component_t {
    entityid id;
    bool attacking;
} attacking_component;
