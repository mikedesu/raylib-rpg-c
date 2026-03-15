/** @file get_weapon_sprite.h
 *  @brief Equipped-weapon sprite lookup helpers.
 */

#pragma once

#include "gamestate.h"
#include "libdraw_context.h"
#include "sprite.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"

/** @brief Return the equipped weapon's front-layer sprite for the entity's current animation. */
static inline shared_ptr<sprite> get_weapon_front_sprite(gamestate& g, entityid id, spritegroup* sg) {
    //massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(sg, "spritegroup is NULL");
    shared_ptr<sprite> retval = nullptr;
    entityid weapon = g.ct.get<equipped_weapon>(id).value_or(ENTITYID_INVALID);
    if (weapon == ENTITYID_INVALID)
        return retval;
    auto it = libdraw_ctx.spritegroups.find(weapon);
    if (it == libdraw_ctx.spritegroups.end())
        return retval;
    spritegroup* w_sg = it->second;
    if (!w_sg)
        return retval;
    if (sg->current == SG_ANIM_NPC_ATTACK)
        retval = w_sg->get( SG_ANIM_LONGSWORD_SLASH_F);
        //retval = spritegroup_get(w_sg, SG_ANIM_LONGSWORD_SLASH_F);
    //else if (sg->current == SG_ANIM_NPC_SHOT) {
    //    retval = spritegroup_get(w_sg, SG_ANIM_BOW_SHOT_F);
    //}
    return retval;
}

/** @brief Return the equipped weapon's back-layer sprite for the entity's current animation. */
static inline shared_ptr<sprite> get_weapon_back_sprite(gamestate& g, entityid id, spritegroup* sg) {
    //massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(sg, "spritegroup is NULL");
    shared_ptr<sprite> retval = nullptr;
    entityid weapon = g.ct.get<equipped_weapon>(id).value_or(ENTITYID_INVALID);
    if (weapon == ENTITYID_INVALID) return retval;
    auto it = libdraw_ctx.spritegroups.find(weapon);
    if (it == libdraw_ctx.spritegroups.end()) return retval;
    spritegroup* w_sg = it->second;
    if (!w_sg) return retval;
    //if (sg->current == SG_ANIM_NPC_ATTACK) retval = spritegroup_get(w_sg, SG_ANIM_LONGSWORD_SLASH_B);
    if (sg->current == SG_ANIM_NPC_ATTACK) retval = w_sg->get(SG_ANIM_LONGSWORD_SLASH_B);
    //else if (sg->current == SG_ANIM_NPC_SHOT) {
    //    retval = spritegroup_get(w_sg, SG_ANIM_BOW_SHOT_B);
    //}
    return retval;
}
