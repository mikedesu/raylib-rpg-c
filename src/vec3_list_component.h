#pragma once
#include "entityid.h"
#include "vec3.h"
typedef struct vec3_list_component_t {
    entityid id;
    vec3* list;
    int count;
    int capacity;
} vec3_list_component;
