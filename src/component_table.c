#include "component_table.h"
#include "massert.h"
#include "mprint.h"

ct* ct_create() {
    ct* table = malloc(sizeof(ct));
    if (!table) { return NULL; }
    table->component_col_count = COMP_COUNT;
    table->component_row_count = 0;
    table->component_capacity = CT_DEFAULT_CAPACITY;

    table->components = malloc(table->component_capacity * table->component_col_count * sizeof(int));
    if (!table->components) {
        free(table);
        return NULL;
    }

    return table;
}

void ct_destroy(ct* table) {
    if (table) {
        if (table->components) { free(table->components); }
        free(table);
    }
}

bool ct_add_entity(ct* table, entityid id) {
    if (!table) {
        merror("table is NULL");
        return false;
    }
    if (id <= ENTITYID_INVALID) {
        merror("id is invalid");
        return false;
    }
    if (id > table->component_row_count) {
        merror("id is out of bounds: %u >= %zu", id, table->component_row_count);
        return false;
    }
    if (table->component_row_count >= table->component_capacity) {
        size_t new_capacity = table->component_capacity == 0 ? 1 : table->component_capacity * 2;
        int* new_components = realloc(table->components, new_capacity * table->component_col_count * sizeof(int));
        //massert(new_components, "realloc failed");
        if (!new_components) {
            merror("realloc failed");
            return false;
        }
        table->components = new_components;
        table->component_capacity = new_capacity;
    }
    for (size_t i = 0; i < table->component_col_count; ++i) { table->components[table->component_row_count * table->component_col_count + i] = 0; }
    // the first index in the row is the entity id
    table->components[table->component_row_count * table->component_col_count] = id;
    ++table->component_row_count;
    return true;
}

bool ct_remove_entity(ct* table, entityid id) {
    if (!table) { return false; }
    if (id <= ENTITYID_INVALID) { return false; }
    if (id >= table->component_row_count) { return false; }
    for (size_t i = id; i < table->component_row_count - 1; ++i) {
        for (size_t j = 0; j < table->component_col_count; ++j) {
            table->components[i * table->component_col_count + j] = table->components[(i + 1) * table->component_col_count + j];
        }
    }
    --table->component_row_count;
    return true;
}

bool ct_add_component(ct* table, entityid id, component comp) {
    if (!table) { return false; }
    if (id <= ENTITYID_INVALID) { return false; }
    if (id >= table->component_row_count) { return false; }
    if (comp < 0 || comp >= table->component_col_count) { return false; }
    table->components[id * table->component_col_count + comp] = 1;
    return true;
}

bool ct_has_entity(ct* table, entityid id) {
    if (!table) { return false; }
    if (id <= ENTITYID_INVALID) { return false; }
    if (id >= table->component_row_count) { return false; }
    return true;
}

bool ct_has_component(ct* table, entityid id, component comp) {
    if (!table) { return false; }
    if (id <= ENTITYID_INVALID) { return false; }
    if (id >= table->component_row_count) { return false; }
    if (comp < 0 || comp >= table->component_col_count) { return false; }
    return table->components[id * table->component_col_count + comp] == 1;
}
