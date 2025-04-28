#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    int x, y, z; // location coords
    int priority; // cost or cost+heuristic
} loc_node_t;

// Swaps two loc_node_t in an array
static inline void loc_swap(loc_node_t* a, loc_node_t* b) {
    loc_node_t t = *a;
    *a = *b;
    *b = t;
}

// Returns true if a's priority is less than b's (min-heap)
static inline bool loc_less(loc_node_t a, loc_node_t b) { return a.priority < b.priority; }

// Heapify up from idx, given heap size and array
static void loc_heapify_up(loc_node_t* heap, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (!loc_less(heap[idx], heap[parent])) break;
        loc_swap(&heap[idx], &heap[parent]);
        idx = parent;
    }
}

// Heapify down from idx, given heap size and array
static void loc_heapify_down(loc_node_t* heap, size_t size, size_t idx) {
    while (true) {
        size_t left = idx * 2 + 1;
        size_t right = idx * 2 + 2;
        size_t smallest = idx;
        if (left < size && loc_less(heap[left], heap[smallest])) smallest = left;
        if (right < size && loc_less(heap[right], heap[smallest])) smallest = right;
        if (smallest == idx) break;
        loc_swap(&heap[idx], &heap[smallest]);
        idx = smallest;
    }
}
