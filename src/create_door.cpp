#include "create_door.h"

entityid create_door_at(shared_ptr<gamestate> g, vec3 loc) {
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

    entityid id = g_add_entity(g);

    g->ct.set<entitytype>(id, ENTITY_DOOR);
    g->ct.set<location>(id, loc);
    g->ct.set<door_open>(id, false);
    g->ct.set<update>(id, true);

    return df_add_at(df, id, loc.x, loc.y);
}
