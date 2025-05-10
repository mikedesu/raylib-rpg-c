#pragma once

#include "component.h"
#include "entityid.h"
#include <stdbool.h>
#include <stdlib.h>

#define CT_DEFAULT_CAPACITY 128

typedef struct component_table_t {
    int* components;
    size_t component_row_count;
    size_t component_col_count;
    size_t component_capacity;
} ct;

ct* ct_create();
void ct_destroy(ct* table);

bool ct_add_entity(ct* table, entityid id);
bool ct_remove_entity(ct* table, entityid id);
bool ct_has_entity(ct* table, entityid id);

bool ct_add_component(ct* table, entityid id, component comp);
bool ct_has_entity(ct* table, entityid id);
bool ct_has_component(ct* table, entityid id, component comp);
