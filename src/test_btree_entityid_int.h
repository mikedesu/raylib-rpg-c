
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

TEST(test_btree_entityid_int_node_insert) {
    // Create root node
    entityid root_key = 50;
    int root_value = 100;
    btree_entityid_int_node* root = btree_entityid_int_node_create(root_key, root_value);
    ASSERT(root != NULL, "root node creation failed");
    
    // Insert left child
    entityid left_key = 30;
    int left_value = 200;
    root = btree_entityid_int_node_insert(root, left_key, left_value);
    ASSERT(root->left != NULL, "left child should exist");
    ASSERT(root->left->key == left_key, "left child key should match");
    ASSERT(root->left->value == left_value, "left child value should match");
    
    // Insert right child
    entityid right_key = 70;
    int right_value = 300;
    root = btree_entityid_int_node_insert(root, right_key, right_value);
    ASSERT(root->right != NULL, "right child should exist");
    ASSERT(root->right->key == right_key, "right child key should match");
    ASSERT(root->right->value == right_value, "right child value should match");
    
    // Insert duplicate key (should update value)
    int updated_value = 400;
    root = btree_entityid_int_node_insert(root, root_key, updated_value);
    ASSERT(root->value == updated_value, "root value should be updated");
    
    // Clean up
    btree_entityid_int_node_destroy(root);
}

void test_btree_entityid_int(void) {
    run_test_btree_entityid_int_node_create_destroy();
    run_test_btree_entityid_int_node_insert();
    // run_test_btree_entityid_int_node_contains();
}
