#include "ComponentTraits.h"
#include "create_npc.h"
#include "liblogic_create_player.h"

entityid create_player(shared_ptr<gamestate> g, vec3 loc, string n) {
    massert(g, "gamestate is NULL");
    massert(n != "", "name is empty string");

    minfo("Creating player...");
    race_t rt = g->chara_creation->race;
    minfo("Race: %s", race2str(rt).c_str());

    return create_npc_at_with(g, rt, loc, [&](entityid id) {
        g_set_hero_id(g, id);
        g->ct.set<entitytype>(id, ENTITY_PLAYER);
        g->ct.set<txalpha>(id, 0);
        g->ct.set<hp>(id, 100);
        g->ct.set<maxhp>(id, 100);
        g->ct.set<vision_distance>(id, 3);
        g->ct.set<light_radius>(id, 3);

        g->ct.set<name>(id, n);

        minfo("Adding inventory to entity id %d", id);
        msuccess("create_player successful, id: %d", id);
    });
}
