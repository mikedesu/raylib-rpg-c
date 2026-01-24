#pragma once

#include "dungeon_floor.h"
#include "gamestate.h"
#include "libgame_defines.h"
#include "spritegroup.h"
#include "textureinfo.h"

extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];
extern unordered_map<entityid, spritegroup*> spritegroups;

static inline bool create_spritegroup(gamestate& g, entityid id, int* keys, int num_keys, int offset_x, int offset_y) {
    minfo("BEGIN create_spritegroup");
    massert(txinfo, "txinfo is null");
    // can hold up to 32 sprites
    spritegroup* group = new spritegroup(SPRITEGROUP_DEFAULT_SIZE);
    massert(group, "spritegroup is NULL");
    //disabling this check until dungeon_floor created
    auto df = g.d.get_current_floor();
    auto maybe_loc = g.ct.get<location>(id);

    // if it has a location...
    if (maybe_loc.has_value()) {
        const vec3 loc = maybe_loc.value();
        massert(loc.x >= 0 && loc.x < df->get_width(), "location x out of bounds: %d", loc.x);
        massert(loc.y >= 0 && loc.y < df->get_height(), "location y out of bounds: %d", loc.y);
        for (int i = 0; i < num_keys; i++) {
            const int k = keys[i];
            Texture2D* tex = &txinfo[k].texture;
            auto s = make_shared<sprite>(tex, txinfo[k].contexts, txinfo[k].num_frames);
            group->add(s);
        }
        group->id = id;
        auto s = group->get(0);
        massert(s, "sprite is NULL");
        group->current = 0;
        float x = loc.x * DEFAULT_TILE_SIZE + offset_x;
        float y = loc.y * DEFAULT_TILE_SIZE + offset_y;
        float w = s->get_width();
        float h = s->get_height();
        group->dest = Rectangle{x, y, w, h};
        group->off_x = offset_x;
        group->off_y = offset_y;
        spritegroups[id] = group;
        msuccess("END create spritegroup");
        return true;
    }

    // if it does not have a location...
    for (int i = 0; i < num_keys; i++) {
        int k = keys[i];
        Texture2D* tex = &txinfo[k].texture;
        auto s = make_shared<sprite>(tex, txinfo[k].contexts, txinfo[k].num_frames);
        group->add(s);
    }
    group->id = id;
    group->current = 0;
    //auto s = spritegroup_get(group, 0);
    auto s = group->get(0);
    massert(s, "sprite is NULL");
    const float x = -DEFAULT_TILE_SIZE + offset_x;
    const float y = -DEFAULT_TILE_SIZE + offset_y;
    const float w = s->get_width();
    const float h = s->get_height();
    group->dest = Rectangle{x, y, w, h};
    group->off_x = offset_x;
    group->off_y = offset_y;
    spritegroups[id] = group;
    msuccess("END create spritegroup");
    return true;
}
