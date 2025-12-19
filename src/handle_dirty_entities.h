#pragma once

#include "create_sg_byid.h"
#include "gamestate.h"

static inline void libdraw_handle_dirty_entities(gamestate& g) {
    //massert(g, "gamestate is NULL");
    //minfo("Begin handle dirty entities");
    if (g.dirty_entities) {
        for (entityid i = g.new_entityid_begin; i < g.new_entityid_end; i++) {
            create_sg_byid(g, i);
        }
        g.dirty_entities = false;
        g.new_entityid_begin = ENTITYID_INVALID;
        g.new_entityid_end = ENTITYID_INVALID;
        g.frame_dirty = true;
    }
    //minfo("End handle dirty entities");
}
