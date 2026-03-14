/** @file check_default_animations.h
 *  @brief Archived commented-out helper for validating default sprite animations.
 */

//#pragma once
//
//#include "gamestate.h"
//#include "spritegroup.h"
//
//extern unordered_map<entityid, spritegroup*> spritegroups;
//
//static inline bool check_default_animations(shared_ptr<gamestate> g) {
//    massert(g, "gamestate is NULL");
//    for (entityid id = 0; id < g->next_entityid; id++) {
//        spritegroup* sg = spritegroups[id];
//        if (sg && sg->current != sg->default_anim) return false;
//    }
//    return true;
//}
