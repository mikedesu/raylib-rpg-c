#pragma once

#include "btree_int.h"
#include "mprint.h"
#include "unit_test.h"

TEST(test_btree_int_0) {
    minfo("Running test_btree_int_0...");
    btree_int* tree = NULL;
    // Insert nodes
    tree = btree_int_insert(tree, 1, 10);
    tree = btree_int_insert(tree, 2, 20);
    tree = btree_int_insert(tree, 3, 30);
    tree = btree_int_insert(tree, 4, 40);
    tree = btree_int_insert(tree, 5, 50);
    // Check if nodes exist
    ASSERT(btree_int_exists(tree, 1), "Node with ID 1 should exist");
    ASSERT(btree_int_exists(tree, 2), "Node with ID 2 should exist");
    ASSERT(btree_int_exists(tree, 3), "Node with ID 3 should exist");
    ASSERT(btree_int_exists(tree, 4), "Node with ID 4 should exist");
    ASSERT(btree_int_exists(tree, 5), "Node with ID 5 should exist");
    // Get values
    ASSERT(btree_int_get_value(tree, 1) == 10, "Value for ID 1 should be 10");
    ASSERT(btree_int_get_value(tree, 2) == 20, "Value for ID 2 should be 20");
    ASSERT(btree_int_get_value(tree, 3) == 30, "Value for ID 3 should be 30");
    ASSERT(btree_int_get_value(tree, 4) == 40, "Value for ID 4 should be 40");
    ASSERT(btree_int_get_value(tree, 5) == 50, "Value for ID 5 should be 50");
    // Update value
    btree_int_set_value(tree, 3, 35);
    ASSERT(btree_int_get_value(tree, 3) == 35, "Updated value for ID 3 should be 35");
    // Remove nodes
    ASSERT(btree_int_remove(&tree, 2), "Removing node with ID 2 should succeed");
    ASSERT(!btree_int_exists(tree, 2), "Node with ID 2 should not exist after removal");
    // Free the tree
    btree_int_free_tree(tree);
    msuccess("test_btree_int_0 completed successfully.");
}

void test_btree_int_tests(void) {
    test_btree_int_0();
    // Add more tests as needed
}
