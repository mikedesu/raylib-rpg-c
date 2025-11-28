#include "libdraw_get_shield_sprite.h"

extern unordered_map<entityid, spritegroup_t*> spritegroups;

shared_ptr<sprite> get_shield_front_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(sg, "spritegroup is NULL");

    shared_ptr<sprite> retval = nullptr;
    const entityid shield = g->ct.get<equipped_shield>(id).value_or(ENTITYID_INVALID);
    if (shield == ENTITYID_INVALID) return retval;

    auto it = spritegroups.find(shield);
    if (it == spritegroups.end()) return retval;

    spritegroup_t* w_sg = it->second;
    if (!w_sg) return retval;

    if (sg->current == SG_ANIM_NPC_GUARD_SUCCESS) retval = spritegroup_get(w_sg, SG_ANIM_BUCKLER_SUCCESS_FRONT);

    return retval;
}


shared_ptr<sprite> get_shield_back_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(sg, "spritegroup is NULL");

    shared_ptr<sprite> retval = nullptr;

    const entityid shield = g->ct.get<equipped_shield>(id).value_or(ENTITYID_INVALID);
    if (shield == ENTITYID_INVALID) return retval;

    auto it = spritegroups.find(shield);
    if (it == spritegroups.end()) return retval;

    spritegroup_t* w_sg = it->second;
    if (!w_sg) return retval;

    if (sg->current == SG_ANIM_NPC_GUARD_SUCCESS) retval = spritegroup_get(w_sg, SG_ANIM_BUCKLER_SUCCESS_BACK);

    return retval;
}
