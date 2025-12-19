#pragma once


#include "gamestate.h"


static inline vec3 get_loc_facing_player(gamestate& g) {
    optional<vec3> maybe_loc = g.ct.get<location>(g.hero_id);

    if (maybe_loc.has_value()) {
        vec3 loc = g.ct.get<location>(g.hero_id).value();
        direction_t dir = g.ct.get<direction>(g.hero_id).value();
        if (dir == DIR_UP) {
            loc.y -= 1;
        } else if (dir == DIR_DOWN) {
            loc.y += 1;
        } else if (dir == DIR_LEFT) {
            loc.x -= 1;
        } else if (dir == DIR_RIGHT) {
            loc.x += 1;
        } else if (dir == DIR_UP_LEFT) {
            loc.x -= 1;
            loc.y -= 1;
        } else if (dir == DIR_UP_RIGHT) {
            loc.x += 1;
            loc.y -= 1;
        } else if (dir == DIR_DOWN_LEFT) {
            loc.x -= 1;
            loc.y += 1;
        } else if (dir == DIR_DOWN_RIGHT) {
            loc.x += 1;
            loc.y += 1;
        }
        return loc;
    }

    return (vec3){-1, -1, -1};
}
