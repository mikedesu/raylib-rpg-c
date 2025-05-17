#pragma once
#include "entityid.h"
#include <stdbool.h>
typedef struct block_success_component_t {
    entityid id;
    bool block_success;
} block_success_component;
