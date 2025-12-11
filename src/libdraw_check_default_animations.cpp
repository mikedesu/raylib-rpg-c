#include "libdraw_check_default_animations.h"
#include "spritegroup.h"

extern unordered_map<entityid, spritegroup_t*> spritegroups;

bool check_default_animations(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    for (entityid id = 0; id < g->next_entityid; id++) {
        spritegroup_t* sg = spritegroups[id];
        if (sg && sg->current != sg->default_anim)
            return false;
    }
    return true;
}
