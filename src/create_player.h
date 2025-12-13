#pragma once

#include "ComponentTraits.h"
#include "create_npc.h"
#include "entitytype.h"
#include "gamestate.h"

static inline entityid create_player(shared_ptr<gamestate> g, vec3 loc, string n) {
    massert(g, "gamestate is NULL");
    massert(n != "", "name is empty string");

    minfo("Creating player...");
    race_t rt = g->chara_creation->race;
    minfo("Race: %s", race2str(rt).c_str());

    const auto id = create_npc_at_with(g, rt, loc, [n](shared_ptr<gamestate> g, entityid id) {
        const int hp_ = 10;
        const int maxhp_ = 10;
        const int vis_dist = 3;
        const int light_rad = 3;
        const int hear_dist = 3;
        const entitytype_t type = ENTITY_PLAYER;

        g_set_hero_id(g, id);
        g->ct.set<entitytype>(id, type);
        g->ct.set<txalpha>(id, 0);
        g->ct.set<hp>(id, hp_);
        g->ct.set<maxhp>(id, maxhp_);
        g->ct.set<vision_distance>(id, vis_dist);
        g->ct.set<light_radius>(id, light_rad);
        g->ct.set<hearing_distance>(id, hear_dist);
        g->ct.set<name>(id, n);

        minfo("Adding inventory to entity id %d", id);
        msuccess("create_player successful, id: %d", id);
    });

    return id;
}
