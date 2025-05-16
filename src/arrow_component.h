#pragma once
#include "arrow.h"
#include "entityid.h"
#include <stddef.h>
typedef struct arrow_type_component_t {
    entityid id;
    arrowtype type;
} arrowtype_component;
