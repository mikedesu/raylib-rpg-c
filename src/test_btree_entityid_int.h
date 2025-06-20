
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

TEST(test_btree_entityid_int_node_contains) {
    // Create a small tree
    btree_entityid_int_node* root = btree_entityid_int_node_create(50, 100);
    root = btree_entityid_int_node_insert(root, 30, 200);
    root = btree_entityid_int_node_insert(root, 70, 300);
    root = btree_entityid_int_node_insert(root, 20, 400);
    root = btree_entityid_int_node_insert(root, 40, 500);
    root = btree_entityid_int_node_insert(root, 60, 600);
    root = btree_entityid_int_node_insert(root, 80, 700);

    // Test existing keys
    ASSERT(btree_entityid_int_node_contains(root, 50), "root key should exist");
    ASSERT(btree_entityid_int_node_contains(root, 30), "left child key should exist");
    ASSERT(btree_entityid_int_node_contains(root, 70), "right child key should exist");
    ASSERT(btree_entityid_int_node_contains(root, 20), "leftmost key should exist");
    ASSERT(btree_entityid_int_node_contains(root, 80), "rightmost key should exist");

    // Test non-existing keys
    ASSERT(!btree_entityid_int_node_contains(root, 10), "key 10 should not exist");
    ASSERT(!btree_entityid_int_node_contains(root, 90), "key 90 should not exist");
    ASSERT(!btree_entityid_int_node_contains(root, 55), "key 55 should not exist");

    // Clean up
    btree_entityid_int_node_destroy(root);
}

TEST(test_btree_entityid_int_node_remove) {
    // Create a small tree
    btree_entityid_int_node* root = btree_entityid_int_node_create(50, 100);
    root = btree_entityid_int_node_insert(root, 30, 200);
    root = btree_entityid_int_node_insert(root, 70, 300);
    root = btree_entityid_int_node_insert(root, 20, 400);
    root = btree_entityid_int_node_insert(root, 40, 500);
    root = btree_entityid_int_node_insert(root, 60, 600);
    root = btree_entityid_int_node_insert(root, 80, 700);

    // Test removing a leaf node
    root = btree_entityid_int_node_remove(root, 20);
    ASSERT(!btree_entityid_int_node_contains(root, 20), "key 20 should be removed");
    ASSERT(btree_entityid_int_node_contains(root, 30), "parent node should still exist");

    // Test removing a node with one child
    root = btree_entityid_int_node_remove(root, 30);
    ASSERT(!btree_entityid_int_node_contains(root, 30), "key 30 should be removed");
    ASSERT(btree_entityid_int_node_contains(root, 40), "child node should still exist");

    // Test removing a node with two children
    root = btree_entityid_int_node_remove(root, 50);
    ASSERT(!btree_entityid_int_node_contains(root, 50), "key 50 should be removed");
    ASSERT(btree_entityid_int_node_contains(root, 60), "right subtree should still exist");
    ASSERT(btree_entityid_int_node_contains(root, 40), "left subtree should still exist");

    // Test removing non-existent key
    btree_entityid_int_node* original_root = root;
    root = btree_entityid_int_node_remove(root, 99);
    ASSERT(root == original_root, "tree should remain unchanged when removing non-existent key");

    // Clean up
    btree_entityid_int_node_destroy(root);
}

TEST(test_btree_entityid_int_node_get) {
    // Create a small tree
    btree_entityid_int_node* root = btree_entityid_int_node_create(50, 100);
    root = btree_entityid_int_node_insert(root, 30, 200);
    root = btree_entityid_int_node_insert(root, 70, 300);
    root = btree_entityid_int_node_insert(root, 20, 400);
    root = btree_entityid_int_node_insert(root, 40, 500);
    root = btree_entityid_int_node_insert(root, 60, 600);
    root = btree_entityid_int_node_insert(root, 80, 700);

    // Test getting existing nodes
    btree_entityid_int_node* node = btree_entityid_int_node_get(root, 50);
    ASSERT(node != NULL, "root node should exist");
    ASSERT(node->key == 50, "root node key should match");
    ASSERT(node->value == 100, "root node value should match");

    node = btree_entityid_int_node_get(root, 20);
    ASSERT(node != NULL, "leftmost node should exist");
    ASSERT(node->key == 20, "leftmost node key should match");
    ASSERT(node->value == 400, "leftmost node value should match");

    node = btree_entityid_int_node_get(root, 80);
    ASSERT(node != NULL, "rightmost node should exist");
    ASSERT(node->key == 80, "rightmost node key should match");
    ASSERT(node->value == 700, "rightmost node value should match");

    // Test getting non-existent nodes
    node = btree_entityid_int_node_get(root, 10);
    ASSERT(node == NULL, "non-existent key 10 should return NULL");

    node = btree_entityid_int_node_get(root, 90);
    ASSERT(node == NULL, "non-existent key 90 should return NULL");

    node = btree_entityid_int_node_get(root, 55);
    ASSERT(node == NULL, "non-existent key 55 should return NULL");

    // Clean up
    btree_entityid_int_node_destroy(root);
}

void test_btree_entityid_int(void) {
    run_test_btree_entityid_int_node_create_destroy();
    run_test_btree_entityid_int_node_insert();
    run_test_btree_entityid_int_node_contains();
    run_test_btree_entityid_int_node_remove();
    run_test_btree_entityid_int_node_get();
}
