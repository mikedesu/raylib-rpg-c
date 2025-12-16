#pragma once

#include "gamestate.h"
#include "spritegroup.h"
#include "textureinfo.h"


extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];
extern unordered_map<entityid, spritegroup_t*> spritegroups;

static inline bool create_spritegroup(shared_ptr<gamestate> g, entityid id, int* keys, int num_keys, int offset_x, int offset_y) {
    minfo("BEGIN create_spritegroup");
    massert(g, "gamestate is NULL");
    massert(txinfo, "txinfo is null");
    // can hold up to 32 sprites
    spritegroup_t* group = spritegroup_create(SPRITEGROUP_DEFAULT_SIZE);
    massert(group, "spritegroup is NULL");
    //disabling this check until dungeon_floor created
    shared_ptr<dungeon_floor_t> df = d_get_current_floor(g->dungeon);
    massert(df, "dungeon_floor is NULL");
    if (!df) {
        spritegroup_destroy(group);
        merror("END create spritegroup");
        return false;
    }
    int df_w = df->width;
    int df_h = df->height;
    //vec3 loc = g_get_loc(g, id);

    optional<vec3> maybe_loc = g->ct.get<location>(id);

    if (maybe_loc.has_value()) {
        vec3 loc = maybe_loc.value();

        massert(loc.x >= 0 && loc.x < df_w, "location x out of bounds: %d", loc.x);
        massert(loc.y >= 0 && loc.y < df_h, "location y out of bounds: %d", loc.y);

        if (loc.x < 0 || loc.x >= df_w || loc.y < 0 || loc.y >= df_h) {
            spritegroup_destroy(group);
            merror("END create spritegroup");
            return false;
        }

        for (int i = 0; i < num_keys; i++) {
            int k = keys[i];
            Texture2D* tex = &txinfo[k].texture;
            shared_ptr<sprite> s = sprite_create2(tex, txinfo[k].contexts, txinfo[k].num_frames);
            spritegroup_add(group, s);
            //msuccess("added sprite to group!");
        }

        group->id = id;
        shared_ptr<sprite> s = spritegroup_get(group, 0);
        massert(s, "sprite is NULL");
        if (!s) {
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
    } else {
        for (int i = 0; i < num_keys; i++) {
            int k = keys[i];
            Texture2D* tex = &txinfo[k].texture;
            shared_ptr<sprite> s = sprite_create2(tex, txinfo[k].contexts, txinfo[k].num_frames);
            spritegroup_add(group, s);
            //msuccess("added sprite to group!");
        }

        group->id = id;
        shared_ptr<sprite> s = spritegroup_get(group, 0);
        massert(s, "sprite is NULL");
        if (!s) {
            spritegroup_destroy(group);
            merror("END create spritegroup");
            return false;
        }
        group->current = 0;

        //group->dest = (Rectangle){(float)loc.x * DEFAULT_TILE_SIZE + offset_x, (float)loc.y * DEFAULT_TILE_SIZE + offset_y, (float)s->width, (float)s->height};
        group->dest = (Rectangle){-1.0f * DEFAULT_TILE_SIZE + offset_x, -1.0f * DEFAULT_TILE_SIZE + offset_y, (float)s->width, (float)s->height};

        group->off_x = offset_x;
        group->off_y = offset_y;

        spritegroups[id] = group;

        msuccess("END create spritegroup");
        return true;
    }
}
