#include "liblogic_is_entity_adjacent.h"

bool is_entity_adjacent(shared_ptr<gamestate> g, entityid id0, entityid id1) {
    massert(g, "gamestate is NULL");
    massert(id0 != ENTITYID_INVALID, "id0 is invalid");
    massert(id1 != ENTITYID_INVALID, "id1 is invalid");
    massert(id0 != id1, "id0 and id1 are the same");
    // use e0 and check the surrounding 8 tiles
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            if (x == 0 && y == 0) continue;
            optional<vec3> maybe_loc0 = g->ct.get<location>(id0);
            optional<vec3> maybe_loc1 = g->ct.get<location>(id1);
            if (maybe_loc0.has_value() && maybe_loc1.has_value()) {
                vec3 loc0 = maybe_loc0.value();
                vec3 loc1 = maybe_loc1.value();
                if (loc0.x + x == loc1.x && loc0.y + y == loc1.y) {
                    return true;
                }
            }
        }
    }
    return false;
}
