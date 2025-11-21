#include "liblogic_create_npc.h"
#include "liblogic_create_npc_set_stats.h"

entityid create_npc_set_stats(shared_ptr<gamestate> g, vec3 loc, race_t race) {
    //minfo("npc_create_set_stats: %d,%d,%d %d", loc.x, loc.y, loc.z, race);
    entityid id = ENTITYID_INVALID;
    string race_name = race2str(race);
    id = create_npc(g, race, loc, race_name);
    if (id != ENTITYID_INVALID) {
        // Set stats
        const int tmphp = 5;
        g->ct.set<hp>(id, tmphp);
        g->ct.set<maxhp>(id, tmphp);

        // for now, if it is for sure not the hero, then we target the hero
        if (id != g->hero_id) {
            g->ct.set<target>(id, g->hero_id);
        }
    }
    return id;
}
