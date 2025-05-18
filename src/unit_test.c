#include "component_table.h"
#include "unit_test.h"

TEST(create_ct_0) {
    ct* table = ct_create();
    ASSERT(table != NULL, "ct creation failed");
    ASSERT(table->component_row_count == 0, "row count should be 0");
    ASSERT(table->component_col_count == C_COUNT, "col count should be C_COUNT");
    ASSERT(table->component_capacity == CT_DEFAULT_CAPACITY, "capacity should be CT_DEFAULT_CAPACITY");
    ct_destroy(table);
}

TEST(create_ct_1) {
    ct* table = ct_create();
    entityid id = 0;
    ASSERT(table != NULL, "ct creation failed");
    ASSERT(table->component_row_count == 0, "row count should be 0");
    ASSERT(table->component_col_count == C_COUNT, "col count should be C_COUNT");
    ASSERT(table->component_capacity == CT_DEFAULT_CAPACITY, "capacity should be CT_DEFAULT_CAPACITY");
    ASSERT(ct_add_entity(table, id), "add entity failed");
    ASSERT(ct_add_component(table, id, C_ID), "add component id failed");
    ASSERT(ct_add_component(table, id, C_NAME), "add component name failed");
    ASSERT(ct_has_entity(table, id), "entity should exist");
    id++;
    ASSERT(ct_add_entity(table, id), "add entity failed");
    ASSERT(ct_add_component(table, id, C_ID), "add component id failed");
    ASSERT(ct_add_component(table, id, C_NAME), "add component name failed");
    ASSERT(ct_has_entity(table, id), "entity should exist");

    ct_destroy(table);
}

TEST(test_ct_has_entity) {
    ct* table = ct_create();
    entityid id = 0;
    ASSERT(table != NULL, "ct creation failed");
    ASSERT(!ct_has_entity(table, id), "entity should not exist yet");
    ASSERT(ct_add_entity(table, id), "add entity failed");
    ASSERT(ct_has_entity(table, id), "entity should exist");
    ASSERT(!ct_has_entity(table, id + 1), "entity should not exist yet");

    ct_destroy(table);
}

TEST(test_ct_add_component) {
    ct* table = ct_create();
    entityid id = 0;
    ASSERT(table != NULL, "ct creation failed");
    ASSERT(ct_add_entity(table, id), "add entity failed");
    ASSERT(ct_add_component(table, id, C_ID), "add component id failed");
    ASSERT(ct_add_component(table, id, C_NAME), "add component name failed");
    ct_destroy(table);
}

TEST(test_ct_has_component) {
    ct* table = ct_create();
    entityid id = 0;
    ASSERT(table != NULL, "ct creation failed");
    ASSERT(ct_add_entity(table, id), "add entity failed");
    ASSERT(ct_add_component(table, id, C_ID), "add component id failed");
    ASSERT(ct_has_component(table, id, C_ID), "component id should exist");
    ASSERT(!ct_has_component(table, id, C_NAME), "component name should not exist yet");

    ct_destroy(table);
}

TEST(test_boundary_conditions) {
    ct* table = ct_create();
    ASSERT(table != NULL, "ct creation failed");

    // Fill the table to its default capacity
    for (entityid id = 0; id < CT_DEFAULT_CAPACITY; ++id) {
        ASSERT(ct_add_entity(table, id), "add entity failed");
        ASSERT(ct_add_component(table, id, C_ID), "add component id failed");
    }

    // Add one more entity to trigger realloc
    entityid id = CT_DEFAULT_CAPACITY;
    ASSERT(ct_add_entity(table, id), "add entity failed");
    ASSERT(ct_add_component(table, id, C_ID), "add component id failed");

    ct_destroy(table);
}

static void test_component_table(void) {
    run_create_ct_0();
    run_create_ct_1();
    run_test_ct_has_entity();
    run_test_ct_add_component();
    run_test_ct_has_component();
    run_test_boundary_conditions();
}

int main(void) {
    test_component_table();
    unit_test_summary();
    return 0;
}
