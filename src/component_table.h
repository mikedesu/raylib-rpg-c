#pragma once
#include "component.h"
#include "entityid.h"
#include <stdbool.h>
#include <stdlib.h>

#define CT_DEFAULT_CAPACITY 128

typedef struct component_table_t {
    long int* components; // Use long int to store bits for components
    size_t component_row_count; // Number of entities (rows)
    size_t component_col_count; // Number of components (columns)
    size_t component_capacity; // Capacity of the table
    size_t ints_per_row; // Number of long ints needed per row (N/64 + 1)
} ct;

ct* ct_create();

void ct_destroy(ct* table);

bool ct_add_entity(ct* table, entityid id);
bool ct_has_entity(ct* table, entityid id);
bool ct_add_component(ct* table, entityid id, component comp);
bool ct_has_entity(ct* table, entityid id);
bool ct_has_component(ct* table, entityid id, component comp);
