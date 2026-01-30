#pragma once
#include "gamestate.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"

extern unordered_map<entityid, spritegroup*> spritegroups;

static inline void update_shield_for_entity(gamestate& g, entityid id, spritegroup* sg) {
    minfo("update shield for entity %d", id);
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    spritegroup* shield_sg = nullptr;
    int ctx = -1;
    const entityid shield_id = g.ct.get<equipped_shield>(id).value_or(ENTITYID_INVALID);
    if (shield_id == ENTITYID_INVALID) {
        merror2("shield_id invalid");
        return;
    }
    
    shield_sg = spritegroups[shield_id];
    if (!shield_sg) {
        merror2("shield_sg null");
        return;
    }

    ctx = sg->sprites2->at(sg->current)->get_currentcontext();
    shield_sg->setcontexts(ctx);
    //merror("I expect a crash here...");
    sg->set_current(SG_ANIM_BUCKLER_SUCCESS_FRONT);
}
