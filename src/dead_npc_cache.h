#pragma once

#include "entityid.h"
#include <cstddef>

class dead_npc_cache {
private:
    size_t count;
    entityid ids[8];

public:
    dead_npc_cache() {
        count = 0;
        for (int i = 0; i < 8; i++) {
            ids[i] = INVALID;
        }
    }

    ~dead_npc_cache() {
    }

    size_t get_count() {
        return count;
    }

    void add_id(entityid id) {
        if (count < 8) {
            ids[count++] = id;
        }
        else {
            // Shift all elements left
            for (size_t i = 1; i < 8; i++) {
                ids[i - 1] = ids[i];
            }
            ids[7] = id; // Add new id at end
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
