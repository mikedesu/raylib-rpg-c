#pragma once

#include "entityid.h"
#include <cstddef>

#ifndef DEAD_NPC_CACHE_SIZE
#define DEAD_NPC_CACHE_SIZE 8
#endif

class dead_npc_cache {
private:
    size_t count;
    entityid ids[DEAD_NPC_CACHE_SIZE];

public:
    dead_npc_cache() {
        count = 0;
        for (int i = 0; i < DEAD_NPC_CACHE_SIZE; i++) {
            ids[i] = INVALID;
        }
    }

    ~dead_npc_cache() {
    }

    size_t get_count() {
        return count;
    }

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

    int contains(entityid id) {
        for (int i = 0; i < count; i++) {
            if (ids[i] == id) {
                return i;
            }
        }
        return -1;
    }

    bool remove_id(entityid id) {
        int i = contains(id);
        if (i != -1) {
            count--;
            for (; i < count; i++) {
                ids[i] = ids[i + 1];
            }
            ids[count] = INVALID;
            return true;
        }
        return false;
    }
};
