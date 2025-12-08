#include "ComponentTraits.h"
#include "create_weapon.h"

//entityid create_weapon_with(shared_ptr<gamestate> g, function<void(entityid)> weaponInitFunction) {
entityid create_weapon_with(shared_ptr<gamestate> g, function<void(shared_ptr<gamestate>, entityid)> weaponInitFunction) {
    massert(g, "gamestate is NULL");

    const auto id = g_add_entity(g);
    g->ct.set<entitytype>(id, ENTITY_ITEM);
    g->ct.set<itemtype>(id, ITEM_WEAPON);

    weaponInitFunction(g, id);
    return id;
}

//entityid create_weapon_at_with(shared_ptr<gamestate> g, vec3 loc, function<void(entityid)> weaponInitFunction) {
entityid create_weapon_at_with(shared_ptr<gamestate> g, vec3 loc, function<void(shared_ptr<gamestate>, entityid)> weaponInitFunction) {
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

    const auto id = create_weapon_with(g, weaponInitFunction);
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
