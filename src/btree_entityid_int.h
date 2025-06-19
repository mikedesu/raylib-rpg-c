#pragma once

#include "entityid.h"
#include <stddef.h>

typedef struct btree_entityid_int_node_t btree_entityid_int_node_t;
typedef struct btree_entityid_int_node_t {
    entityid key;
    int value;
    btree_entityid_int_node_t* left;
    btree_entityid_int_node_t* right;
} btree_entityid_int_node;

btree_entityid_int_node* btree_entityid_int_node_create(entityid key, int value);
btree_entityid_int_node* btree_entityid_int_node_insert(btree_entityid_int_node* node, entityid key, int value);
btree_entityid_int_node* btree_entityid_int_node_remove(btree_entityid_int_node* node, entityid key);
btree_entityid_int_node* btree_entityid_int_node_get(btree_entityid_int_node* node, entityid key);
int btree_entityid_int_node_contains(const btree_entityid_int_node* node, entityid key);
void btree_entityid_int_node_destroy(btree_entityid_int_node* node);
void btree_entityid_int_node_rotate_left(btree_entityid_int_node** node);
void btree_entityid_int_node_rotate_right(btree_entityid_int_node** node);
int btree_entityid_int_node_height(const btree_entityid_int_node* node);

typedef struct btree_entityid_int_t {
    btree_entityid_int_node* root;
    size_t height;
    size_t count;
} btree_entityid_int;

btree_entityid_int* btree_entityid_int_create(void);
int btree_entityid_int_get(const btree_entityid_int* tree, entityid key);
int btree_entityid_int_insert(btree_entityid_int* tree, entityid key, int value);
int btree_entityid_int_remove(btree_entityid_int* tree, entityid key);
void btree_entityid_int_destroy(btree_entityid_int* tree);
void btree_entityid_int_clear(btree_entityid_int* tree);
//void btree_entityid_int_print(const btree_entityid_int* tree);
size_t btree_entityid_int_size(const btree_entityid_int* tree);
size_t btree_entityid_int_height(const btree_entityid_int* tree);
