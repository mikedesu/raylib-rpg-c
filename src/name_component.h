#pragma once
#include "entityid.h"
#include <string>
typedef struct name_component_t {
    entityid id;
    char name[32];
} name_component;
