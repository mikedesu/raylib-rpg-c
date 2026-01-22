#pragma once

#include "gamestate.h"
#include "libdraw_update_shield_for_entity.h"
#include "libdraw_update_weapon_for_entity.h"
#include "race.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"

static inline void libdraw_set_sg_is_casting(gamestate& g, entityid id, spritegroup* const sg) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    const int cur = SG_ANIM_NPC_ATTACK;
    spritegroup_set_current(sg, cur);
    update_weapon_for_entity(g, id, sg);
    g.ct.set<casting>(id, true);
}

static inline void libdraw_set_sg_spell_casting(shared_ptr<gamestate> g, entityid id, spritegroup* const sg) {
    minfo("set sg spell casting");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    massert(g->ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_SPELL, "entity is not a spell");
    spritegroup_set_current(sg, 0);
}

static inline void libdraw_set_sg_spell_persisting(shared_ptr<gamestate> g, entityid id, spritegroup* const sg) {
    minfo("set sg spell casting");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    massert(g->ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_SPELL, "entity is not a spell");
    spritegroup_set_current(sg, 1);
}

static inline void libdraw_set_sg_spell_ending(shared_ptr<gamestate> g, entityid id, spritegroup* const sg) {
    minfo("set sg spell casting");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    massert(g->ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_SPELL, "entity is not a spell");
    spritegroup_set_current(sg, 2);
}


static inline void libdraw_set_sg_block_success(gamestate& g, entityid id, spritegroup* const sg) {
    minfo("set sg block success");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    const race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
    const int anim_index = r == RACE_GREEN_SLIME ? SG_ANIM_SLIME_IDLE : r == RACE_WOLF ? SG_ANIM_WOLF_IDLE : SG_ANIM_NPC_GUARD_SUCCESS;
    spritegroup_set_current(sg, anim_index);
    update_shield_for_entity(g, id, sg);
    g.ct.set<block_success>(id, false);
}


static inline void libdraw_set_sg_is_damaged(gamestate& g, entityid id, spritegroup* const sg) {
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    const race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
    const int anim_index = r == RACE_GREEN_SLIME ? SG_ANIM_SLIME_DMG : SG_ANIM_NPC_DMG;
    spritegroup_set_current(sg, anim_index);
}


static inline void libdraw_set_sg_is_dead(gamestate& g, entityid id, spritegroup* const sg) {
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    if (!g.ct.get<dead>(id).has_value()) return;
    if (!g.ct.get<dead>(id).value()) return;
    race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
    if (r == RACE_NONE) return;
    const int anim_index = r == RACE_BAT           ? SG_ANIM_BAT_DIE
                           : r == RACE_GREEN_SLIME ? SG_ANIM_SLIME_DIE
                           : r == RACE_SKELETON    ? SG_ANIM_SKELETON_DIE
                           : r == RACE_RAT         ? SG_ANIM_RAT_DIE
                           : r == RACE_ZOMBIE      ? SG_ANIM_ZOMBIE_DIE
                                                   : SG_ANIM_NPC_SPINDIE;
    if (sg->current == anim_index) return;
    sg->set_default_anim(anim_index);
    spritegroup_set_current(sg, sg->default_anim);
    //sg_reset_anim(sg);
    sg->set_stop_on_last_frame(true);
}


static inline void libdraw_set_sg_is_attacking(gamestate& g, entityid id, spritegroup* const sg) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    //weapontype wtype = g_get_weapontype(g, weapon);
    //if (wtype == WEAPON_BOW) {
    //    cur = SG_ANIM_NPC_SHOT;
    //}
    //if (race == RACE_BAT) {
    //    cur = SG_ANIM_BAT_ATTACK;
    const race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
    massert(r != RACE_NONE, "race cant be none");
    spritegroup_set_current(sg, r == RACE_GREEN_SLIME ? SG_ANIM_SLIME_JUMP_ATTACK
                    : r == RACE_SKELETON  ? SG_ANIM_SKELETON_ATTACK
                    : r == RACE_RAT       ? SG_ANIM_RAT_ATTACK
                    : r == RACE_ZOMBIE    ? SG_ANIM_ZOMBIE_ATTACK
                    : r == RACE_BAT       ? SG_ANIM_BAT_ATTACK
                                          : SG_ANIM_NPC_ATTACK
    );
    update_weapon_for_entity(g, id, sg);
    g.ct.set<attacking>(id, false);
}
