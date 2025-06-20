#include "path_node.h"
#include "vec3.h"
#include <stdlib.h>

path_node* create_path_node(vec3 position, path_node* parent, vec3 target) {
    path_node* node = malloc(sizeof(path_node));
    if (!node) return NULL;

    node->pos = position;
    node->parent = parent;

    // Calculate costs
    if (parent) {
        node->g_cost = parent->g_cost + 1; // Assume cost of 1 to move to adjacent cell
    } else {
        node->g_cost = 0; // Start node
    }

    node->h_cost = calculate_heuristic(position, target);
    node->f_cost = node->g_cost + node->h_cost;

    return node;
}

bool location_in_list(path_node** list, int list_size, vec3 pos) {
    for (int i = 0; i < list_size; i++) {
        if (locations_equal(list[i]->pos, pos)) {
            return true;
        }
    }
    return false;
}

void get_neighbors(vec3 pos, vec3 neighbors[8]) {
    neighbors[0] = (vec3){pos.x, pos.y - 1, pos.z}; // Up
    neighbors[1] = (vec3){pos.x + 1, pos.y - 1, pos.z}; // Up-Right
    neighbors[2] = (vec3){pos.x + 1, pos.y, pos.z}; // Right
    neighbors[3] = (vec3){pos.x + 1, pos.y + 1, pos.z}; // Down-Right
    neighbors[4] = (vec3){pos.x, pos.y + 1, pos.z}; // Down
    neighbors[5] = (vec3){pos.x - 1, pos.y + 1, pos.z}; // Down-Left
    neighbors[6] = (vec3){pos.x - 1, pos.y, pos.z}; // Left
    neighbors[7] = (vec3){pos.x - 1, pos.y - 1, pos.z}; // Up-Left
}

//void get_neighbors(vec3 pos, vec3 neighbors[4]) {
//    neighbors[0] = (vec3){pos.x, pos.y - 1, pos.z}; // Up
//    neighbors[1] = (vec3){pos.x + 1, pos.y, pos.z}; // Right
//    neighbors[2] = (vec3){pos.x, pos.y + 1, pos.z}; // Down
//    neighbors[3] = (vec3){pos.x - 1, pos.y, pos.z}; // Left
//}

bool is_position_walkable(vec3 pos, dungeon_floor_t* df) {
    // Check bounds
    if (pos.x < 0 || pos.x >= df->width || pos.y < 0 || pos.y >= df->height) {
        return false;
    }

    // Get the tile at the position and check if walkable
    tile_t* t = df_tile_at(df, pos);
    return tile_is_walkable(t->type);
}

vec3* find_path(vec3 start, vec3 end, dungeon_floor_t* df, int* path_length) {
    *path_length = 0;

    // If start and end are the same
    if (locations_equal(start, end)) {
        vec3* path = malloc(sizeof(vec3));
        if (!path) return NULL;

        path[0] = start;
        *path_length = 1;
        return path;
    }

    // Initialize open and closed lists
    path_node** open_list = NULL;
    int open_count = 0;
    int open_capacity = 0;

    path_node** closed_list = NULL;
    int closed_count = 0;
    int closed_capacity = 0;

    // Add start node to open list
    path_node* start_node = create_path_node(start, NULL, end);
    if (!start_node) return NULL;

    // Add to open list
    if (open_count == open_capacity) {
        open_capacity = open_capacity == 0 ? 8 : open_capacity * 2;
        open_list = realloc(open_list, open_capacity * sizeof(path_node*));
        if (!open_list) {
            free(start_node);
            return NULL;
        }
    }
    open_list[open_count++] = start_node;

    // Main loop
    while (open_count > 0) {
        // Sort open list by f_cost (lowest first)
        qsort(open_list, open_count, sizeof(path_node*), compare_nodes);

        // Get node with lowest f_cost
        path_node* current = open_list[0];

        // Remove current from open list
        open_list[0] = open_list[--open_count];

        // Add current to closed list
        if (closed_count == closed_capacity) {
            closed_capacity = closed_capacity == 0 ? 8 : closed_capacity * 2;
            closed_list = realloc(closed_list, closed_capacity * sizeof(path_node*));
            if (!closed_list) {
                // Clean up
                for (int i = 0; i < open_count; i++) free(open_list[i]);
                free(open_list);
                free(current);
                return NULL;
            }
        }
        closed_list[closed_count++] = current;

        // Check if we've reached the destination
        if (locations_equal(current->pos, end)) {
            // Reconstruct path
            int count = 0;
            path_node* node = current;
            while (node) {
                count++;
                node = node->parent;
            }

            vec3* path = malloc(count * sizeof(vec3));
            if (!path) {
                // Clean up
                for (int i = 0; i < open_count; i++) free(open_list[i]);
                for (int i = 0; i < closed_count; i++) free(closed_list[i]);
                free(open_list);
                free(closed_list);
                return NULL;
            }

            node = current;
            int i = 0;
            while (node) {
                path[i++] = node->pos;
                node = node->parent;
            }

            *path_length = count;

            // Clean up
            for (int i = 0; i < open_count; i++) free(open_list[i]);
            for (int i = 0; i < closed_count; i++) free(closed_list[i]);
            free(open_list);
            free(closed_list);

            return path;
        }

        // Check neighbors
        vec3 neighbors[8];
        get_neighbors(current->pos, neighbors);

        for (int i = 0; i < 8; i++) {
            vec3 neighbor_pos = neighbors[i];

            // Check if neighbor is walkable
            if (!is_position_walkable(neighbor_pos, df)) {
                continue;
            }

            // Check if neighbor is in closed list
            bool in_closed = false;
            for (int j = 0; j < closed_count; j++) {
                if (locations_equal(closed_list[j]->pos, neighbor_pos)) {
                    in_closed = true;
                    break;
                }
            }
            if (in_closed) continue;

            // Calculate g_cost for this neighbor
            int tentative_g_cost = current->g_cost + 1; // Assuming cost of 1 to move to adjacent cell

            // Check if neighbor is in open list
            path_node* neighbor_node = NULL;
            bool in_open = false;
            for (int j = 0; j < open_count; j++) {
                if (locations_equal(open_list[j]->pos, neighbor_pos)) {
                    neighbor_node = open_list[j];
                    in_open = true;
                    break;
                }
            }

            if (in_open) {
                // If we've found a better path to this neighbor
                if (tentative_g_cost < neighbor_node->g_cost) {
                    neighbor_node->g_cost = tentative_g_cost;
                    neighbor_node->f_cost = neighbor_node->g_cost + neighbor_node->h_cost;
                    neighbor_node->parent = current;
                }
            } else {
                // Add neighbor to open list
                path_node* new_neighbor = create_path_node(neighbor_pos, current, end);
                if (!new_neighbor) {
                    // Clean up
                    for (int i = 0; i < open_count; i++) free(open_list[i]);
                    for (int i = 0; i < closed_count; i++) free(closed_list[i]);
                    free(open_list);
                    free(closed_list);
                    return NULL;
                }

                if (open_count == open_capacity) {
                    open_capacity = open_capacity * 2;
                    path_node** new_open_list = realloc(open_list, open_capacity * sizeof(path_node*));
                    if (!new_open_list) {
                        // Clean up
                        free(new_neighbor);
                        for (int i = 0; i < open_count; i++) free(open_list[i]);
                        for (int i = 0; i < closed_count; i++) free(closed_list[i]);
                        free(open_list);
                        free(closed_list);
                        return NULL;
                    }
                    open_list = new_open_list;
                }
                open_list[open_count++] = new_neighbor;
            }
        }
    }

    // No path found
    // Clean up
    for (int i = 0; i < open_count; i++) free(open_list[i]);
    for (int i = 0; i < closed_count; i++) free(closed_list[i]);
    free(open_list);
    free(closed_list);

    return NULL;
}
