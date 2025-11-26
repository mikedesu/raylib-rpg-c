#include "ComponentTraits.h"
#include "liblogic_create_weapon.h"

entityid create_weapon_at(shared_ptr<gamestate> g, vec3 loc, weapontype_t type) {
    minfo("weapon create at location...");
    massert(g, "gamestate is NULL");

    shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, loc.z);
    shared_ptr<tile_t> tile = df_tile_at(df, loc);
    massert(tile, "failed to get tile");

    if (!tile_is_walkable(tile->type)) {
        merror("cannot create entity on non-walkable tile");
        return ENTITYID_INVALID;
    }

    if (tile_has_live_npcs(g, tile)) {
        merror("cannot create entity on tile with live NPCs");
        return ENTITYID_INVALID;
    }

    entityid id = create_weapon(g, type);
    if (id == ENTITYID_INVALID) {
        return ENTITYID_INVALID;
    }


    minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
    if (!df_add_at(df, id, loc.x, loc.y)) {
        return ENTITYID_INVALID;
    }

    g->ct.set<location>(id, loc);

    return id;
}


entityid create_weapon(shared_ptr<gamestate> g, weapontype_t type) {
    massert(g, "gamestate is NULL");
    minfo("BEGIN create_weapon");

    entityid id = g_add_entity(g);

    g->ct.set<entitytype>(id, ENTITY_ITEM);
    g->ct.set<itemtype>(id, ITEM_WEAPON);
    g->ct.set<weapontype>(id, type);

    string item_name = "unnamed-weapon";
    vec3 dmg = {1, 4, 0};
    string desc = "no description";
    int dura = 100;
    int max_dura = 100;

    if (type == WEAPON_AXE) {
        item_name = "axe";
        dmg = {1, 8, 0};
        desc = "Normally used to chop wood.";
    } else if (type == WEAPON_DAGGER) {
        item_name = "dagger";
        dmg = {1, 4, 0};
        desc = "A small, sharp dagger.";
    } else if (type == WEAPON_SWORD) {
        item_name = "sword";
        dmg = {1, 6, 0};
        desc = "The G.O.A.T.";
    }

    g->ct.set<name>(id, item_name);
    g->ct.set<description>(id, desc);
    g->ct.set<damage>(id, dmg);

    g->ct.set<durability>(id, dura);
    g->ct.set<max_durability>(id, max_dura);

    g->ct.set<txalpha>(id, 255);
    g->ct.set<update>(id, true);
    g->ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});


    minfo("END create_weapon");
    return id;
}
