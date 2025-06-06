#pragma once
#include "entityid.h"
#include <raylib.h>
typedef struct rect_component_t {
    entityid id;
    Rectangle data;
} rect_component;
