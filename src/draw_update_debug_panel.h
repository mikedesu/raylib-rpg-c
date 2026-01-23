#pragma once

#include "gamestate.h"
#include "spritegroup.h"

extern unordered_map<entityid, spritegroup*> spritegroups;

static inline void update_debug_panel(gamestate& g) {
    // concat a string onto the end of the debug panel message
    //char tmp[1024] = {0};
    //

    // get the hero's current context from spritegroups

    spritegroup* sg = spritegroups[g.hero_id];
    shared_ptr<sprite> s = sg->get_current();
    int ctx = s->get_currentcontext();

    char tmp[1024] = {0};
    snprintf(tmp, 1024, "hero context: %d\n@evildojo666", ctx);
    strncat(g.debugpanel.buffer, tmp, sizeof(g.debugpanel.buffer) - strlen(g.debugpanel.buffer) - 1);
}
