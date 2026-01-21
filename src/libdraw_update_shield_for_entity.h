#pragma once

#include "gamestate.h"
#include "spritegroup.h"


#include "spritegroup_anim.h"

extern unordered_map<entityid, spritegroup_t*> spritegroups;

static inline void update_shield_for_entity(gamestate& g, entityid id, spritegroup_t* sg) {
    minfo("update shield for entity %d", id);

    //massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");

    spritegroup_t* shield_sg = nullptr;
    entityid shield_id = ENTITYID_INVALID;
    int ctx = -1;

    shield_id = g.ct.get<equipped_shield>(id).value_or(ENTITYID_INVALID);

    if (shield_id == ENTITYID_INVALID) {
        return;
    }

    shield_sg = spritegroups[shield_id];

    if (!shield_sg) {
        return;
    }

    ctx = sg->sprites2->at(sg->current)->get_currentcontext();
    spritegroup_setcontexts(shield_sg, ctx);

    merror("I expect a crash here...");
    spritegroup_set_current(shield_sg, SG_ANIM_BUCKLER_SUCCESS_FRONT);
}
