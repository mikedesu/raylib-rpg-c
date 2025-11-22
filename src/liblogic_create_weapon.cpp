
#include "liblogic_create_weapon.h"

entityid create_weapon_at(shared_ptr<gamestate> g, vec3 loc, weapontype_t type) {
    minfo("potion create...");
    massert(g, "gamestate is NULL");

    shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, loc.z);

    minfo("calling df_tile_at...");

    shared_ptr<tile_t> tile = df_tile_at(df, loc);

    massert(tile, "failed to get tile");

    minfo("checking if tile is walkable...");

    if (!tile_is_walkable(tile->type)) {
        merror("cannot create entity on non-walkable tile");
        return ENTITYID_INVALID;
    }

    minfo("checking if tile has live NPCs...");

    if (tile_has_live_npcs(g, tile)) {
        merror("cannot create entity on tile with live NPCs");
        return ENTITYID_INVALID;
    }

    entityid id = g_add_entity(g);

    g->ct.set<entitytype>(id, ENTITY_ITEM);
    g->ct.set<itemtype>(id, ITEM_WEAPON);
    g->ct.set<weapontype>(id, type);

    string item_name = "unnamed-weapon";

    if (type == WEAPON_AXE) {
        item_name = "axe";
    } else if (type == WEAPON_DAGGER) {
        item_name = "dagger";
    } else if (type == WEAPON_SWORD) {
        item_name = "sword";
    }

    g->ct.set<name>(id, item_name);
    g->ct.set<location>(id, loc);
    g->ct.set<txalpha>(id, 255);
    g->ct.set<update>(id, true);
    g->ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});

    minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);

    if (!df_add_at(df, id, loc.x, loc.y)) {
        return ENTITYID_INVALID;
    }

    return ENTITYID_INVALID;
}

entityid create_weapon(shared_ptr<gamestate> g, weapontype_t type) {
    massert(g, "gamestate is NULL");

    entityid id = g_add_entity(g);

    g->ct.set<entitytype>(id, ENTITY_ITEM);
    g->ct.set<itemtype>(id, ITEM_WEAPON);
    g->ct.set<weapontype>(id, type);

    string item_name = "unnamed-weapon";

    if (type == WEAPON_AXE) {
        item_name = "axe";
    } else if (type == WEAPON_DAGGER) {
        item_name = "dagger";
    } else if (type == WEAPON_SWORD) {
        item_name = "sword";
    }

    g->ct.set<name>(id, item_name);
    g->ct.set<txalpha>(id, 255);
    g->ct.set<update>(id, true);
    g->ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});

    return id;
}
