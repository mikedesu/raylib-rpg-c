#pragma once
#include "entityid.h"
#include <stdbool.h>

// forward declaration
typedef struct btree_int_t btree_int;

typedef struct btree_int_t {
    entityid id; // entity ID
    int value; // integer value associated with the entity ID
    btree_int* left; // pointer to left child node
    btree_int* right; // pointer to right child node
    btree_int* parent; // pointer to parent node
} btree_int;

// Function to create a new btree_int node
btree_int* btree_int_new(entityid id, int value);
// Function to free a btree_int node
void btree_int_free(btree_int* node);
// Function to insert a new node into the binary tree
btree_int* btree_int_insert(btree_int* root, entityid id, int value);
// Function to find a node by entity ID
btree_int* btree_int_find(btree_int* root, entityid id);
// Function to remove a node by entity ID
bool btree_int_remove(btree_int** root, entityid id);
// Function to get the value associated with an entity ID
int btree_int_get_value(btree_int* root, entityid id);
// Function to set the value associated with an entity ID
bool btree_int_set_value(btree_int* root, entityid id, int value);
// Function to check if a node with the given entity ID exists
bool btree_int_exists(btree_int* root, entityid id);
// Function to get the minimum node in the binary tree
btree_int* btree_int_min(btree_int* root);
// Function to get the maximum node in the binary tree
btree_int* btree_int_max(btree_int* root);
// Function to free the entire binary tree
void btree_int_free_tree(btree_int* root);
// Function to print the binary tree (for debugging purposes)
void btree_int_print(btree_int* root, int level);
// Functions to rebalance the binary tree
void btree_int_rebalance(btree_int** root);
void btree_int_rotate_left(btree_int** root);
void btree_int_rotate_right(btree_int** root);
// Function to get the height of the binary tree
int btree_int_height(btree_int* root);
// Function to get the balance factor of a node
int btree_int_balance_factor(btree_int* node);
// Function to check if the binary tree is balanced
bool btree_int_is_balanced(btree_int* root);
