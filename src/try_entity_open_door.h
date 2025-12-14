#pragma once


#include "gamestate.h"
#include "tile_has_door.h"

static inline bool try_entity_open_door(shared_ptr<gamestate> g, entityid id, vec3 loc) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");


    if (tile_has_door(g, loc)) {
        auto df = d_get_current_floor(g->dungeon);
        auto t = df_tile_at(df, loc);

        for (size_t i = 0; i < t->entities->size(); i++) {
            const entityid myid = t->entities->at(i);
            const entitytype_t type = g->ct.get<entitytype>(myid).value_or(ENTITY_NONE);
            if (type == ENTITY_DOOR) {
                auto maybe_is_open = g->ct.get<door_open>(myid);

                if (maybe_is_open.has_value()) {
                    const bool is_open = maybe_is_open.value();
                    if (is_open) {
                        g->ct.set<door_open>(myid, false);

                        //if (id == g->hero_id) {
                        //    add_message(g, "You closed a door");
                        //}

                    } else {
                        g->ct.set<door_open>(myid, true);

                        //if (id == g->hero_id) {
                        //    add_message(g, "You opened a door");
                        //}
                    }

                    PlaySound(g->sfx->at(SFX_CHEST_OPEN));
                    return true;
                }
            }
        }
    }

    return false;
}
