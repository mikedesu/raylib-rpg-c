#pragma once

#include "gamestate.h"
#include "spritegroup.h"

extern unordered_map<entityid, spritegroup_t*> spritegroups;

static inline bool camera_lock_on(gamestate& g) {
    //massert(g, "gamestate is NULL");
    if (!g.cam_lockon)
        return false;
    if (spritegroups.find(g.hero_id) == spritegroups.end())
        return false;
    spritegroup_t* grp = spritegroups[g.hero_id];
    if (!grp)
        return false;
    // get the old camera position
    Vector2 old_target = g.cam2d.target;
    g.cam2d.target = (Vector2){grp->dest.x, grp->dest.y};
    // if the target changes, we need to set a flag indicating as such
    if (old_target.x != g.cam2d.target.x || old_target.y != g.cam2d.target.y)
        g.cam_changed = g.frame_dirty = true;
    else
        g.cam_changed = false;
    return true;
}
