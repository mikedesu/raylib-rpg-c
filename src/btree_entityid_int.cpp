#include "btree_entityid_int.h"
#include <stdlib.h>

btree_entityid_int_node* btree_entityid_int_node_create(entityid key, int value) {
    btree_entityid_int_node* node = (btree_entityid_int_node*)malloc(sizeof(btree_entityid_int_node));
    if (!node) {
        return NULL; // Memory allocation failed
    }
    node->key = key;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

btree_entityid_int_node* btree_entityid_int_node_insert(btree_entityid_int_node* node, entityid key, int value) {
    if (node == NULL) {
        return btree_entityid_int_node_create(key, value);
    }

    if (key < node->key) {
        node->left = btree_entityid_int_node_insert(node->left, key, value);
    } else if (key > node->key) {
        node->right = btree_entityid_int_node_insert(node->right, key, value);
    } else {
        // Key already exists, update the value
        node->value = value;
    }

    return node; // Return the unchanged node pointer
}

btree_entityid_int_node* btree_entityid_int_node_remove(btree_entityid_int_node* node, entityid key) {
    if (node == NULL) {
        return NULL; // Key not found
    }

    if (key < node->key) {
        node->left = btree_entityid_int_node_remove(node->left, key);
    } else if (key > node->key) {
        node->right = btree_entityid_int_node_remove(node->right, key);
    } else {
        // Node with the key found
        if (node->left == NULL) {
            btree_entityid_int_node* temp = node->right;
            free(node);
            return temp; // Return the right child
        } else if (node->right == NULL) {
            btree_entityid_int_node* temp = node->left;
            free(node);
            return temp; // Return the left child
        }

        // Node with two children: Get the inorder successor (smallest in the right subtree)
        btree_entityid_int_node* temp = node->right;
        while (temp && temp->left != NULL) {
            temp = temp->left;
        }

        // Copy the inorder successor's content to this node
        node->key = temp->key;
        node->value = temp->value;

        // Delete the inorder successor
        node->right = btree_entityid_int_node_remove(node->right, temp->key);
    }

    return node; // Return the unchanged node pointer
}

btree_entityid_int_node* btree_entityid_int_node_get(btree_entityid_int_node* node, entityid key) {
    if (node == NULL || node->key == key) {
        return node; // Key found or reached a leaf
    }

    if (key < node->key) {
        return btree_entityid_int_node_get(node->left, key); // Search in the left subtree
    } else {
        return btree_entityid_int_node_get(node->right, key); // Search in the right subtree
    }
}

int btree_entityid_int_node_contains(const btree_entityid_int_node* node, entityid key) {
    if (node == NULL) {
        return 0; // Key not found
    }

    if (node->key == key) {
        return 1; // Key found
    } else if (key < node->key) {
        return btree_entityid_int_node_contains(node->left, key); // Search in the left subtree
    } else {
        return btree_entityid_int_node_contains(node->right, key); // Search in the right subtree
    }
}

void btree_entityid_int_node_destroy(btree_entityid_int_node* node) {
    if (node == NULL) {
        return; // Nothing to destroy
    }

    btree_entityid_int_node_destroy(node->left); // Recursively destroy left subtree
    btree_entityid_int_node_destroy(node->right); // Recursively destroy right subtree
    free(node); // Free the current node
}

void btree_entityid_int_node_rotate_left(btree_entityid_int_node** node) {
    if (node == NULL || *node == NULL) {
        return; // Nothing to rotate
    }

    btree_entityid_int_node* right_child = (*node)->right;
    if (right_child == NULL) {
        return; // No right child to rotate
    }

    (*node)->right = right_child->left; // Move the left child of the right child to the current node's right
    right_child->left = *node; // Make the current node the left child of the right child
    *node = right_child; // Update the current node to be the new root
}

void btree_entityid_int_node_rotate_right(btree_entityid_int_node** node) {
    if (node == NULL || *node == NULL) {
        return; // Nothing to rotate
    }

    btree_entityid_int_node* left_child = (*node)->left;
    if (left_child == NULL) {
        return; // No left child to rotate
    }

    (*node)->left = left_child->right; // Move the right child of the left child to the current node's left
    left_child->right = *node; // Make the current node the right child of the left child
    *node = left_child; // Update the current node to be the new root
}

int btree_entityid_int_node_height(const btree_entityid_int_node* node) {
    if (node == NULL) {
        return 0; // Height of an empty node is 0
    }
    int left_height = btree_entityid_int_node_height(node->left);
    int right_height = btree_entityid_int_node_height(node->right);
    return (left_height > right_height ? left_height : right_height) + 1; // Return the height of the node
}

btree_entityid_int* btree_entityid_int_create(void) {
    btree_entityid_int* tree = (btree_entityid_int*)malloc(sizeof(btree_entityid_int));
    if (!tree) {
        return NULL; // Memory allocation failed
    }
    tree->root = NULL;
    tree->height = 0;
    tree->count = 0;
    return tree;
}

int btree_entityid_int_get(const btree_entityid_int* tree, entityid key) {
    if (tree == NULL || tree->root == NULL) {
        return -1; // Tree is empty or not initialized
    }
    btree_entityid_int_node* node = btree_entityid_int_node_get(tree->root, key);
    return (node != NULL) ? node->value : -1; // Return value or -1 if not found
}

int btree_entityid_int_insert(btree_entityid_int* tree, entityid key, int value) {
    if (tree == NULL) {
        return -1; // Tree is not initialized
    }

    tree->root = btree_entityid_int_node_insert(tree->root, key, value);
    if (tree->root == NULL) {
        return -1; // Memory allocation failed
    }

    tree->count++;
    // Update height if necessary (not implemented here)
    // tree->height = ...;

    return 0; // Success
}

int btree_entityid_int_remove(btree_entityid_int* tree, entityid key) {
    if (tree == NULL || tree->root == NULL) {
        return -1; // Tree is empty or not initialized
    }

    tree->root = btree_entityid_int_node_remove(tree->root, key);
    if (tree->root == NULL) {
        return -1; // Key not found or memory allocation failed
    }

    tree->count--;
    // Update height if necessary (not implemented here)
    tree->height = btree_entityid_int_node_height(tree->root);

    return 0; // Success
}

void btree_entityid_int_destroy(btree_entityid_int* tree) {
    if (tree == NULL) {
        return; // Nothing to destroy
    }

    btree_entityid_int_node_destroy(tree->root); // Recursively destroy the tree
    free(tree); // Free the tree structure
    tree = NULL; // Avoid dangling pointer
}

void btree_entityid_int_clear(btree_entityid_int* tree) {
    if (tree == NULL) {
        return; // Nothing to clear
    }

    btree_entityid_int_node_destroy(tree->root); // Recursively destroy the tree
    tree->root = NULL; // Set root to NULL
    tree->count = 0; // Reset count
    tree->height = 0; // Reset height
}

//void btree_entityid_int_print(const btree_entityid_int* tree) {
//    if (tree == NULL || tree->root == NULL) {
//        printf("Tree is empty.\n");
//        return; // Nothing to print
//    }
//
//    // A simple in-order traversal to print the keys and values
//    void print_node(const btree_entityid_int_node* node) {
//        if (node == NULL) {
//            return; // Base case
//        }
//        print_node(node->left); // Visit left subtree
//        printf("Key: %d, Value: %d\n", node->key, node->value); // Print current node
//        print_node(node->right); // Visit right subtree
//    }
//
//    print_node(tree->root); // Start printing from the root
//}

size_t btree_entityid_int_size(const btree_entityid_int* tree) {
    if (tree == NULL) {
        return 0; // Tree is not initialized
    }
    return tree->count; // Return the number of nodes in the tree
}

size_t btree_entityid_int_height(const btree_entityid_int* tree) {
    if (tree == NULL) {
        return 0; // Tree is not initialized
    }
    return tree->height; // Return the height of the tree
}
