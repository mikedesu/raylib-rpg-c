//#include "location_priority_queue.h"
//#include "unit_test.h"
//TEST(test_push_pop) {
//    loc_pq_t* pq = loc_pq_new(4);
//    ASSERT(pq != NULL, "pq creation failed");
//    loc_node_t n1 = {0, 0, 0, 5};
//    loc_node_t n2 = {1, 1, 0, 3};
//    loc_node_t n3 = {2, 2, 0, 4};
//    ASSERT(loc_pq_push(pq, n1), "push n1 failed");
//    ASSERT(loc_pq_push(pq, n2), "push n2 failed");
//    ASSERT(loc_pq_push(pq, n3), "push n3 failed");
//    ASSERT(pq->size == 3, "pq size incorrect");
//    loc_node_t out;
//    ASSERT(loc_pq_pop(pq, &out), "pop failed");
//    ASSERT(out.x == 1 && out.y == 1 && out.priority == 3, "pop order incorrect");
//    ASSERT(loc_pq_pop(pq, &out), "pop failed");
//    ASSERT(out.priority == 4, "pop order incorrect");
//    ASSERT(loc_pq_pop(pq, &out), "pop failed");
//    ASSERT(out.priority == 5, "pop order incorrect");
//    ASSERT(loc_pq_is_empty(pq), "pq should be empty after pops");
//    loc_pq_free(pq);
//}

//int main(void) {
//    run_test_push_pop();
//    unit_test_summary();
//    return 0;
//}

//#include "location_priority_queue.h"
#include "component_table.h"
#include "unit_test.h"

TEST(create_ct_0) {
    ct* table = ct_create();
    ASSERT(table != NULL, "ct creation failed");
    ASSERT(table->component_row_count == 0, "row count should be 0");
    ASSERT(table->component_col_count == C_COUNT, "col count should be COMP_COUNT");
    ASSERT(table->component_capacity == CT_DEFAULT_CAPACITY, "capacity should be CT_DEFAULT_CAPACITY");
    ct_destroy(table);
}

TEST(create_ct_1) {
    ct* table = ct_create();
    ASSERT(table != NULL, "ct creation failed");
    ASSERT(table->component_row_count == 0, "row count should be 0");
    ASSERT(table->component_col_count == C_COUNT, "col count should be COMP_COUNT");
    ASSERT(table->component_capacity == CT_DEFAULT_CAPACITY, "capacity should be CT_DEFAULT_CAPACITY");

    entityid id = 0;

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

//TEST(test_push_pop_0) {
//    loc_pq_t* pq = loc_pq_new(4);
//    ASSERT(pq != NULL, "pq creation failed");
//    loc_pq_free(pq);
//}

//TEST(test_push_pop_1) {
//    loc_pq_t* pq = loc_pq_new(4);
//    ASSERT(pq != NULL, "pq creation failed");
//    ASSERT(loc_pq_push(pq, (loc_node_t){0, 0, 0, 5}), "push n1 failed");
//    ASSERT(loc_pq_push(pq, (loc_node_t){1, 1, 0, 3}), "push n2 failed");
//    ASSERT(loc_pq_push(pq, (loc_node_t){2, 2, 0, 4}), "push n3 failed");
//    ASSERT(pq->size == 3, "pq size incorrect");
//    loc_pq_free(pq);
//}

//TEST(test_push_pop_2) {
//    loc_pq_t* pq = loc_pq_new(4);
//    ASSERT(pq != NULL, "pq creation failed");
//    loc_pq_push(pq, (loc_node_t){0, 0, 0, 5});
//    loc_pq_push(pq, (loc_node_t){1, 1, 0, 3});
//    loc_pq_push(pq, (loc_node_t){2, 2, 0, 4});
//    loc_node_t out;
//    ASSERT(loc_pq_pop(pq, &out), "pop failed");
//    ASSERT(out.x == 1 && out.y == 1 && out.priority == 3, "pop order incorrect");
//    ASSERT(loc_pq_pop(pq, &out), "pop failed");
//    ASSERT(out.priority == 4, "pop order incorrect");
//    ASSERT(loc_pq_pop(pq, &out), "pop failed");
//    ASSERT(out.priority == 5, "pop order incorrect");
//    ASSERT(loc_pq_is_empty(pq), "pq should be empty after pops");
//    loc_pq_free(pq);
//}

//TEST(test_push_pop_fail_0) {
//    loc_pq_t* pq = loc_pq_new(2);
//    ASSERT(pq != NULL, "pq creation failed");
//    ASSERT(loc_pq_push(pq, (loc_node_t){0, 0, 0, 1}), "push first node failed");
//    ASSERT(loc_pq_push(pq, (loc_node_t){1, 0, 0, 2}), "push second node failed");
//    // This push should fail due to capacity limit
//    ASSERT(!loc_pq_push(pq, (loc_node_t){2, 0, 0, 3}), "push should have failed but didn't");
//    loc_pq_free(pq);
//}

//TEST(test_push_pop_fail_0) {
//    loc_pq_t* pq = loc_pq_new(2);
//    ASSERT(pq != NULL, "pq creation failed");
//    ASSERT(loc_pq_push(pq, (loc_node_t){0, 0, 0, 1}), "push first node failed");
//    ASSERT(loc_pq_push(pq, (loc_node_t){1, 0, 0, 2}), "push second node failed");
//    bool pushed = loc_pq_push(pq, (loc_node_t){2, 0, 0, 3});
//    ASSERT(pushed == false, "expected push to fail due to capacity");
//    loc_pq_free(pq);
//}

//TEST(test_push_pop_fail_0) {
//    loc_pq_t* pq = loc_pq_new(2);
//    ASSERT(pq != NULL, "pq creation failed");
//    ASSERT(loc_pq_push(pq, (loc_node_t){0, 0, 0, 1}), "push first node failed");
//    ASSERT(loc_pq_push(pq, (loc_node_t){1, 0, 0, 2}), "push second node failed");
//    bool pushed = loc_pq_push(pq, (loc_node_t){2, 0, 0, 3});
//    ASSERT(pushed == true, "expected push to succeed but it should fail");
//    loc_pq_free(pq);
//}

int main(void) {
    //run_test_push_pop_0();
    //run_test_push_pop_1();
    //run_test_push_pop_2();
    //run_test_push_pop_fail_0();

    run_create_ct_0();
    run_create_ct_1();

    unit_test_summary();
    return 0;
}
