#include "ComponentTraits.h"
#include "liblogic_create_shield.h"
#include "shield.h"

entityid create_shield_at(shared_ptr<gamestate> g, vec3 loc, shieldtype_t type) {
    minfo("shield create at location...");
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

    entityid id = create_shield(g, type);
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


entityid create_shield(shared_ptr<gamestate> g, shieldtype_t type) {
    massert(g, "gamestate is NULL");
    minfo("BEGIN create_shield");

    entityid id = g_add_entity(g);

    g->ct.set<entitytype>(id, ENTITY_ITEM);
    g->ct.set<itemtype>(id, ITEM_SHIELD);
    g->ct.set<shieldtype>(id, type);

    string item_name = "unnamed-shield";
    string desc = "no-description";

    if (type == SHIELD_BUCKLER) {
        item_name = "buckler";
        desc = "Your basic buckler.";
    } else if (type == SHIELD_KITE) {
        item_name = "kite shield";
        desc = "Standard issue.";
    } else if (type == SHIELD_TOWER) {
        item_name = "tower shield";
        desc = "It towers over you.";
    }

    g->ct.set<name>(id, item_name);
    g->ct.set<description>(id, desc);
    g->ct.set<txalpha>(id, 255);
    g->ct.set<update>(id, true);
    g->ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});

    minfo("END create_shield");
    return id;
}
