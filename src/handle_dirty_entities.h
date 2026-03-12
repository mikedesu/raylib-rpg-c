#pragma once

#include "create_sg_byid.h"
#include "gamestate.h"
#include "update_sprite.h"

static inline void libdraw_handle_dirty_entities(gamestate& g) {
    if (!g.dirty_entities)
        return;
    for (entityid i = g.new_entityid_begin; i < g.new_entityid_end; i++) {
        create_sg_byid(g, i);
        libdraw_update_sprite_pre(g, i);
    }
    g.dirty_entities = false;
    g.new_entityid_begin = ENTITYID_INVALID;
    g.new_entityid_end = ENTITYID_INVALID;
    g.frame_dirty = true;
}
