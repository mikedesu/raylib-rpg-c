#pragma once
#include "entityid.h"
#include <stdbool.h>
typedef struct blocking_component_t {
    entityid id;
    bool blocking;
} blocking_component;
