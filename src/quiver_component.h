#pragma once
#include "entityid.h"
#include <stddef.h>
#define MAX_QUIVER_SIZE 32
typedef struct quiver_component_t {
    entityid id;
    entityid arrows[MAX_QUIVER_SIZE]; // max 32 arrows
    size_t count;
    size_t capacity;
} quiver_component;
