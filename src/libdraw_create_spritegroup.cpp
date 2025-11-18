#include "libdraw_create_spritegroup.h"
#include "spritegroup.h"


extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];
extern unordered_map<entityid, spritegroup_t*> spritegroups;

//bool create_spritegroup(shared_ptr<gamestate> g, textureinfo* txinfo, entityid id, int* keys, int num_keys, int offset_x, int offset_y) {
bool create_spritegroup(shared_ptr<gamestate> g, entityid id, int* keys, int num_keys, int offset_x, int offset_y) {
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
        return false;
    }
    int df_w = df->width;
    int df_h = df->height;
    //vec3 loc = g_get_loc(g, id);
    vec3 loc = g->ct.get<location>(id).value();
    massert(loc.x >= 0 && loc.x < df_w, "location x out of bounds: %d", loc.x);
    massert(loc.y >= 0 && loc.y < df_h, "location y out of bounds: %d", loc.y);
    if (loc.x < 0 || loc.x >= df_w || loc.y < 0 || loc.y >= df_h) {
        spritegroup_destroy(group);
        return false;
    }
    //minfo("Creating spritegroup for entity %d at loc %d,%d", id, (int)loc.x, (int)loc.y);
    for (int i = 0; i < num_keys; i++) {
        //minfo("loading sprite for key %d", keys[i]);
        int k = keys[i];
        Texture2D* tex = &txinfo[k].texture;
        //sprite* s = sprite_create(tex, txinfo[k].contexts, txinfo[k].num_frames);
        //minfo("creating sprite...");
        shared_ptr<sprite> s = sprite_create2(tex, txinfo[k].contexts, txinfo[k].num_frames);
        //msuccess("created sprite!");
        // as expected lol
        // easy fix
        // boom, fixed
        //minfo("adding sprite to group...");
        spritegroup_add(group, s);
        //msuccess("added sprite to group!");
    }
    //msuccess("Created spritegroup with sprites");
    group->id = id;
    //sprite* s = spritegroup_get(group, 0);
    shared_ptr<sprite> s = spritegroup_get(group, 0);
    massert(s, "sprite is NULL");
    if (!s) {
        spritegroup_destroy(group);
        return false;
    }
    group->current = 0;

    group->dest = (Rectangle){(float)loc.x * DEFAULT_TILE_SIZE + offset_x, (float)loc.y * DEFAULT_TILE_SIZE + offset_y, (float)s->width, (float)s->height};

    group->off_x = offset_x;
    group->off_y = offset_y;

    //hashtable_entityid_spritegroup_insert(spritegroups, id, group);
    // how its done in the future...
    spritegroups[id] = group;
    return true;
}
