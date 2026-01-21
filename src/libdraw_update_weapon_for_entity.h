#pragma once

#include "gamestate.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"


extern unordered_map<entityid, spritegroup_t*> spritegroups;

static inline void update_weapon_for_entity(gamestate& g, entityid id, spritegroup_t* sg) {
    //massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");

    spritegroup_t* w_sg = nullptr;
    entityid weaponid = ENTITYID_INVALID;
    int ctx = -1;

    weaponid = g.ct.get<equipped_weapon>(id).value_or(ENTITYID_INVALID);
    if (weaponid == ENTITYID_INVALID)
        return;
    w_sg = spritegroups[weaponid];
    if (!w_sg)
        return;

    ctx = sg->sprites2->at(sg->current)->get_currentcontext();
    spritegroup_setcontexts(w_sg, ctx);

    // this really should be either SLASH_F or SLASH_B
    // eventually we will select this based on other factors as well
    // or rather we'll have a better way to select which animation joins the entity's attack
    // lets test this theory by changing it to the B anim

    //spritegroup_set_current(w_sg, SG_ANIM_LONGSWORD_SLASH_B);
    spritegroup_set_current(w_sg, SG_ANIM_LONGSWORD_SLASH_F);
}
