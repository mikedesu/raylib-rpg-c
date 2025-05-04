#pragma once

#include "dungeon_floor.h"
#include "location.h"
#include <math.h>
#include <stdbool.h>

typedef struct path_node_t {
    loc_t pos; // Position
    int g_cost; // Cost from start to this node
    int h_cost; // Heuristic cost (estimated cost to destination)
    int f_cost; // Total cost (g_cost + h_cost)
    struct path_node_t* parent; // Parent node for reconstructing path
} path_node;

path_node* create_path_node(loc_t position, path_node* parent, loc_t target);
bool is_position_walkable(loc_t pos, dungeon_floor_t* df);
loc_t* find_path(loc_t start, loc_t end, dungeon_floor_t* df, int* path_length);

void get_neighbors(loc_t pos, loc_t neighbors[4]);

// Calculate Manhattan distance (heuristic for A*)
static inline int calculate_heuristic(loc_t a, loc_t b) { return abs(a.x - b.x) + abs(a.y - b.y); }

static inline int compare_nodes(const void* a, const void* b) {
    path_node* node_a = *(path_node**)a;
    path_node* node_b = *(path_node**)b;

    if (node_a->f_cost != node_b->f_cost) {
        return node_a->f_cost - node_b->f_cost; // Sort by f_cost first
    }

    // If f_costs are equal, prioritize lower h_cost (closer to target)
    return node_a->h_cost - node_b->h_cost;
}

static inline bool locations_equal(loc_t a, loc_t b) { return a.x == b.x && a.y == b.y; }
