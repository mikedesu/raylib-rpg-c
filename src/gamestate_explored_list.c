#include "gamestate_explored_list.h"

bool g_add_explored_list(gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the explored list component
    //return g_add_component(g, id, C_EXPLORED_LIST, (void*)&loc, sizeof(vec3_component), (void**)&g->explored_list, &g->explored_list_count, &g->explored_list_capacity);
    return g_add_component(g, id, C_EXPLORED_LIST, NULL, sizeof(vec3_list_component), (void**)&g->explored_list, &g->explored_list_count, &g->explored_list_capacity);
}

bool g_has_explored(const gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_EXPLORED_LIST);
}

bool g_set_explored(gamestate* const g, entityid id, vec3 loc) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->explored_list == NULL) return false;
    for (int i = 0; i < g->explored_list_count; i++) {
        if (g->explored_list[i].id == id) {
            // Check if the location already exists in the explored list
            for (int j = 0; j < g->explored_list[i].count; j++) {
                if (vec3_equal(g->explored_list[i].list[j], loc)) {
                    return true; // Location already exists, no need to add
                }
            }
            // If not found, add the new location to the explored list

            return true;
        }
    }
    return false;
}

// Check if a location is in the explored list for a given entity
bool g_is_explored(const gamestate* const g, entityid id, vec3 loc) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->explored_list == NULL) return false;
    // Iterate through the explored list for the entity
    vec3_list_component* explored = NULL;
    if (g_get_explored_for_entity(g, id, &explored)) {
        for (int i = 0; i < explored->count; i++) {
            if (vec3_equal(explored->list[i], loc)) {
                return true; // Location found in the explored list
            }
        }
    }
    return false; // Location not found in the explored list
}

//bool g_get_explored_for_entity(const gamestate* const g, entityid id, vec3** explored, int* count) {
bool g_get_explored_for_entity(const gamestate* const g, entityid id, vec3_list_component** explored) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->explored_list == NULL) return false;
    for (int i = 0; i < g->explored_list_count; i++) {
        if (g->explored_list[i].id == id) {
            *explored = &g->explored_list[i];
            // If you need to return the count, you can do so here
            return true; // Found the entity's explored list
        }
    }
    *explored = NULL; // Not found
    return false; // Entity not found in the explored list
}

bool g_add_explored(gamestate* const g, entityid id, vec3 loc) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the explored list component
    //if (!g_add_explored_list(g, id)) return false; // Ensure the explored list component exists

    vec3_list_component* explored = NULL;
    if (g_get_explored_for_entity(g, id, &explored)) {
        // Check if the location already exists in the explored list
        for (int i = 0; i < explored->count; i++) {
            if (vec3_equal(explored->list[i], loc)) {
                return true; // Location already exists, no need to add
            }
        }
        // If not found, add the new location to the explored list
        if (explored->count < explored->capacity) {
            explored->list[explored->count++] = loc; // Add new location
            return true;
        }
    }
    return false; // Failed to add location
}
