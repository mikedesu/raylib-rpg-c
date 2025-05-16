#include "component_table.h"
#include "entityid.h"
#include "massert.h"
#include "mprint.h"
#include <string.h>

ct* ct_create() {
    ct* table = malloc(sizeof(ct));
    massert(table, "malloc failed");
    table->component_col_count = C_COUNT;
    table->component_row_count = 0;
    table->component_capacity = CT_DEFAULT_CAPACITY;
    table->ints_per_row = (table->component_col_count / 64) + 1;
    table->components = malloc(table->component_capacity * table->ints_per_row * sizeof(long int));
    if (!table->components) {
        free(table);
        return NULL;
    }
    return table;
}

void ct_destroy(ct* table) {
    if (table) {
        if (table->components) free(table->components);
        free(table);
    }
}

bool ct_add_entity(ct* table, entityid id) {
    massert(table, "table is NULL");
    if (!table || id == ENTITYID_INVALID || id >= table->component_row_count || ct_has_entity(table, id)) return false;
    if (table->component_row_count >= table->component_capacity) {
        size_t new_capacity = table->component_capacity == 0 ? 1 : table->component_capacity * 2;
        long int* new_components = realloc(table->components, new_capacity * table->ints_per_row * sizeof(long int));
        if (!new_components) {
            merror("realloc failed");
            return false;
        }
        table->components = new_components;
        table->component_capacity = new_capacity;
    }
    // instead of using a loop to zero the row out, cant we just use memset?
    memset(&table->components[table->component_row_count * table->ints_per_row], 0, table->ints_per_row * sizeof(long int));
    // the first index in the row is the entity id
    table->components[table->component_row_count * table->ints_per_row] = id;
    ++table->component_row_count;
    return true;
}

bool ct_has_entity(ct* table, entityid id) {
    massert(table, "table is NULL");
    return table && id > ENTITYID_INVALID && id < table->component_row_count;
}

bool ct_add_component(ct* table, entityid id, component comp) {
    massert(table, "table is NULL");
    minfo("ct_add_component: id %d comp %s", id, component2str(comp));
    if (!table || id <= ENTITYID_INVALID || id >= table->component_row_count || comp < 0 || comp >= table->component_col_count) return false;
    massert(ct_has_entity(table, id), "ct_add_component: entity not found");
    size_t int_index = comp / 64, bit_index = comp % 64; // Determine which bit to set
    table->components[id * table->ints_per_row + int_index] |= (1UL << bit_index); // Set the bit
    msuccess("ct_add_component: id %d comp %s", id, component2str(comp));
    return true;
}

bool ct_has_component(ct* table, entityid id, component comp) {
    massert(table, "table is NULL");
    if (!table || id <= ENTITYID_INVALID || id >= table->component_row_count || comp < 0 || comp >= table->component_col_count) return false;
    size_t int_index = comp / 64, bit_index = comp % 64;
    return (table->components[id * table->ints_per_row + int_index] & (1UL << bit_index)) != 0;
}
