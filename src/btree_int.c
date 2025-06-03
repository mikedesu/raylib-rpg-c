#include "btree_int.h"
#include <stdio.h>
#include <stdlib.h>

btree_int* btree_int_new(entityid id, int value) {
    btree_int* node = (btree_int*)malloc(sizeof(btree_int));
    if (node) {
        node->id = id;
        node->value = value;
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;
    }
    return node;
}

void btree_int_free(btree_int* node) {
    if (node) {
        free(node);
    }
}

btree_int* btree_int_insert(btree_int* root, entityid id, int value) {
    if (root == NULL) {
        return btree_int_new(id, value);
    }
    if (id < root->id) {
        root->left = btree_int_insert(root->left, id, value);
        root->left->parent = root;
    } else if (id > root->id) {
        root->right = btree_int_insert(root->right, id, value);
        root->right->parent = root;
    } else {
        // If the id already exists, update the value
        root->value = value;
    }
    return root;
}

btree_int* btree_int_find(btree_int* root, entityid id) {
    if (root == NULL || root->id == id) {
        return root;
    }
    if (id < root->id) {
        return btree_int_find(root->left, id);
    } else {
        return btree_int_find(root->right, id);
    }
}

bool btree_int_remove(btree_int** root, entityid id) {
    if (*root == NULL) {
        return false; // Node not found
    }

    if (id < (*root)->id) {
        return btree_int_remove(&(*root)->left, id);
    } else if (id > (*root)->id) {
        return btree_int_remove(&(*root)->right, id);
    } else {
        // Node found
        btree_int* node_to_remove = *root;

        // Case 1: No child
        if (node_to_remove->left == NULL && node_to_remove->right == NULL) {
            *root = NULL;
        }
        // Case 2: One child
        else if (node_to_remove->left == NULL || node_to_remove->right == NULL) {
            *root = (node_to_remove->left != NULL) ? node_to_remove->left : node_to_remove->right;
            (*root)->parent = node_to_remove->parent;
        }
        // Case 3: Two children
        else {
            // Find the in-order successor (smallest in the right subtree)
            btree_int* successor = node_to_remove->right;
            while (successor->left != NULL) {
                successor = successor->left;
            }
            // Replace the value and id of the node to remove with the successor's
            node_to_remove->id = successor->id;
            node_to_remove->value = successor->value;
            // Remove the successor
            btree_int_remove(&node_to_remove->right, successor->id);
        }

        btree_int_free(node_to_remove);
        return true;
    }
}

int btree_int_get_value(btree_int* root, entityid id) {
    btree_int* node = btree_int_find(root, id);
    if (node) {
        return node->value;
    }
    return 0; // Return 0 or some default value if the id is not found
}

bool btree_int_set_value(btree_int* root, entityid id, int value) {
    btree_int* node = btree_int_find(root, id);
    if (node) {
        node->value = value;
        return true; // Value updated successfully
    }
    return false; // Node with the given id not found
}

bool btree_int_exists(btree_int* root, entityid id) {
    return btree_int_find(root, id) != NULL; // Returns true if the node exists, false otherwise
}

btree_int* btree_int_min(btree_int* root) {
    if (root == NULL) {
        return NULL; // Tree is empty
    }
    while (root->left != NULL) {
        root = root->left; // Traverse to the leftmost node
    }
    return root; // Return the leftmost node, which is the minimum
}
btree_int* btree_int_max(btree_int* root) {
    if (root == NULL) {
        return NULL; // Tree is empty
    }
    while (root->right != NULL) {
        root = root->right; // Traverse to the rightmost node
    }
    return root; // Return the rightmost node, which is the maximum
}

void btree_int_free_tree(btree_int* root) {
    if (root) {
        btree_int_free_tree(root->left);
        btree_int_free_tree(root->right);
        btree_int_free(root);
    }
}

void btree_int_print(btree_int* root, int level) {
    if (root == NULL) {
        return; // Base case: if the node is NULL, return
    }
    btree_int_print(root->right, level + 1); // Print right subtree first
    for (int i = 0; i < level; i++) {
        printf("   "); // Indentation for levels
    }
    printf("%d (%d)\n", root->id, root->value); // Print the current node's id and value
    btree_int_print(root->left, level + 1); // Print left subtree
}

void btree_int_rebalance(btree_int** root) {
    if (*root == NULL) {
        return; // Nothing to rebalance if the tree is empty
    }

    int balance = btree_int_balance_factor(*root);

    // Left heavy
    if (balance > 1) {
        if (btree_int_balance_factor((*root)->left) < 0) {
            btree_int_rotate_left(&(*root)->left); // Left-Right case
        }
        btree_int_rotate_right(root); // Left-Left case
    }
    // Right heavy
    else if (balance < -1) {
        if (btree_int_balance_factor((*root)->right) > 0) {
            btree_int_rotate_right(&(*root)->right); // Right-Left case
        }
        btree_int_rotate_left(root); // Right-Right case
    }
}

void btree_int_rotate_left(btree_int** root) {
    if (*root == NULL || (*root)->right == NULL) {
        return; // Cannot rotate left if root is NULL or right child is NULL
    }

    btree_int* new_root = (*root)->right; // New root will be the right child
    (*root)->right = new_root->left; // Move the left child of new root to the right of current root
    if (new_root->left != NULL) {
        new_root->left->parent = *root; // Update parent of the left child
    }

    new_root->left = *root; // Set current root as the left child of new root
    new_root->parent = (*root)->parent; // Update parent of new root
    (*root)->parent = new_root; // Update parent of current root to new root

    *root = new_root; // Update the root pointer to the new root
}

void btree_int_rotate_right(btree_int** root) {
    if (*root == NULL || (*root)->left == NULL) {
        return; // Cannot rotate right if root is NULL or left child is NULL
    }

    btree_int* new_root = (*root)->left; // New root will be the left child
    (*root)->left = new_root->right; // Move the right child of new root to the left of current root
    if (new_root->right != NULL) {
        new_root->right->parent = *root; // Update parent of the right child
    }

    new_root->right = *root; // Set current root as the right child of new root
    new_root->parent = (*root)->parent; // Update parent of new root
    (*root)->parent = new_root; // Update parent of current root to new root

    *root = new_root; // Update the root pointer to the new root
}
// Function to get the height of the binary tree
int btree_int_height(btree_int* root) {
    if (root == NULL) {
        return 0; // Height of an empty tree is 0
    }
    int left_height = btree_int_height(root->left);
    int right_height = btree_int_height(root->right);
    return (left_height > right_height ? left_height : right_height) + 1; // Height is max of left and right subtree heights plus 1 for the current node
}

// Function to get the balance factor of a node
int btree_int_balance_factor(btree_int* node) {
    if (node == NULL) {
        return 0; // Balance factor of an empty node is 0
    }
    int left_height = btree_int_height(node->left);
    int right_height = btree_int_height(node->right);
    return left_height - right_height; // Balance factor is the height of left subtree minus height of right subtree
}
// Function to check if the binary tree is balanced
bool btree_int_is_balanced(btree_int* root) {
    if (root == NULL) {
        return true; // An empty tree is balanced
    }
    int balance = btree_int_balance_factor(root);
    // Check if the current node is balanced and recursively check left and right subtrees
    return (balance >= -1 && balance <= 1) && btree_int_is_balanced(root->left) && btree_int_is_balanced(root->right);
}
