#pragma once

#include "entityid.h"
#include "gamestate.h"
#include <raymath.h>

static inline bool check_hearing(shared_ptr<gamestate> g, entityid id, vec3 loc) {
    massert(g, "gamestate is NULL");
    if (id == ENTITYID_INVALID || vec3_equal(loc, (vec3){-1, -1, -1}))
        return false;
    const vec3 hero_loc = g->ct.get<location>(g->hero_id).value_or((vec3){-1, -1, -1});
    if (vec3_equal(hero_loc, (vec3){-1, -1, -1}))
        return false;
    // is the hero on the same floor as loc?
    if (hero_loc.z != loc.z)
        return false;
    const Vector2 p0 = {static_cast<float>(hero_loc.x), static_cast<float>(hero_loc.y)}, p1 = {static_cast<float>(loc.x), static_cast<float>(loc.y)};
    const float dist = Vector2Distance(p0, p1), hearing = g->ct.get<hearing_distance>(g->hero_id).value_or(3);
    return dist <= hearing;
}
