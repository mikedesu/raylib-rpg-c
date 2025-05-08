#pragma once

#include "component.h"
#include "entityid.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct component_set_t {
    entityid id;

    component* components;
    size_t component_count;
    size_t component_capacity;
} component_set;

component_set* component_set_create(entityid id);
void component_set_destroy(component_set* set);
void component_set_add_component(component_set* set, component comp);
void component_set_remove_component(component_set* set, component comp);
bool component_set_has_component(component_set* set, component comp);
component* component_set_get_component(component_set* set, size_t index);
void component_set_clear(component_set* set);
