#pragma once
#include "entityid.h"
#include "location.h"
typedef struct target_path_component_t {
    entityid id;
    loc_t* target_path;
    int target_path_length;
} target_path_component;
