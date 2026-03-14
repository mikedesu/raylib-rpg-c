/** @file draw_update_debug_panel.h
 *  @brief Helper for rebuilding the debug panel text buffer.
 */

#pragma once

#include "gamestate.h"
#include "spritegroup.h"

extern unordered_map<entityid, spritegroup*> spritegroups;

/** @brief Append the current debug text payload into the debug panel buffer. */
static inline void update_debug_panel(gamestate& g) {
    minfo3("update debug panel");
    // concat a string onto the end of the debug panel message
    //char tmp[1024] = {0};
    // get the hero's current context from spritegroups


    if (g.hero_id == INVALID) {
        return;
    }

    //spritegroup* sg = spritegroups[g.hero_id];
    //shared_ptr<sprite> s = sg->get_current();
    //int ctx = s->get_currentcontext();

    char tmp[1024] = {0};
    snprintf(tmp, 1024, "@evildojo666");
    strncat(g.debugpanel.buffer, tmp, sizeof(g.debugpanel.buffer) - strlen(g.debugpanel.buffer) - 1);
    msuccess3("update debug panel");
}
