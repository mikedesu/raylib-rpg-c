#pragma once

#include "dungeon_floor.h"
#include "vec3.h"
#include <math.h>
#include <stdbool.h>

typedef struct path_node_t {
    vec3 pos; // Position
    int g_cost; // Cost from start to this node
    int h_cost; // Heuristic cost (estimated cost to destination)
    int f_cost; // Total cost (g_cost + h_cost)
    struct path_node_t* parent; // Parent node for reconstructing path
} path_node;

path_node* create_path_node(vec3 position, path_node* parent, vec3 target);
bool is_position_walkable(vec3 pos, dungeon_floor_t* df);
vec3* find_path(vec3 start, vec3 end, dungeon_floor_t* df, int* path_length);

void get_neighbors(vec3 pos, vec3 neighbors[4]);

// Calculate Manhattan distance (heuristic for A*)
static inline int calculate_heuristic(vec3 a, vec3 b) { return abs(a.x - b.x) + abs(a.y - b.y); }

static inline int compare_nodes(const void* a, const void* b) {
    path_node* node_a = *(path_node**)a;
    path_node* node_b = *(path_node**)b;

    if (node_a->f_cost != node_b->f_cost) {
        return node_a->f_cost - node_b->f_cost; // Sort by f_cost first
    }

    // If f_costs are equal, prioritize lower h_cost (closer to target)
    return node_a->h_cost - node_b->h_cost;
}

static inline bool locations_equal(vec3 a, vec3 b) { return a.x == b.x && a.y == b.y; }
