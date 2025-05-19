#pragma once

#include "gamestate.h"
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

TEST(test_all_component_types) {
    ct* table = ct_create();
    entityid id = 0;
    ASSERT(table != NULL, "ct creation failed");
    ASSERT(ct_add_entity(table, id), "add entity failed");
    // Test adding and verifying all component types
    ASSERT(ct_add_component(table, id, C_NAME), "add C_NAME failed");
    ASSERT(ct_has_component(table, id, C_NAME), "C_NAME should exist");
    ASSERT(ct_add_component(table, id, C_TYPE), "add C_TYPE failed");
    ASSERT(ct_has_component(table, id, C_TYPE), "C_TYPE should exist");
    ASSERT(ct_add_component(table, id, C_RACE), "add C_RACE failed");
    ASSERT(ct_has_component(table, id, C_RACE), "C_RACE should exist");
    ASSERT(ct_add_component(table, id, C_DIRECTION), "add C_DIRECTION failed");
    ASSERT(ct_has_component(table, id, C_DIRECTION), "C_DIRECTION should exist");
    ASSERT(ct_add_component(table, id, C_LOCATION), "add C_LOCATION failed");
    ASSERT(ct_has_component(table, id, C_LOCATION), "C_LOCATION should exist");
    ASSERT(ct_add_component(table, id, C_SPRITE_MOVE), "add C_SPRITE_MOVE failed");
    ASSERT(ct_has_component(table, id, C_SPRITE_MOVE), "C_SPRITE_MOVE should exist");
    ASSERT(ct_add_component(table, id, C_DEAD), "add C_DEAD failed");
    ASSERT(ct_has_component(table, id, C_DEAD), "C_DEAD should exist");
    ASSERT(ct_add_component(table, id, C_UPDATE), "add C_UPDATE failed");
    ASSERT(ct_has_component(table, id, C_UPDATE), "C_UPDATE should exist");
    ASSERT(ct_add_component(table, id, C_ATTACKING), "add C_ATTACKING failed");
    ASSERT(ct_has_component(table, id, C_ATTACKING), "C_ATTACKING should exist");
    ASSERT(ct_add_component(table, id, C_BLOCKING), "add C_BLOCKING failed");
    ASSERT(ct_has_component(table, id, C_BLOCKING), "C_BLOCKING should exist");
    ASSERT(ct_add_component(table, id, C_BLOCK_SUCCESS), "add C_BLOCK_SUCCESS failed");
    ASSERT(ct_has_component(table, id, C_BLOCK_SUCCESS), "C_BLOCK_SUCCESS should exist");
    ASSERT(ct_add_component(table, id, C_DAMAGED), "add C_DAMAGED failed");
    ASSERT(ct_has_component(table, id, C_DAMAGED), "C_DAMAGED should exist");
    ASSERT(ct_add_component(table, id, C_INVENTORY), "add C_INVENTORY failed");
    ASSERT(ct_has_component(table, id, C_INVENTORY), "C_INVENTORY should exist");
    ASSERT(ct_add_component(table, id, C_TARGET), "add C_TARGET failed");
    ASSERT(ct_has_component(table, id, C_TARGET), "C_TARGET should exist");
    ASSERT(ct_add_component(table, id, C_TARGET_PATH), "add C_TARGET_PATH failed");
    ASSERT(ct_has_component(table, id, C_TARGET_PATH), "C_TARGET_PATH should exist");
    ASSERT(ct_add_component(table, id, C_DEFAULT_ACTION), "add C_DEFAULT_ACTION failed");
    ASSERT(ct_has_component(table, id, C_DEFAULT_ACTION), "C_DEFAULT_ACTION should exist");
    ASSERT(ct_add_component(table, id, C_EQUIPMENT), "add C_EQUIPMENT failed");
    ASSERT(ct_has_component(table, id, C_EQUIPMENT), "C_EQUIPMENT should exist");
    ASSERT(ct_add_component(table, id, C_STATS), "add C_STATS failed");
    ASSERT(ct_has_component(table, id, C_STATS), "C_STATS should exist");
    ASSERT(ct_add_component(table, id, C_ITEMTYPE), "add C_ITEMTYPE failed");
    ASSERT(ct_has_component(table, id, C_ITEMTYPE), "C_ITEMTYPE should exist");
    ASSERT(ct_add_component(table, id, C_WEAPONTYPE), "add C_WEAPONTYPE failed");
    ASSERT(ct_has_component(table, id, C_WEAPONTYPE), "C_WEAPONTYPE should exist");
    ASSERT(ct_add_component(table, id, C_SHIELDTYPE), "add C_SHIELDTYPE failed");
    ASSERT(ct_has_component(table, id, C_SHIELDTYPE), "C_SHIELDTYPE should exist");
    ASSERT(ct_add_component(table, id, C_POTIONTYPE), "add C_POTIONTYPE failed");
    ASSERT(ct_has_component(table, id, C_POTIONTYPE), "C_POTIONTYPE should exist");
    ASSERT(ct_add_component(table, id, C_DAMAGE), "add C_DAMAGE failed");
    ASSERT(ct_has_component(table, id, C_DAMAGE), "C_DAMAGE should exist");
    ASSERT(ct_add_component(table, id, C_AC), "add C_AC failed");
    ASSERT(ct_has_component(table, id, C_AC), "C_AC should exist");
    ct_destroy(table);
}

TEST(test_multiple_entities) {
    ct* table = ct_create();
    ASSERT(table != NULL, "ct creation failed");
    // Test with multiple entities having different components
    entityid id1 = 0;
    entityid id2 = 1;
    ASSERT(ct_add_entity(table, id1), "add entity id1 failed");
    ASSERT(ct_add_entity(table, id2), "add entity id2 failed");
    ASSERT(ct_add_component(table, id1, C_NAME), "add C_NAME to id1 failed");
    ASSERT(ct_add_component(table, id1, C_TYPE), "add C_TYPE to id1 failed");
    ASSERT(ct_add_component(table, id2, C_RACE), "add C_RACE to id2 failed");
    ASSERT(ct_add_component(table, id2, C_LOCATION), "add C_LOCATION to id2 failed");
    ASSERT(ct_has_component(table, id1, C_NAME), "C_NAME should exist for id1");
    ASSERT(ct_has_component(table, id1, C_TYPE), "C_TYPE should exist for id1");
    ASSERT(!ct_has_component(table, id1, C_RACE), "C_RACE should not exist for id1");
    ASSERT(ct_has_component(table, id2, C_RACE), "C_RACE should exist for id2");
    ASSERT(ct_has_component(table, id2, C_LOCATION), "C_LOCATION should exist for id2");
    ASSERT(!ct_has_component(table, id2, C_NAME), "C_NAME should not exist for id2");
    ct_destroy(table);
}

void test_component_table(void) {
    run_create_ct_0();
    run_create_ct_1();
    run_test_ct_has_entity();
    run_test_ct_add_component();
    run_test_ct_has_component();
    run_test_boundary_conditions();
    run_test_all_component_types();
    run_test_multiple_entities();
}
