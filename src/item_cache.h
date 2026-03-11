#pragma once

#include "entityid.h"
#include <cstddef>

#ifndef ITEM_CACHE_SIZE
#define ITEM_CACHE_SIZE 8
#endif

class item_cache {
private:
    size_t count;
    entityid ids[ITEM_CACHE_SIZE];

public:
    item_cache() {
        count = 0;
        for (int i = 0; i < ITEM_CACHE_SIZE; i++) {
            ids[i] = INVALID;
        }
    }

    ~item_cache() {
    }

    size_t get_count() const {
        return count;
    }

    bool add_id(entityid id) {
        if (count >= ITEM_CACHE_SIZE) {
            return false;
        }
        ids[count++] = id;
        return true;
    }

    int contains(entityid id) const {
        for (size_t i = 0; i < count; i++) {
            if (ids[i] == id) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    bool remove_id(entityid id) {
        int i = contains(id);
        if (i == -1) {
            return false;
        }
        count--;
        for (size_t j = static_cast<size_t>(i); j < count; j++) {
            ids[j] = ids[j + 1];
        }
        ids[count] = INVALID;
        return true;
    }

    entityid top() const {
        if (count == 0) {
            return INVALID;
        }
        return ids[count - 1];
    }

    entityid at(size_t index) const {
        if (index >= count) {
            return INVALID;
        }
        return ids[index];
    }
};
