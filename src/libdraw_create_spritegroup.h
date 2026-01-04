#pragma once

#include "gamestate.h"
#include "spritegroup.h"
#include "textureinfo.h"

extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];
extern unordered_map<entityid, spritegroup_t*> spritegroups;

static inline bool create_spritegroup(gamestate& g, entityid id, int* keys, int num_keys, int offset_x, int offset_y)
{
    minfo("BEGIN create_spritegroup");
    massert(txinfo, "txinfo is null");
    // can hold up to 32 sprites
    spritegroup_t* group = spritegroup_create(SPRITEGROUP_DEFAULT_SIZE);
    massert(group, "spritegroup is NULL");
    //disabling this check until dungeon_floor created
    auto df = g.d.get_current_floor();
    auto maybe_loc = g.ct.get<location>(id);
    // if it has a location...
    if (maybe_loc.has_value())
    {
        const vec3 loc = maybe_loc.value();
        massert(loc.x >= 0 && loc.x < df.get_width(), "location x out of bounds: %d", loc.x);
        massert(loc.y >= 0 && loc.y < df.height, "location y out of bounds: %d", loc.y);
        //if (loc.x < 0 || loc.x >= df.get_width() || loc.y < 0 || loc.y >= df.height)
        //{
        //    spritegroup_destroy(group);
        //    merror("END create spritegroup");
        //    return false;
        //}
        for (int i = 0; i < num_keys; i++)
        {
            const int k = keys[i];
            Texture2D* tex = &txinfo[k].texture;
            auto s = sprite_create2(tex, txinfo[k].contexts, txinfo[k].num_frames);
            spritegroup_add(group, s);
            //msuccess("added sprite to group!");
        }
        group->id = id;
        auto s = spritegroup_get(group, 0);
        massert(s, "sprite is NULL");
        if (!s)
        {
            spritegroup_destroy(group);
            merror("END create spritegroup");
            return false;
        }
        group->current = 0;
        group->dest = (Rectangle){(float)loc.x * DEFAULT_TILE_SIZE + offset_x, (float)loc.y * DEFAULT_TILE_SIZE + offset_y, (float)s->width, (float)s->height};
        group->off_x = offset_x;
        group->off_y = offset_y;
        spritegroups[id] = group;
        msuccess("END create spritegroup");
        return true;
    }

    // if it does not have a location...
    for (int i = 0; i < num_keys; i++)
    {
        int k = keys[i];
        Texture2D* tex = &txinfo[k].texture;
        auto s = sprite_create2(tex, txinfo[k].contexts, txinfo[k].num_frames);
        spritegroup_add(group, s);
        //msuccess("added sprite to group!");
    }
    group->id = id;
    group->current = 0;
    auto s = spritegroup_get(group, 0);
    massert(s, "sprite is NULL");
    if (!s)
    {
        spritegroup_destroy(group);
        merror("END create spritegroup");
        return false;
    }
    const float x = -DEFAULT_TILE_SIZE + offset_x, y = -DEFAULT_TILE_SIZE + offset_y, w = s->width, h = s->height;
    group->dest = (Rectangle){x, y, w, h};
    group->off_x = offset_x;
    group->off_y = offset_y;
    spritegroups[id] = group;
    msuccess("END create spritegroup");
    return true;
}
