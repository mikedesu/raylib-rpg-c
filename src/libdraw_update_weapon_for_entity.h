/** @file libdraw_update_weapon_for_entity.h
 *  @brief Helper for syncing equipped-weapon animation with an entity spritegroup.
 */

#pragma once

#include "gamestate.h"
#include "libdraw_context.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"

/** @brief Update an equipped weapon spritegroup to match the owner's current context. */
static inline void update_weapon_for_entity(gamestate& g, entityid id, spritegroup* sg) {
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    spritegroup* w_sg = nullptr;
    entityid weaponid = ENTITYID_INVALID;
    int ctx = -1;
    weaponid = g.ct.get<equipped_weapon>(id).value_or(ENTITYID_INVALID);
    if (weaponid == ENTITYID_INVALID) return;
    w_sg = spritegroups[weaponid];
    if (!w_sg) return;
    ctx = sg->sprites2->at(sg->current)->get_currentcontext();
    w_sg->setcontexts(ctx);
    // this really should be either SLASH_F or SLASH_B
    // eventually we will select this based on other factors as well
    // or rather we'll have a better way to select which animation joins the entity's attack
    // lets test this theory by changing it to the B anim
    //spritegroup_set_current(w_sg, SG_ANIM_LONGSWORD_SLASH_B);
    w_sg->set_current(SG_ANIM_LONGSWORD_SLASH_F);
}
