#pragma once

#include "location.h"
#include <math.h>

typedef struct path_node_t {
    loc_t pos; // Position
    int g_cost; // Cost from start to this node
    int h_cost; // Heuristic cost (estimated cost to destination)
    int f_cost; // Total cost (g_cost + h_cost)
    struct path_node_t* parent; // Parent node for reconstructing path
} path_node;

// Calculate Manhattan distance (heuristic for A*)
static inline int calculate_heuristic(loc_t a, loc_t b) { return abs(a.x - b.x) + abs(a.y - b.y); }
