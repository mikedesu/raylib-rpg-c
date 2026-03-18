/** @file item_cache.h
 *  @brief Fixed-size item-id cache used by dungeon tiles.
 */

#pragma once

#include "entityid.h"
#include <cstddef>

#ifndef ITEM_CACHE_SIZE
/// @brief Maximum number of item ids retained in one tile cache.
#define ITEM_CACHE_SIZE 8
#endif

/**
 * @brief Compact tile-local cache of item entity ids.
 *
 * Preserves insertion order and exposes the newest item as `top()`.
 */
class item_cache {
private:
    size_t count;
    entityid ids[ITEM_CACHE_SIZE];

public:
    /** @brief Construct an empty item cache. */
    item_cache() {
        count = 0;
        for (int i = 0; i < ITEM_CACHE_SIZE; i++) {
            ids[i] = ENTITYID_INVALID;
        }
    }

    ~item_cache() = default;

    /** @brief Return the number of cached item ids. */
    size_t get_count() const {
        return count;
    }

    /** @brief Append an item id if capacity remains. */
    bool add_id(entityid id) {
        if (count >= ITEM_CACHE_SIZE) {
            return false;
        }
        ids[count++] = id;
        return true;
    }

    /** @brief Return the index of an item id in the cache, or `-1` when absent. */
    int contains(entityid id) const {
        for (size_t i = 0; i < count; i++) {
            if (ids[i] == id) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    /** @brief Remove an item id from the cache if present. */
    bool remove_id(entityid id) {
        int i = contains(id);
        if (i == -1) {
            return false;
        }
        count--;
        for (size_t j = static_cast<size_t>(i); j < count; j++) {
            ids[j] = ids[j + 1];
        }
        ids[count] = ENTITYID_INVALID;
        return true;
    }

    /** @brief Return the most recently cached item id, or `ENTITYID_INVALID`. */
    entityid top() const {
        if (count == 0) {
            return ENTITYID_INVALID;
        }
        return ids[count - 1];
    }

    /** @brief Return the cached item id at `index`, or `ENTITYID_INVALID` when out of bounds. */
    entityid at(size_t index) const {
        if (index >= count) {
            return ENTITYID_INVALID;
        }
        return ids[index];
    }
};
