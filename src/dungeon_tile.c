#include "dungeon_tile.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

void dungeon_tile_init(dungeon_tile_t* const t, dungeon_tile_type_t type) {
    if (!t) {
        merror("dungeon_tile_init: tile is NULL");
        return;
    }

    t->type = type;
    t->visible = false;
    t->explored = false;
    const size_t malloc_sz = sizeof(entityid) * DUNGEON_TILE_MAX_ENTITIES_DEFAULT;
    t->entities = malloc(malloc_sz);
    if (!t->entities) {
        merror("dungeon_tile_init: failed to allocate entities array");
        t->type = DUNGEON_TILE_TYPE_NONE; // Reset to safe state
        return;
    }
    memset(t->entities, -1, malloc_sz);
    t->entity_count = 0;
    t->entity_max = DUNGEON_TILE_MAX_ENTITIES_DEFAULT;

    t->has_pressure_plate = false;
    t->pressure_plate_up_tx_key = -1;
    t->pressure_plate_down_tx_key = -1;
    t->pressure_plate_event = -1;

    t->has_wall_switch = false;
    t->wall_switch_on = false;
    t->wall_switch_up_tx_key = -1;
    t->wall_switch_down_tx_key = -1;
    t->wall_switch_event = -1;
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

bool dungeon_tile_has_pressure_plate(const dungeon_tile_t* const t) { return !t ? false : t->has_pressure_plate; }

int dungeon_tile_get_pressure_plate_event(const dungeon_tile_t* const t) { return !t ? -1 : t->pressure_plate_event; }

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

entityid dungeon_tile_add(dungeon_tile_t* const t, entityid id) {
    if (!t || !t->entities) {
        merror("dungeon_tile_add: Invalid tile or entity array");
        return ENTITYID_INVALID;
    }

    // Early exit if tile is full and resize fails
    if (t->entity_count >= t->entity_max) {
        mwarning("dungeon_tile_add: Tile full, attempting resize");
        if (!dungeon_tile_resize(t)) {
            merror("dungeon_tile_add: Resize failed");
            return ENTITYID_INVALID;
        }
    }

    // Find first empty slot
    for (size_t i = 0; i < t->entity_max; i++) {
        if (t->entities[i] == ENTITYID_INVALID) {
            t->entities[i] = id;
            t->entity_count++;
            return id;
        }
    }

    // This should never happen due to earlier check
    merror("dungeon_tile_add: No empty slot found after resize");
    return ENTITYID_INVALID;
}

entityid dungeon_tile_remove(dungeon_tile_t* tile, entityid id) {
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

dungeon_tile_t* dungeon_tile_create(dungeon_tile_type_t type) {
    dungeon_tile_t* t = malloc(sizeof(dungeon_tile_t));
    if (!t) return NULL;
    dungeon_tile_init(t, type);
    return t;
}

void dungeon_tile_free(dungeon_tile_t* t) {
    if (!t) return;
    free(t->entities);
    free(t);
}

size_t dungeon_tile_entity_count(const dungeon_tile_t* const t) { return !t ? 0 : t->entity_count; }

entityid dungeon_tile_get_entity(const dungeon_tile_t* const t, size_t i) {
    return !t ? -1 : i < t->entity_max ? t->entities[i] : -1;
}

void dungeon_tile_set_wall_switch(dungeon_tile_t* const t, bool b) {
    if (!t) return;
    t->has_wall_switch = b;
}

void dungeon_tile_set_wall_switch_up_tx_key(dungeon_tile_t* t, int key) {
    if (!t) return;
    t->wall_switch_up_tx_key = key;
}

void dungeon_tile_set_wall_switch_down_tx_key(dungeon_tile_t* const t, int key) {
    if (!t) return;
    t->wall_switch_down_tx_key = key;
}

void dungeon_tile_set_wall_switch_event(dungeon_tile_t* const t, int event) {
    if (!t) return;
    t->wall_switch_event = event;
}

bool dungeon_tile_has_wall_switch(const dungeon_tile_t* const t) { return !t ? false : t->has_wall_switch; }

int dungeon_tile_get_wall_switch_up_tx_key(const dungeon_tile_t* const t) { return !t ? -1 : t->wall_switch_up_tx_key; }

int dungeon_tile_get_wall_switch_down_tx_key(const dungeon_tile_t* const t) {
    return !t ? -1 : t->wall_switch_down_tx_key;
}

int dungeon_tile_get_wall_switch_event(const dungeon_tile_t* const t) { return !t ? -1 : t->wall_switch_event; }

void dungeon_tile_set_wall_switch_on(dungeon_tile_t* const t, bool on) {
    if (!t) return;
    t->wall_switch_on = on;
}

bool dungeon_tile_is_wall_switch_on(const dungeon_tile_t* const t) { return !t ? false : t->wall_switch_on; }
