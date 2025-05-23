#include "dungeon_tile.h"
//#include "em.h"
#include "gamestate.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

void tile_init(tile_t* const t, tiletype_t type) {
    massert(t, "tile is NULL");
    t->type = type;
    t->visible = t->explored = t->has_pressure_plate = t->has_wall_switch = t->wall_switch_on = t->cached_player_present = false;
    t->dirty_entities = t->dirty_visibility = true;
    const size_t malloc_sz = sizeof(entityid) * DUNGEON_TILE_MAX_ENTITIES_DEFAULT;
    t->entities = malloc(malloc_sz);
    if (!t->entities) {
        merror("dungeon_tile_init: failed to allocate entities array");
        t->type = TILE_NONE; // Reset to safe state
        return;
    }
    memset(t->entities, -1, malloc_sz);
    t->entity_max = DUNGEON_TILE_MAX_ENTITIES_DEFAULT;

    t->pressure_plate_up_tx_key = t->pressure_plate_down_tx_key = t->pressure_plate_event = -1;
    t->wall_switch_up_tx_key = t->wall_switch_down_tx_key = t->wall_switch_event = -1;
    t->entity_count = t->cached_live_npcs = 0;
    // Start dirty
}

bool tile_resize(tile_t* t) {
    massert(t, "tile is NULL");
    massert(t->entities, "tile entities is NULL");
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
    massert(t, "tile is NULL");
    massert(t->entities, "tile entities is NULL");
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
    // This should never happen due to earlier check
    merror("dungeon_tile_add: No empty slot found after resize");
    return ENTITYID_INVALID;
}

entityid tile_remove(tile_t* tile, entityid id) {
    massert(tile, "tile is NULL");
    massert(tile->entities, "tile entities is NULL");
    massert(id != ENTITYID_INVALID, "tile_remove: id is invalid");
    bool did_remove = false;
    for (int i = 0; i < tile->entity_max; i++) {
        if (tile->entities[i] == id) {
            tile->entities[i] = -1;
            // shift all elements to the left with memcpy
            memcpy(&tile->entities[i], &tile->entities[i + 1], sizeof(entityid) * (tile->entity_max - i - 1));
            tile->entities[tile->entity_max - 1] = -1;
            tile->entity_count--;
            tile->dirty_entities = did_remove = true;
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
    massert(type >= TILE_NONE, "tile_create: type is invalid");
    massert(type < TILE_COUNT, "tile_create: type is out of bounds");
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

//void recompute_entity_cache(gamestate* g, tile_t* t, em_t* em) {
void recompute_entity_cache(gamestate* g, tile_t* t) {
    massert(g, "gamestate is NULL");
    massert(t, "tile is NULL");
    
    // Only recompute if cache is dirty
    if (!t->dirty_entities) return;
    
    // Reset counters
    t->cached_live_npcs = 0;
    t->cached_player_present = false;
    
    // Iterate through all entities on the tile
    for (size_t i = 0; i < t->entity_max; i++) {
        entityid id = t->entities[i];
        if (id == ENTITYID_INVALID) continue;
        
        // Skip dead entities
        if (g_is_dead(g, id)) continue;
        
        // Check entity type
        entitytype_t type = g_get_type(g, id);
        if (type == ENTITY_NPC) {
            t->cached_live_npcs++;
        } else if (type == ENTITY_PLAYER) {
            t->cached_player_present = true;
        }
    }
    
    // Cache is now clean
    t->dirty_entities = false;
}

void recompute_entity_cache_at(gamestate* g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    massert(x >= 0, "x is out of bounds");
    massert(y >= 0, "y is out of bounds");
    massert(z >= 0, "z is out of bounds");
    massert(z < g->d->num_floors, "z is out of bounds");
    dungeon_floor_t* const df = d_get_floor(g->d, z);
    massert(df, "failed to get dungeon floor");
    tile_t* const t = df_tile_at(df, (loc_t){x, y, z});
    if (!t) {
        merror("tile not found");
        return;
    }
    recompute_entity_cache(g, t);
}

size_t tile_live_npc_count_at(gamestate* g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    massert(x >= 0, "x is out of bounds");
    massert(y >= 0, "y is out of bounds");
    massert(z >= 0, "z is out of bounds");
    recompute_entity_cache_at(g, x, y, z);
    dungeon_t* d = g_get_dungeon(g);
    massert(d, "failed to get dungeon");
    dungeon_floor_t* df = d_get_floor(d, z);
    massert(df, "failed to get dungeon floor");
    massert(x < df->width, "x is out of bounds");
    massert(y < df->height, "y is out of bounds");
    tile_t* t = df_tile_at(df, (loc_t){x, y, z});
    massert(t, "failed to get tile");
    return tile_live_npc_count(g, t);
}

size_t tile_serialized_size(const tile_t* t) {
    massert(t, "tile is NULL");
    // Calculate total size needed:
    // tiletype_t + 8 bools + 9 ints + 2 size_t + (entity_max * sizeof(entityid))
    size_t size = sizeof(tiletype_t) 
                + (8 * sizeof(bool)) 
                + (9 * sizeof(int)) 
                + (2 * sizeof(size_t)) 
                + (t->entity_max * sizeof(entityid));
    return size;
}

size_t tile_serialize(const tile_t* t, char* buffer, size_t buffer_size) {
    massert(t, "tile is NULL");
    massert(buffer, "buffer is NULL");
    
    size_t required_size = tile_serialized_size(t);
    if (buffer_size < required_size) {
        merror("Buffer too small for serialization");
        return 0;
    }
    
    char* ptr = buffer;
    
    // Serialize basic fields
    memcpy(ptr, &t->type, sizeof(tiletype_t));
    ptr += sizeof(tiletype_t);
    
    memcpy(ptr, &t->visible, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(ptr, &t->explored, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(ptr, &t->has_pressure_plate, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(ptr, &t->has_wall_switch, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(ptr, &t->wall_switch_on, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(ptr, &t->cached_player_present, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(ptr, &t->dirty_entities, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(ptr, &t->dirty_visibility, sizeof(bool));
    ptr += sizeof(bool);
    
    // Serialize integer fields
    memcpy(ptr, &t->pressure_plate_up_tx_key, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, &t->pressure_plate_down_tx_key, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, &t->pressure_plate_event, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, &t->wall_switch_up_tx_key, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, &t->wall_switch_down_tx_key, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, &t->wall_switch_event, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, &t->cached_live_npcs, sizeof(int));
    ptr += sizeof(int);
    
    // Serialize entity data - count and max
    memcpy(ptr, &t->entity_count, sizeof(size_t));
    ptr += sizeof(size_t);
    memcpy(ptr, &t->entity_max, sizeof(size_t));
    ptr += sizeof(size_t);
    
    // Serialize entity array - write full size even if empty
    size_t entity_array_size = t->entity_max * sizeof(entityid);
    memset(ptr, 0xFF, entity_array_size); // Initialize to all -1 (empty)
    if (t->entity_count > 0) {
        memcpy(ptr, t->entities, entity_array_size);
    }
    ptr += entity_array_size;
    
    return ptr - buffer;
}

bool tile_deserialize(tile_t* t, const char* buffer, size_t buffer_size) {
    massert(t, "tile is NULL");
    massert(buffer, "buffer is NULL");
    
    const char* ptr = buffer;
    
    // Deserialize basic fields
    memcpy(&t->type, ptr, sizeof(tiletype_t));
    ptr += sizeof(tiletype_t);
    
    memcpy(&t->visible, ptr, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(&t->explored, ptr, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(&t->has_pressure_plate, ptr, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(&t->has_wall_switch, ptr, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(&t->wall_switch_on, ptr, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(&t->cached_player_present, ptr, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(&t->dirty_entities, ptr, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(&t->dirty_visibility, ptr, sizeof(bool));
    ptr += sizeof(bool);
    
    // Deserialize integer fields
    memcpy(&t->pressure_plate_up_tx_key, ptr, sizeof(int));
    ptr += sizeof(int);
    memcpy(&t->pressure_plate_down_tx_key, ptr, sizeof(int));
    ptr += sizeof(int);
    memcpy(&t->pressure_plate_event, ptr, sizeof(int));
    ptr += sizeof(int);
    memcpy(&t->wall_switch_up_tx_key, ptr, sizeof(int));
    ptr += sizeof(int);
    memcpy(&t->wall_switch_down_tx_key, ptr, sizeof(int));
    ptr += sizeof(int);
    memcpy(&t->wall_switch_event, ptr, sizeof(int));
    ptr += sizeof(int);
    memcpy(&t->cached_live_npcs, ptr, sizeof(int));
    ptr += sizeof(int);
    
    // Deserialize entity data
    size_t entity_count, entity_max;
    memcpy(&entity_count, ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    memcpy(&entity_max, ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    
    // Allocate entities array
    t->entities = malloc(entity_max * sizeof(entityid));
    if (!t->entities) {
        merror("Failed to allocate entities array during deserialization");
        return false;
    }
    memcpy(t->entities, ptr, entity_max * sizeof(entityid));
    ptr += entity_max * sizeof(entityid);
    
    t->entity_count = entity_count;
    t->entity_max = entity_max;
    
    return true;
}
