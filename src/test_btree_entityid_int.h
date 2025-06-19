
#pragma once
#include "btree_entityid_int.h"
#include "unit_test.h"

TEST(test_btree_entityid_int_node_create_destroy) {
    entityid key = 1;
    int value = 42;
    btree_entityid_int_node* node = btree_entityid_int_node_create(key, value);
    ASSERT(node != NULL, "node creation failed");
    ASSERT(node->key == key, "node key should match");
    ASSERT(node->value == value, "node value should match");
    ASSERT(node->left == NULL, "node left child should be NULL");
    ASSERT(node->right == NULL, "node right child should be NULL");
    btree_entityid_int_node_destroy(node);
}

void test_btree_entityid_int(void) {
    run_test_btree_entityid_int_node_create_destroy();
    // run_test_btree_entityid_int_node_contains();
}
