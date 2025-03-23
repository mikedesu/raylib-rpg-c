#include "dungeon_tile.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>


void dungeon_tile_init(dungeon_tile_t* tile, const dungeon_tile_type_t type) {
    if (!tile) {
        merror("dungeon_tile_init: tile is NULL");
        return;
    }

    tile->type = type;
    tile->visible = false;
    tile->explored = false;

    const size_t malloc_sz = sizeof(entityid) * DUNGEON_TILE_MAX_ENTITIES_DEFAULT;
    tile->entities = malloc(malloc_sz);
    if (!tile->entities) {
        merror("dungeon_tile_init: failed to allocate entities array");
        tile->type = DUNGEON_TILE_TYPE_NONE; // Reset to safe state
        return;
    }
    memset(tile->entities, -1, malloc_sz);
    tile->entity_count = 0;
    tile->entity_max = DUNGEON_TILE_MAX_ENTITIES_DEFAULT;

    tile->has_pressure_plate = false;
    tile->pressure_plate_up_tx_key = -1;
    tile->pressure_plate_down_tx_key = -1;
    tile->pressure_plate_event = -1;

    tile->has_wall_switch = false;
    tile->wall_switch_on = false;
    tile->wall_switch_up_tx_key = -1;
    tile->wall_switch_down_tx_key = -1;
    tile->wall_switch_event = -1;
}


void dungeon_tile_set_pressure_plate(dungeon_tile_t* const t, bool b) {
    if (!t) return;
    t->has_pressure_plate = b;
}


void dungeon_tile_set_pressure_plate_up_tx_key(dungeon_tile_t* const t, int k) {
    if (!t) return;
    t->pressure_plate_up_tx_key = k;
}

void dungeon_tile_set_pressure_plate_down_tx_key(dungeon_tile_t* const t, int k) {
    if (!t) return;
    t->pressure_plate_down_tx_key = k;
}


void dungeon_tile_set_pressure_plate_event(dungeon_tile_t* const t, int e) {
    if (!t) return;
    t->pressure_plate_event = e;
}


const bool dungeon_tile_has_pressure_plate(const dungeon_tile_t* tile) {
    if (!tile) {
        merror("dungeon_tile_has_pressure_plate: tile is NULL");
        return false;
    }
    return tile->has_pressure_plate;
}


const int dungeon_tile_get_pressure_plate_event(const dungeon_tile_t* tile) {
    if (!tile) {
        merror("dungeon_tile_get_pressure_plate_event: tile is NULL");
        return -1;
    }
    return tile->pressure_plate_event;
}


//const bool dungeon_tile_resize(dungeon_tile_t* tile) {
//    if (!tile) {
//        merror("dungeon_tile_resize: tile is NULL");
//        return false;
//    }
//    size_t new_max = tile->entity_max * 2;
//    size_t malloc_sz = sizeof(entityid) * new_max;
//    if (malloc_sz > DUNGEON_TILE_MAX_ENTITIES_MAX) {
//        merror("dungeon_tile_resize: malloc_sz > DUNGEON_TILE_MAX_ENTITIES_MAX");
//        return false;
//    }
//    // instead of reallocing, do a malloc instead, a memset, and then copy
//    entityid* new_entities = (entityid*)malloc(malloc_sz);
//    if (new_entities == NULL) {
//        merror("dungeon_tile_resize: new_entities malloc failed");
//        return false;
//    }
//    memset(new_entities, -1, malloc_sz);
//    memcpy(new_entities, tile->entities, sizeof(entityid) * tile->entity_max);
//    free(tile->entities);
//    tile->entities = new_entities;
//    tile->entity_max = new_max;
//    return true;
//}


bool dungeon_tile_resize(dungeon_tile_t* t) {
    if (!t || !t->entities) {
        merror("dungeon_tile_resize: tile or entities is NULL");
        return false;
    }
    size_t new_max = t->entity_max * 2;
    if (new_max > DUNGEON_TILE_MAX_ENTITIES_MAX) {
        merror("dungeon_tile_resize: new_max exceeds max entities");
        return false;
    }
    size_t sz = sizeof(entityid) * new_max;
    entityid* new_e = malloc(sz);
    if (!new_e) {
        merror("dungeon_tile_resize: malloc failed");
        return false;
    }
    memset(new_e, -1, sz);
    memcpy(new_e, t->entities, sizeof(entityid) * t->entity_max);
    free(t->entities);
    t->entities = new_e;
    t->entity_max = new_max;
    return true;
}


//const entityid dungeon_tile_add(dungeon_tile_t* const t, const entityid id) {
//    minfoint("dungeon_tile_add: adding entity", id);
//    int index = -1;
//    for (int i = 0; i < DUNGEON_TILE_MAX_ENTITIES_DEFAULT; i++) {
//        if (t->entities[i] == -1) {
//            index = i;
//            break;
//        }
//    }
//    if (index != -1) {
//        t->entities[index] = id;
//        t->entity_count++;
//    } else {
//        merror("dungeon_tile_add: no space for entity, resizing...");
//        dungeon_tile_resize(t);
//        return dungeon_tile_add(t, id);
//    }
//    return index;
//}


//entityid dungeon_tile_add(dungeon_tile_t* const t, entityid id) {
//    if (!t || !t->entities) {
//        merror("dungeon_tile_add: tile or entities is NULL");
//        return -1;
//    }
//    minfoint("Adding entity", id);
//
//    for (size_t i = 0; i < t->entity_max; i++) {
//        if (t->entities[i] == -1) {
//            t->entities[i] = id;
//            t->entity_count++;
//            return id; // Return ID, not index
//        }
//    }
//
//    mwarning("No space for entity, resizing...");
//    if (!dungeon_tile_resize(t)) { // Assume resize returns success
//        merror("Resize failed");
//        return -1;
//    }
//    return dungeon_tile_add(t, id); // Recurse
//}


entityid dungeon_tile_add(dungeon_tile_t* const t, entityid id) {
    if (!t || !t->entities) return -1;
    minfoint("Adding entity", id);

    for (size_t i = 0; i < t->entity_max; i++) { // Use entity_max
        if (t->entities[i] == -1) {
            t->entities[i] = id;
            t->entity_count++;
            return id;
        }
    }

    mwarning("No space, resizing...");
    if (!dungeon_tile_resize(t)) return -1;
    return dungeon_tile_add(t, id);
}


const entityid dungeon_tile_remove(dungeon_tile_t* tile, const entityid id) {
    bool did_remove = false;
    for (int i = 0; i < tile->entity_max; i++) {
        if (tile->entities[i] == id) {
            tile->entities[i] = -1;
            // shift all elements to the left with memcpy
            memcpy(&tile->entities[i], &tile->entities[i + 1], sizeof(entityid) * (tile->entity_max - i - 1));
            tile->entities[tile->entity_max - 1] = -1;
            tile->entity_count--;
            did_remove = true;
            break;
        }
    }
    if (!did_remove) {
        merror("dungeon_tile_remove: entity not found");
        return -1;
    }
    return id;
}


dungeon_tile_t* dungeon_tile_create(const dungeon_tile_type_t type) {
    dungeon_tile_t* t = malloc(sizeof(dungeon_tile_t));
    if (!t) {
        merror("dungeon_tile_create: failed to allocate memory for dungeon_tile_t");
        return NULL;
    }
    dungeon_tile_init(t, type);
    return t;
}


void dungeon_tile_free(dungeon_tile_t* t) {
    if (!t) return;
    free(t->entities);
    free(t);
}


const size_t dungeon_tile_entity_count(const dungeon_tile_t* t) {
    if (!t) return 0;
    return t->entity_count;
}


const entityid dungeon_tile_get_entity(const dungeon_tile_t* tile, const size_t index) {
    if (!tile) {
        merror("dungeon_tile_get_entity: tile is NULL");
        return -1;
    }
    if (index < tile->entity_max) {
        return tile->entities[index];
    }
    return -1;
}


void dungeon_tile_set_wall_switch(dungeon_tile_t* tile, const bool has_wall_switch) {
    if (!tile) {
        merror("dungeon_tile_set_wall_switch: tile is NULL");
        return;
    }
    tile->has_wall_switch = has_wall_switch;
}


void dungeon_tile_set_wall_switch_up_tx_key(dungeon_tile_t* tile, const int key) {
    if (!tile) {
        merror("dungeon_tile_set_wall_switch_up_tx_key: tile is NULL");
        return;
    }
    tile->wall_switch_up_tx_key = key;
}


void dungeon_tile_set_wall_switch_down_tx_key(dungeon_tile_t* tile, const int key) {
    if (!tile) {
        merror("dungeon_tile_set_wall_switch_down_tx_key: tile is NULL");
        return;
    }
    tile->wall_switch_down_tx_key = key;
}


void dungeon_tile_set_wall_switch_event(dungeon_tile_t* tile, const int event) {
    if (!tile) {
        merror("dungeon_tile_set_wall_switch_event: tile is NULL");
        return;
    }
    tile->wall_switch_event = event;
}


const bool dungeon_tile_has_wall_switch(const dungeon_tile_t* tile) {
    if (!tile) {
        merror("dungeon_tile_has_wall_switch: tile is NULL");
        return false;
    }
    return tile->has_wall_switch;
}


const int dungeon_tile_get_wall_switch_up_tx_key(const dungeon_tile_t* tile) {
    if (!tile) {
        merror("dungeon_tile_get_wall_switch_up_tx_key: tile is NULL");
        return -1;
    }
    return tile->wall_switch_up_tx_key;
}


const int dungeon_tile_get_wall_switch_down_tx_key(const dungeon_tile_t* tile) {
    if (!tile) {
        merror("dungeon_tile_get_wall_switch_down_tx_key: tile is NULL");
        return -1;
    }
    return tile->wall_switch_down_tx_key;
}


const int dungeon_tile_get_wall_switch_event(const dungeon_tile_t* tile) {
    if (!tile) {
        merror("dungeon_tile_get_wall_switch_event: tile is NULL");
        return -1;
    }
    return tile->wall_switch_event;
}


void dungeon_tile_set_wall_switch_on(dungeon_tile_t* tile, const bool on) {
    if (!tile) {
        merror("dungeon_tile_set_wall_switch_on: tile is NULL");
        return;
    }
    tile->wall_switch_on = on;
}


const bool dungeon_tile_is_wall_switch_on(const dungeon_tile_t* tile) {
    if (!tile) {
        merror("dungeon_tile_is_wall_switch_on: tile is NULL");
        return false;
    }
    return tile->wall_switch_on;
}
