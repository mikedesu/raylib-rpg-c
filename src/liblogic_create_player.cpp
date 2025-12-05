#include "ComponentTraits.h"
#include "create_npc.h"
#include "liblogic_create_player.h"

entityid create_player(shared_ptr<gamestate> g, vec3 loc, string name) {
    massert(g, "gamestate is NULL");
    massert(name != "", "name is empty string");
    // use the previously-written liblogic_npc_create function
    minfo("Creating player...");
    race_t rt = g->chara_creation->race;
    minfo("Race: %s", race2str(rt).c_str());

    entityid id = create_npc(g, rt, loc, name);
    massert(id != ENTITYID_INVALID, "failed to create player");
    msuccess("create_npc successful, id: %d", id);

    g_set_hero_id(g, id);

    //g_add_type(g, id, ENTITY_PLAYER);
    g->ct.set<entitytype>(id, ENTITY_PLAYER);

    //g_set_tx_alpha(g, id, 0);
    g->ct.set<txalpha>(id, 0);
    g->ct.set<hp>(id, 100);
    g->ct.set<maxhp>(id, 100);

    g->ct.set<vision_distance>(id, 3);
    g->ct.set<light_radius>(id, 3);

    //g_set_stat(g, id, STATS_LEVEL, 666);
    //g_add_equipped_weapon(g, id, ENTITYID_INVALID);

    minfo("Adding inventory to entity id %d", id);
    //g_add_inventory(g, id);
    msuccess("create_player successful, id: %d", id);
    return id;
}
