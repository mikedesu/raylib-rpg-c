#pragma once

#include "dungeon_floor.h"
#include "entitytype.h"
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
    //spritegroup* group = spritegroup_create(SPRITEGROUP_DEFAULT_SIZE);
    spritegroup* group = new spritegroup(SPRITEGROUP_DEFAULT_SIZE);

    massert(group, "spritegroup is NULL");
    //disabling this check until dungeon_floor created
    auto df = g.d.get_current_floor();
    auto maybe_loc = g.ct.get<location>(id);

    minfo("checking if has location");
    // if it has a location...
    if (maybe_loc.has_value()) {
        minfo("it DOES have a location");
        const vec3 loc = maybe_loc.value();
        massert(loc.x >= 0 && loc.x < df->get_width(), "location x out of bounds: %d", loc.x);
        massert(loc.y >= 0 && loc.y < df->get_height(), "location y out of bounds: %d", loc.y);

        minfo2("creating spritegroups...");
        minfo2("num_keys: %d", num_keys);
        int count = 0;
        for (int i = 0; i < num_keys; i++) {
            const int k = keys[i];
            minfo("k: %d", k);
            Texture2D* tex = &txinfo[k].texture;
            auto s = make_shared<sprite>(tex, txinfo[k].contexts, txinfo[k].num_frames);
            massert(s, "s is NULL for some reason!");
            group->add(s);
            count++;
        }
        msuccess2("spritegroups created");
        minfo2("count: %d", count);
        minfo2("setting id: %d", id);
        group->id = id;

        string n = g.ct.get<name>(id).value_or("no-name");
        minfo2("name: %s", n.c_str());
        entitytype_t t = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
        string t_s = entitytype_to_str(t);
        minfo2("type: %s", t_s.c_str());
        minfo2("group->get(0)");

        // how many items are in the group?
        minfo2("group->count(): %lu", group->count());

        auto s = group->get(0);
        massert(s, "sprite is NULL");

        minfo2("group->current = 0");
        group->current = 0;

        float x = loc.x * DEFAULT_TILE_SIZE + offset_x;
        float y = loc.y * DEFAULT_TILE_SIZE + offset_y;
        minfo("getting width");
        float w = s->get_width();
        minfo("getting height");
        float h = s->get_height();

        minfo("setting destination...");
        group->dest = Rectangle{x, y, w, h};
        group->off_x = offset_x;
        group->off_y = offset_y;
        spritegroups[id] = group;

        msuccess("END create spritegroup");

        return true;
    }

    minfo("it does NOT have a location");
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
    float x = -DEFAULT_TILE_SIZE + offset_x;
    float y = -DEFAULT_TILE_SIZE + offset_y;
    float w = s->get_width();
    float h = s->get_height();
    group->dest = Rectangle{x, y, w, h};
    group->off_x = offset_x;
    group->off_y = offset_y;
    spritegroups[id] = group;
    msuccess("END create spritegroup");
    return true;
}

static inline bool create_sg(gamestate& g, entityid id, int* keys, int num_keys) {
    return create_spritegroup(g, id, keys, num_keys, -12, -12);
}
