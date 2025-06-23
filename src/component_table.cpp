#include "component_table.h"
#include "entityid.h"
#include "massert.h"
#include "mprint.h"
//#include "mprint.h"
#include <string.h>

ct* ct_create() {
    ct* table = (ct*)malloc(sizeof(ct));
    massert(table, "malloc failed");
    table->component_col_count = C_COUNT;
    table->component_row_count = 0;
    table->component_capacity = CT_DEFAULT_CAPACITY;
    table->ints_per_row = (table->component_col_count / 64) + 1;
    table->components = (long int*)malloc(table->component_capacity * table->ints_per_row * sizeof(long int));
    if (!table->components) {
        free(table);
        return NULL;
    }
    return table;
}

void ct_destroy(ct* table) {
    if (table) {
        if (table->components) {
            free(table->components);
        }
        free(table);
    }
}

bool ct_add_entity(ct* table, entityid id) {
    massert(table, "table is NULL");
    if (!table || id == ENTITYID_INVALID) {
        return false;
    }
    if (ct_has_entity(table, id)) {
        return false;
    }

    if ((size_t)id >= table->component_capacity) {
        size_t new_capacity;
        if (table->component_capacity == 0) {
            new_capacity = 1;
        } else {
            new_capacity = table->component_capacity * 2;
        }
        size_t num_rows = new_capacity * table->ints_per_row;
        long int* new_components = (long int*)realloc(table->components, num_rows * sizeof(long int));
        if (!new_components) {
            return false;
        }
        // Initialize the new rows to zero
        //new_capacity = table->component_capacity * table->ints_per_row;
        int index = table->component_capacity * table->ints_per_row;
        size_t bytes_to_zero = (new_capacity - table->component_capacity) * table->ints_per_row * sizeof(long int);
        memset(&new_components[index], 0, bytes_to_zero); // Zero out the new rows
        // Update the table with the new components and capacity
        table->components = new_components;
        table->component_capacity = new_capacity;
    }
    if ((size_t)id < table->component_row_count) {
        return false;
    }

    if (table->component_row_count >= table->component_capacity) {
        size_t new_capacity = table->component_capacity == 0 ? 1 : table->component_capacity * 2;
        size_t num_rows = new_capacity * table->ints_per_row;
        long int* new_components = (long int*)realloc(table->components, num_rows * sizeof(long int));
        if (!new_components) {
            return false;
        }
        table->components = new_components;
        table->component_capacity = new_capacity;
    }
    // instead of using a loop to zero the row out, cant we just use memset?
    int index = table->component_row_count * table->ints_per_row;
    memset(&table->components[index], 0, table->ints_per_row * sizeof(long int));
    // the first index in the row is the entity id
    table->components[index] = id;
    ++table->component_row_count;
    return true;
}

bool ct_has_entity(ct* table, entityid id) {
    massert(table, "table is NULL");
    return table && id > ENTITYID_INVALID && (size_t)id < table->component_row_count;
}

bool ct_add_component(ct* table, entityid id, component comp) {
    massert(table, "table is NULL");
    if (!table) {
        return false;
    }

    if (id <= ENTITYID_INVALID) {
        merror("ct_add_component: id is invalid: %d", id);
        return false;
    }

    //if (id >= table->component_row_count) {
    //    merror("ct_add_component: id out of range: %d >= %zu", id, table->component_row_count);
    //    return false;
    //}

    if (comp < 0 || comp >= table->component_col_count) {
        merror("ct_add_component: component out of range: %d not in [0, %zu)", comp, table->component_col_count);
        return false;
    }

    //if (!table || id <= ENTITYID_INVALID || id >= table->component_row_count || comp < 0 ||
    //    comp >= table->component_col_count) {
    //    return false;
    //}
    massert(ct_has_entity(table, id), "ct_add_component: entity not found");
    size_t int_index = comp / 64;
    size_t bit_index = comp % 64; // Determine which bit to set
    size_t comp_index = id * table->ints_per_row + int_index;
    //table->components[id * table->ints_per_row + int_index] |= (1UL << bit_index); // Set the bit
    table->components[comp_index] |= (1UL << bit_index); // Set the bit
    return true;
}

bool ct_has_component(ct* table, entityid id, component comp) {
    massert(table, "table is NULL");
    if (!table) {
        return false;
    }

    if (id <= ENTITYID_INVALID) {
        merror("ct_has_component: id is invalid: %d", id);
        return false;
    }

    //if (id >= table->component_row_count) {
    //    merror("ct_has_component: id out of range: %d >= %zu", id, table->component_row_count);
    //    return false;
    //}

    if (comp < 0 || comp >= table->component_col_count) {
        merror("ct_has_component: component out of range: %d not in [0, %zu)", comp, table->component_col_count);
        return false;
    }

    //if (!table || id <= ENTITYID_INVALID || id >= table->component_row_count || comp < 0 ||
    //    comp >= table->component_col_count)
    //    return false;
    size_t int_index = comp / 64;
    size_t bit_index = comp % 64;
    size_t comp_index = id * table->ints_per_row + int_index;
    return (table->components[comp_index] & (1UL << bit_index)) != 0;
}
