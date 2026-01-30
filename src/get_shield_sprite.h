#pragma once

#include "gamestate.h"
#include "sprite.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"

extern unordered_map<entityid, spritegroup*> spritegroups;

static inline shared_ptr<sprite> get_shield_front_sprite(gamestate& g, entityid id, spritegroup* sg) {
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(sg, "spritegroup is NULL");
    const entityid shield = g.ct.get<equipped_shield>(id).value_or(ENTITYID_INVALID);
    if (shield == ENTITYID_INVALID) {
        return nullptr;
    }
    auto it = spritegroups.find(shield);
    if (it == spritegroups.end()) {
        return nullptr;
    }
    spritegroup* w_sg = it->second;
    if (!w_sg) {
        return nullptr;
    }
    if (sg->current != SG_ANIM_NPC_GUARD_SUCCESS) {
        return nullptr;
    }
    return w_sg->get( SG_ANIM_BUCKLER_SUCCESS_FRONT);
}


static inline shared_ptr<sprite> get_shield_back_sprite(gamestate& g, entityid id, spritegroup* sg) {
    //massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(sg, "spritegroup is NULL");
    shared_ptr<sprite> retval = nullptr;
    const entityid shield = g.ct.get<equipped_shield>(id).value_or(ENTITYID_INVALID);
    if (shield == ENTITYID_INVALID) {
        return retval;
    }
    auto it = spritegroups.find(shield);
    if (it == spritegroups.end()) {
        return retval;
    }
    spritegroup* w_sg = it->second;
    if (!w_sg) {
        return retval;
    }
    if (sg->current == SG_ANIM_NPC_GUARD_SUCCESS) {
        retval = w_sg->get( SG_ANIM_BUCKLER_SUCCESS_BACK);
    }
    return retval;
}
