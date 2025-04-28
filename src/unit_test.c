#include "location_priority_queue.h"
#include "unit_test.h"

TEST(test_push_pop) {
    loc_pq_t* pq = loc_pq_new(4);
    ASSERT(pq != NULL, "pq creation failed");
    loc_node_t n1 = {0, 0, 0, 5};
    loc_node_t n2 = {1, 1, 0, 3};
    loc_node_t n3 = {2, 2, 0, 4};

    ASSERT(loc_pq_push(pq, n1), "push n1 failed");
    ASSERT(loc_pq_push(pq, n2), "push n2 failed");
    ASSERT(loc_pq_push(pq, n3), "push n3 failed");
    ASSERT(pq->size == 3, "pq size incorrect");

    loc_node_t out;
    ASSERT(loc_pq_pop(pq, &out), "pop failed");
    ASSERT(out.x == 1 && out.y == 1 && out.priority == 3, "pop order incorrect");

    ASSERT(loc_pq_pop(pq, &out), "pop failed");
    ASSERT(out.priority == 4, "pop order incorrect");

    ASSERT(loc_pq_pop(pq, &out), "pop failed");
    ASSERT(out.priority == 5, "pop order incorrect");

    ASSERT(loc_pq_is_empty(pq), "pq should be empty after pops");
    loc_pq_free(pq);
}

int main(void) {
    test_push_pop();
    unit_test_summary();
    return 0;
}
