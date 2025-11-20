#include "libdraw_set_sg.h"
#include "spritegroup_anim.h"

void libdraw_set_sg_is_damaged(shared_ptr<gamestate> g, entityid id, spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    race_t r = g->ct.get<race>(id).value_or(RACE_NONE);
    int anim_index = SG_ANIM_NPC_DMG;
    if (r == RACE_GREEN_SLIME) {
        anim_index = SG_ANIM_SLIME_DMG;
    }
    spritegroup_set_current(sg, anim_index);
    g->ct.set<damaged>(id, true);
}


void libdraw_set_sg_is_dead(shared_ptr<gamestate> g, entityid id, spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    if (!g->ct.get<dead>(id).has_value()) {
        return;
    }
    if (!g->ct.get<dead>(id).value()) {
        return;
    }

    race_t r = g->ct.get<race>(id).value_or(RACE_NONE);
    if (r == RACE_NONE) {
        return;
    }

    int anim_index = SG_ANIM_NPC_SPINDIE;

    if (r == RACE_BAT) {
        anim_index = SG_ANIM_BAT_DIE;
    } else if (r == RACE_GREEN_SLIME) {
        anim_index = SG_ANIM_SLIME_DIE;
    }

    if (sg->current == anim_index) {
        return;
    }

    sg_set_default_anim(sg, anim_index);
    spritegroup_set_current(sg, sg->default_anim);
    spritegroup_set_stop_on_last_frame(sg, true);
}
