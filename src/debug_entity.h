#pragma once

#include "entityid.h"
#include "gamestate.h"
#include "massert.h"
#include "mprint.h"

static inline void debug_entity(gamestate& g, entityid id) {
    minfo("id: %d", id);
    minfo("name: %s", g.ct.get<name>(id).value_or("no-name").c_str());

    entitytype_t t = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
    std::string t_s = entitytype_to_str(t);

    minfo("type: %d", t);
    minfo("type: %s", t_s.c_str());
}
