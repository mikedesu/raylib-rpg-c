#pragma once
#include "entityid.h"
#include <stddef.h>
#define MAX_INVENTORY_SIZE 32
typedef struct inventory_component_t {
    entityid id;
    entityid inventory[MAX_INVENTORY_SIZE]; // max 32 items
    size_t count;
} inventory_component;
