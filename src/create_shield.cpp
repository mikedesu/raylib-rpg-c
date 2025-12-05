#include "ComponentTraits.h"
#include "create_shield.h"


entityid create_shield_with(shared_ptr<gamestate> g, function<void(entityid)> shieldInitFunction) {
    const auto id = g_add_entity(g);
    g->ct.set<entitytype>(id, ENTITY_ITEM);
    g->ct.set<itemtype>(id, ITEM_SHIELD);
    shieldInitFunction(id);
    return id;
}


entityid create_shield_at_with(shared_ptr<gamestate> g, vec3 loc, function<void(entityid)> shieldInitFunction) {
    const auto id = create_shield_with(g, shieldInitFunction);

    minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);

    auto df = d_get_floor(g->dungeon, loc.z);

    if (!df_add_at(df, id, loc.x, loc.y)) {
        return ENTITYID_INVALID;
    }

    g->ct.set<location>(id, loc);

    return id;
}
