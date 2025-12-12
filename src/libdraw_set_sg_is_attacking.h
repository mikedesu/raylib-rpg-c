#pragma once
#include "gamestate.h"
#include "libdraw_update_weapon_for_entity.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"

static inline void libdraw_set_sg_is_attacking(shared_ptr<gamestate> g, entityid id, spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    //weapontype wtype = g_get_weapontype(g, weapon);
    int cur = SG_ANIM_NPC_ATTACK;
    //if (wtype == WEAPON_BOW) {
    //    cur = SG_ANIM_NPC_SHOT;
    //}
    //if (race == RACE_BAT) {
    //    cur = SG_ANIM_BAT_ATTACK;
    //} else if (race == RACE_GREEN_SLIME) {
    //    cur = SG_ANIM_SLIME_ATTACK;
    //}
    spritegroup_set_current(sg, cur);
    update_weapon_for_entity(g, id, sg);
    g->ct.set<attacking>(id, false);
}
