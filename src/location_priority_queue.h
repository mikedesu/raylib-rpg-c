#pragma once

#include "location_heap_node.h"
#include "mprint.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    loc_node_t* data;
    size_t size;
    size_t capacity;
} loc_pq_t;

static loc_pq_t* loc_pq_new(size_t capacity) {
    loc_pq_t* pq = (loc_pq_t*)malloc(sizeof(loc_pq_t));
    if (!pq) {
        merror("loc_pq_new: failed to allocate memory for loc_pq_t");
        return NULL;
    }
    pq->data = (loc_node_t*)malloc(sizeof(loc_node_t) * capacity);
    if (!pq->data) {
        merror("loc_pq_new: failed to allocate memory for loc_node_t array");
        free(pq);
        return NULL;
    }
    pq->size = 0;
    pq->capacity = capacity;
    msuccess("loc_pq_new: created priority queue with capacity %zu", capacity);
    return pq;
}

static void loc_pq_free(loc_pq_t* pq) {
    if (!pq) return;
    free(pq->data);
    free(pq);
}

static bool loc_pq_is_empty(loc_pq_t* pq) { return pq->size == 0; }

static bool loc_pq_push(loc_pq_t* pq, loc_node_t node) {
    if (pq->size == pq->capacity) {
        merror("loc_pq_push: priority queue is full");
        return false;
    }
    pq->data[pq->size] = node;
    loc_heapify_up(pq->data, pq->size);
    pq->size++;
    msuccess("loc_pq_push: pushed node (%d, %d, %d, %d)", node.x, node.y, node.z, node.priority);
    return true;
}

static bool loc_pq_pop(loc_pq_t* pq, loc_node_t* out) {
    if (pq->size == 0) {
        merror("loc_pq_pop: priority queue is empty");
        return false;
    }
    *out = pq->data[0];
    pq->data[0] = pq->data[pq->size - 1];
    pq->size--;
    loc_heapify_down(pq->data, pq->size, 0);
    return true;
}
