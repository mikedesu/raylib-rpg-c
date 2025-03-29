#include "dungeon_tile.h"
#include "em.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

void tile_init(tile_t* const t, tiletype_t type) {
    if (!t) {
        merror("dungeon_tile_init: tile is NULL");
        return;
    }
    t->type = type;
    t->visible = t->explored = false;
    const size_t malloc_sz = sizeof(entityid) * DUNGEON_TILE_MAX_ENTITIES_DEFAULT;
    t->entities = malloc(malloc_sz);
    if (!t->entities) {
        merror("dungeon_tile_init: failed to allocate entities array");
        t->type = TILE_NONE; // Reset to safe state
        return;
    }
    memset(t->entities, -1, malloc_sz);
    t->entity_count = 0;
    t->entity_max = DUNGEON_TILE_MAX_ENTITIES_DEFAULT;

    t->has_pressure_plate = t->has_wall_switch = t->wall_switch_on = false;
    t->pressure_plate_up_tx_key = t->pressure_plate_down_tx_key = t->pressure_plate_event = -1;
    t->wall_switch_up_tx_key = t->wall_switch_down_tx_key = t->wall_switch_event = -1;

    t->cached_live_npcs = 0;
    t->cached_player_present = false;
    t->dirty_entities = true; // Start dirty
    t->dirty_visibility = true;
}

void tile_set_pressure_plate(tile_t* const t, bool b) {
    if (!t) return;
    t->has_pressure_plate = b;
}

void tile_set_pressure_plate_up_tx_key(tile_t* const t, int k) {
    if (!t) return;
    t->pressure_plate_up_tx_key = k;
}

void tile_set_pressure_plate_down_tx_key(tile_t* const t, int k) {
    if (!t) return;
    t->pressure_plate_down_tx_key = k;
}

void tile_set_pressure_plate_event(tile_t* const t, int e) {
    if (!t) return;
    t->pressure_plate_event = e;
}

bool tile_has_pressure_plate(const tile_t* const t) { return !t ? false : t->has_pressure_plate; }

int tile_get_pressure_plate_event(const tile_t* const t) { return !t ? -1 : t->pressure_plate_event; }

bool tile_resize(tile_t* t) {
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

entityid tile_add(tile_t* const t, entityid id) {
    if (!t || !t->entities) {
        merror("dungeon_tile_add: Invalid tile or entity array");
        return ENTITYID_INVALID;
    }

    // Early exit if tile is full and resize fails
    if (t->entity_count >= t->entity_max) {
        mwarning("dungeon_tile_add: Tile full, attempting resize");
        if (!tile_resize(t)) {
            merror("dungeon_tile_add: Resize failed");
            return ENTITYID_INVALID;
        }
    }

    // Find first empty slot
    for (size_t i = 0; i < t->entity_max; i++) {
        if (t->entities[i] == ENTITYID_INVALID) {
            t->entities[i] = id;
            t->entity_count++;
            t->dirty_entities = true;
            return id;
        }
    }

    //return id;

    // This should never happen due to earlier check
    merror("dungeon_tile_add: No empty slot found after resize");
    return ENTITYID_INVALID;
}

entityid tile_remove(tile_t* tile, entityid id) {
    bool did_remove = false;
    for (int i = 0; i < tile->entity_max; i++) {
        if (tile->entities[i] == id) {
            tile->entities[i] = -1;
            // shift all elements to the left with memcpy
            memcpy(&tile->entities[i], &tile->entities[i + 1], sizeof(entityid) * (tile->entity_max - i - 1));
            tile->entities[tile->entity_max - 1] = -1;
            tile->entity_count--;
            tile->dirty_entities = true;
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

tile_t* tile_create(tiletype_t type) {
    tile_t* t = malloc(sizeof(tile_t));
    if (!t) return NULL;
    tile_init(t, type);
    return t;
}

void tile_free(tile_t* t) {
    if (!t) return;
    free(t->entities);
    free(t);
}

size_t tile_entity_count(const tile_t* const t) { return !t ? 0 : t->entity_count; }

entityid tile_get_entity(const tile_t* const t, size_t i) { return !t ? -1 : i < t->entity_max ? t->entities[i] : -1; }

void tile_set_wall_switch(tile_t* const t, bool b) {
    if (!t) return;
    t->has_wall_switch = b;
}

void tile_set_wall_switch_up_tx_key(tile_t* t, int key) {
    if (!t) return;
    t->wall_switch_up_tx_key = key;
}

void tile_set_wall_switch_down_tx_key(tile_t* const t, int key) {
    if (!t) return;
    t->wall_switch_down_tx_key = key;
}

void tile_set_wall_switch_event(tile_t* const t, int event) {
    if (!t) return;
    t->wall_switch_event = event;
}

bool tile_has_wall_switch(const tile_t* const t) { return !t ? false : t->has_wall_switch; }

int tile_get_wall_switch_up_tx_key(const tile_t* const t) { return !t ? -1 : t->wall_switch_up_tx_key; }

int tile_get_wall_switch_down_tx_key(const tile_t* const t) { return !t ? -1 : t->wall_switch_down_tx_key; }

int tile_get_wall_switch_event(const tile_t* const t) { return !t ? -1 : t->wall_switch_event; }

void tile_set_wall_switch_on(tile_t* const t, bool on) {
    if (!t) return;
    t->wall_switch_on = on;
}

bool tile_is_wall_switch_on(const tile_t* const t) { return !t ? false : t->wall_switch_on; }

static void recompute_entity_cache(tile_t* t, em_t* em) {
    if (!t->dirty_entities) return;

    t->cached_live_npcs = 0;
    t->cached_player_present = false;

    for (size_t i = 0; i < t->entity_max; i++) {
        entity* e = em_get(em, t->entities[i]);
        if (!e || e->is_dead) continue;

        if (e->type == ENTITY_NPC) t->cached_live_npcs++;
        if (e->type == ENTITY_PLAYER) t->cached_player_present = true;
    }

    t->dirty_entities = false;
}

bool tile_has_live_npcs(tile_t* t, em_t* em) {
    if (!t) return false;
    recompute_entity_cache(t, em);
    return t->cached_live_npcs > 0;
}

bool tile_has_player(tile_t* t, em_t* em) {
    if (!t) return false;
    recompute_entity_cache(t, em);
    return t->cached_player_present;
}
