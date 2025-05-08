#include "component_set.h"

component_set* component_set_create(entityid id) {
    component_set* set = malloc(sizeof(component_set));
    if (!set) { return NULL; }
    set->id = id;
    set->components = NULL;
    set->component_count = 0;
    set->component_capacity = 0;
    return set;
}

void component_set_destroy(component_set* set) {
    if (set) {
        free(set->components);
        free(set);
    }
}

void component_set_add_component(component_set* set, component comp) {
    if (set->component_count >= set->component_capacity) {
        size_t new_capacity = set->component_capacity == 0 ? 1 : set->component_capacity * 2;
        component* new_components = realloc(set->components, new_capacity * sizeof(component));
        if (!new_components) { return; }
        set->components = new_components;
        set->component_capacity = new_capacity;
    }
    set->components[set->component_count++] = comp;
}

void component_set_remove_component(component_set* set, component comp) {
    for (size_t i = 0; i < set->component_count; ++i) {
        if (set->components[i] == comp) {
            set->components[i] = set->components[--set->component_count];
            return;
        }
    }
}

bool component_set_has_component(component_set* set, component comp) {
    for (size_t i = 0; i < set->component_count; ++i) {
        if (set->components[i] == comp) { return true; }
    }
    return false;
}

component* component_set_get_component(component_set* set, size_t index) {
    if (index < set->component_count) { return &set->components[index]; }
    return NULL;
}

void component_set_clear(component_set* set) { set->component_count = 0; }
