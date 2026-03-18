/** @file dead_npc_cache.h
 *  @brief Small rolling cache of dead NPC entity ids for tile occupancy.
 */

#pragma once

#include "entityid.h"
#include <cstddef>

#ifndef DEAD_NPC_CACHE_SIZE
/// @brief Maximum number of dead NPC ids retained in one tile cache.
#define DEAD_NPC_CACHE_SIZE 8
#endif

/**
 * @brief Fixed-size cache of dead NPC ids associated with a tile.
 *
 * When full, newly added ids evict the oldest cached entry.
 */
class dead_npc_cache {
private:
    size_t count;
    entityid ids[DEAD_NPC_CACHE_SIZE];

public:
    /** @brief Construct an empty dead-NPC cache. */
    dead_npc_cache() {
        count = 0;
        for (int i = 0; i < DEAD_NPC_CACHE_SIZE; i++) {
            ids[i] = ENTITYID_INVALID;
        }
    }

    ~dead_npc_cache() = default;

    /** @brief Return the number of cached dead NPC ids. */
    size_t get_count() const {
        return count;
    }

    /** @brief Append a dead NPC id, evicting the oldest id if the cache is full. */
    void add_id(entityid id) {
        if (count < DEAD_NPC_CACHE_SIZE) {
            ids[count++] = id;
        }
        else {
            // Shift all elements left
            for (size_t i = 1; i < DEAD_NPC_CACHE_SIZE; i++) {
                ids[i - 1] = ids[i];
            }
            ids[DEAD_NPC_CACHE_SIZE - 1] = id; // Add new id at end
        }
    }

    /** @brief Return the index of an id in the cache, or `-1` when absent. */
    int contains(entityid id) const {
        for (size_t i = 0; i < count; i++) {
            if (ids[i] == id) {
                return i;
            }
        }
        return -1;
    }

    /** @brief Remove an id from the cache if present. */
    bool remove_id(entityid id) {
        int i = contains(id);
        if (i != -1) {
            count--;
            for (; i < count; i++) {
                ids[i] = ids[i + 1];
            }
            ids[count] = ENTITYID_INVALID;
            return true;
        }
        return false;
    }

    /** @brief Return the most recently cached dead NPC id, or `ENTITYID_INVALID`. */
    entityid top() const {
        if (count == 0) {
            return ENTITYID_INVALID;
        }
        return ids[count - 1];
    }

    /** @brief Remove and return the most recently cached dead NPC id, or `ENTITYID_INVALID`. */
    entityid pop() {
        if (count == 0) {
            return ENTITYID_INVALID;
        }
        entityid id = ids[count - 1];
        ids[count - 1] = ENTITYID_INVALID;
        count--;
        return id;
    }
};
