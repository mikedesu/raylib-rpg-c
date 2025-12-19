#pragma once

#include "gamestate.h"

static inline bool is_entity_adjacent(gamestate& g, entityid id0, entityid id1) {
    //massert(g, "gamestate is NULL");
    massert(id0 != ENTITYID_INVALID, "id0 is invalid");
    massert(id1 != ENTITYID_INVALID, "id1 is invalid");
    massert(id0 != id1, "id0 and id1 are the same");

    // check if on same floor
    auto loc0 = g.ct.get<location>(id0).value_or((vec3){-1, -1, -1});
    auto loc1 = g.ct.get<location>(id1).value_or((vec3){-1, -1, -1});
    if (loc0.z == -1 || loc1.z == -1 || loc0.z != loc1.z)
        return false;

    // use e0 and check the surrounding 8 tiles
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            if (x == 0 && y == 0)
                continue;
            if (loc0.x + x == loc1.x && loc0.y + y == loc1.y)
                return true;
        }
    }
    return false;
}
