#pragma once
#include "entityid.h"
#include "vec3.h"
//#include "location.h"
typedef struct target_path_component_t {
    entityid id;
    vec3* target_path;
    int target_path_length;
} target_path_component;
