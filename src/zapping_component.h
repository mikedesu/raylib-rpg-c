#pragma once
#include "entityid.h"
#include <stdbool.h>
typedef struct zapping_component_t {
    entityid id;
    bool zapping;
} zapping_component;
