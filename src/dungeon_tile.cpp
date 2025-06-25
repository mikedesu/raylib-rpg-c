#include "dungeon_tile.h"
#include "entityid.h"
#include "gamestate.h"
#include "mprint.h"
#include <algorithm>
#include <vector>

using std::vector;

void tile_init(tile_t* const t, tiletype_t type) {
    massert(t, "tile is NULL");
    t->type = type;
    t->visible = false;
    t->explored = false;
    //t->has_pressure_plate = false;
    //t->has_wall_switch = false;
    //t->wall_switch_on = false;
    t->cached_player_present = false;
    t->dirty_entities = true;
    t->dirty_visibility = true;

    t->entities = new vector<entityid>();
    t->entities->reserve(DUNGEON_TILE_MAX_ENTITIES_DEFAULT);
    //t->pressure_plate_up_tx_key = -1;
    //t->pressure_plate_down_tx_key = -1;
    //t->pressure_plate_event = -1;
    //t->wall_switch_up_tx_key = -1;
    //t->wall_switch_down_tx_key = -1;
    //t->wall_switch_event = -1;
    //t->entity_count = 0;
    t->cached_live_npcs = 0;
}

bool tile_resize(tile_t* t) {
    massert(t, "tile is NULL");
    massert(t->entities, "tile entities is NULL");
    if (t->entities->capacity() * 2 > DUNGEON_TILE_MAX_ENTITIES_MAX) {
        merror("dungeon_tile_resize: new capacity exceeds max entities");
        return false;
    }
    t->entities->reserve(t->entities->capacity() * 2);
    return true;
}

entityid tile_add(tile_t* const t, entityid id) {
    massert(t, "tile is NULL");
    massert(t->entities, "tile entities is NULL");
    // Early exit if tile is full and resize fails
    //if (t->entity_count >= t->entity_max) {
    //    ////mwarning("dungeon_tile_add: Tile full, attempting resize");
    //    if (!tile_resize(t)) {
    //        //merror("dungeon_tile_add: Resize failed");
    //        return ENTITYID_INVALID;
    //    }
    //}
    t->entities->push_back(id);
    t->dirty_entities = true;
    return id;
}

entityid tile_remove(tile_t* tile, entityid id) {
    massert(tile, "tile is NULL");
    massert(tile->entities, "tile entities is NULL");
    massert(id != ENTITYID_INVALID, "tile_remove: id is invalid");
    auto it = std::find(tile->entities->begin(), tile->entities->end(), id);
    if (it != tile->entities->end()) {
        tile->entities->erase(it);
        tile->dirty_entities = true;
        return id;
    }
    merror("dungeon_tile_remove: entity not found");
    return ENTITYID_INVALID;
}

tile_t* tile_create(tiletype_t type) {
    massert(type >= TILE_NONE, "tile_create: type is invalid");
    massert(type < TILE_COUNT, "tile_create: type is out of bounds");
    tile_t* t = (tile_t*)malloc(sizeof(tile_t));
    if (!t) return NULL;
    tile_init(t, type);
    return t;
}

void tile_free(tile_t* t) {
    if (!t) return;
    delete t->entities;
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
    for (entityid id : *t->entities) {
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
    tile_t* const t = df_tile_at(df, (vec3){x, y, z});
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
    tile_t* t = df_tile_at(df, (vec3){x, y, z});
    massert(t, "failed to get tile");
    return tile_live_npc_count(g, t);
}

/*
size_t tile_serialized_size(const tile_t* t) {
    massert(t, "tile is NULL");
    // Calculate size by exactly matching what's written in tile_serialize
    size_t size = 0;
    // Basic fields
    size += sizeof(tiletype_t); // type
    size += 8 * sizeof(bool); // 8 boolean fields
    // Integer fields
    size += 7 * sizeof(int); // 7 integer fields
    // Entity data
    size += 2 * sizeof(size_t); // entity_count and entity_max
    size += t->entity_max * sizeof(entityid); // entities array
    return size;
}
*/

/*
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
*/

/*
bool tile_deserialize(tile_t* t, const char* buffer, size_t buffer_size) {
    massert(t, "tile is NULL");
    massert(buffer, "buffer is NULL");
    // Validate buffer size
    size_t required_size = tile_serialized_size(t);
    if (buffer_size < required_size) {
        merror("Buffer too small for tile deserialization: %zu < %zu", buffer_size, required_size);
        return false;
    }
    const char* ptr = buffer;
    // Deserialize basic fields
    memcpy(&t->type, ptr, sizeof(tiletype_t));
    ptr += sizeof(tiletype_t);
    // Validate tile type
    if (t->type < TILE_NONE || t->type >= TILE_COUNT) {
        merror("Invalid tile type during deserialization: %d", t->type);
        return false;
    }
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
    // Validate entity data
    if (entity_max > DUNGEON_TILE_MAX_ENTITIES_MAX) {
        merror("Invalid entity_max during deserialization: %zu", entity_max);
        return false;
    }
    if (entity_count > entity_max) {
        merror("Invalid entity_count during deserialization: %zu > %zu", entity_count, entity_max);
        return false;
    }
    // Allocate entities array
    t->entities = (entityid*)malloc(entity_max * sizeof(entityid));
    if (!t->entities) {
        merror("Failed to allocate entities array during deserialization");
        return false;
    }
    // Initialize all entities to invalid
    for (size_t i = 0; i < entity_max; i++) {
        t->entities[i] = ENTITYID_INVALID;
    }
    // Copy entity data
    memcpy(t->entities, ptr, entity_max * sizeof(entityid));
    ptr += entity_max * sizeof(entityid);
    t->entity_count = entity_count;
    t->entity_max = entity_max;
    return true;
}
*/

size_t tile_memory_size(const tile_t* t) {
    massert(t, "tile is NULL");
    // Calculate the memory size of a tile
    size_t size = 0;
    // Size of the tile_t struct itself
    size += sizeof(tile_t);
    // Size of vector and its contents
    if (t->entities) {
        size += sizeof(vector<entityid>);
        size += t->entities->capacity() * sizeof(entityid);
    }
    return size;
}
