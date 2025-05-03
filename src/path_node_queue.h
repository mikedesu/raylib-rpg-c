#pragma once

#include "location.h"
#include "path_node.h"
#include <stdlib.h>
#include <string.h>

// A simple priority queue for path nodes
typedef struct {
    path_node* nodes;
    int count;
    int capacity;
} node_queue;

static inline int compare_nodes(const void* a, const void* b);

static inline int compare_nodes(const void* a, const void* b) {
    path_node* node_a = (path_node*)a;
    path_node* node_b = (path_node*)b;

    // Compare by f_cost first
    if (node_a->f_cost != node_b->f_cost) return node_a->f_cost - node_b->f_cost;

    // If f_costs are equal, prefer the node with lower h_cost
    return node_a->h_cost - node_b->h_cost;
}

// Initialize a new queue
static node_queue create_node_queue(int initial_capacity) {
    node_queue queue;
    queue.nodes = (path_node*)malloc(initial_capacity * sizeof(path_node));
    queue.count = 0;
    queue.capacity = initial_capacity;
    return queue;
}

// Add a node to the queue and maintain priority order
static void queue_add(node_queue* queue, path_node node) {
    // Resize if needed
    if (queue->count >= queue->capacity) {
        queue->capacity *= 2;
        queue->nodes = (path_node*)realloc(queue->nodes, queue->capacity * sizeof(path_node));
    }

    // Add the node
    queue->nodes[queue->count] = node;
    queue->count++;

    // Sort the queue using our comparison function
    qsort(queue->nodes, queue->count, sizeof(path_node), compare_nodes);
}

// Remove and return the highest priority node (lowest f_cost)
static path_node queue_pop(node_queue* queue) {
    path_node node = queue->nodes[0];
    queue->count--;

    // Shift remaining nodes
    if (queue->count > 0) { memmove(&queue->nodes[0], &queue->nodes[1], queue->count * sizeof(path_node)); }

    return node;
}

// Check if a position is already in the queue
static bool queue_contains_position(node_queue* queue, loc_t pos) {
    for (int i = 0; i < queue->count; i++) {
        if (queue->nodes[i].pos.x == pos.x && queue->nodes[i].pos.y == pos.y) { return true; }
    }
    return false;
}

// Find a node with a specific position
static int queue_find_node_index(node_queue* queue, loc_t pos) {
    for (int i = 0; i < queue->count; i++) {
        if (queue->nodes[i].pos.x == pos.x && queue->nodes[i].pos.y == pos.y) { return i; }
    }
    return -1;
}

// Free queue resources
static inline void queue_destroy(node_queue* queue) {
    free(queue->nodes);
    queue->nodes = NULL;
    queue->count = 0;
    queue->capacity = 0;
}
