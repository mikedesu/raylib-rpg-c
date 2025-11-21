#include "libdraw_check_default_animations.h"

extern unordered_map<entityid, spritegroup_t*> spritegroups;

bool check_default_animations(const shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");

    for (entityid id = 0; id < g->next_entityid; id++) {
        //spritegroup_t* const sg = hashtable_entityid_spritegroup_get(spritegroups, id);
        spritegroup_t* sg = spritegroups[id];

        if (sg && sg->current != sg->default_anim) {
            // which sg isnt done?
            //merror("SPRITE HASNT RESET: %d\n", id);
            return false;
        }
    }

    return true;
}
