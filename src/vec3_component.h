#pragma once
#include "entityid.h"
#include "vec3.h"
typedef struct vec3_component_t {
    entityid id;
    vec3 data;
} vec3_component;
