#include "libdraw_update_shield_for_entity.h"
#include "spritegroup_anim.h"

extern unordered_map<entityid, spritegroup_t*> spritegroups;

void update_shield_for_entity(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");

    spritegroup_t* shield_sg = nullptr;
    entityid shield_id = ENTITYID_INVALID;
    int ctx = -1;

    shield_id = g->ct.get<equipped_shield>(id).value_or(ENTITYID_INVALID);
    if (shield_id == ENTITYID_INVALID) return;
    shield_sg = spritegroups[shield_id];
    if (!shield_sg) return;

    ctx = sg->sprites2->at(sg->current)->currentcontext;
    spritegroup_setcontexts(shield_sg, ctx);

    spritegroup_set_current(shield_sg, SG_ANIM_BUCKLER_SUCCESS_FRONT);
}
