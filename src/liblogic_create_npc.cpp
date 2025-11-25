#include "ComponentTraits.h"
#include "liblogic_create_npc.h"

entityid create_npc(shared_ptr<gamestate> g, race_t rt, vec3 loc, const string n) {
    minfo("begin create npc");
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

    entityid id = g_add_entity(g);

    g->ct.set<name>(id, n);
    g->ct.set<entitytype>(id, ENTITY_NPC);
    g->ct.set<race>(id, rt);
    g->ct.set<location>(id, loc);
    g->ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
    g->ct.set<dead>(id, false);
    g->ct.set<update>(id, true);
    g->ct.set<direction>(id, DIR_DOWN_RIGHT);
    g->ct.set<attacking>(id, false);
    g->ct.set<blocking>(id, false);
    g->ct.set<blocksuccess>(id, false);
    g->ct.set<damaged>(id, false);
    g->ct.set<txalpha>(id, 0);
    g->ct.set<inventory>(id, make_shared<vector<entityid>>());
    g->ct.set<equipped_weapon>(id, ENTITYID_INVALID);
    g->ct.set<aggro>(id, false);

    // here we have some hard decisions to make about how to template-out NPC creation
    // all NPCs begin at level 1. level-up mechanisms will be determined elsewhere
    g->ct.set<level>(id, 1);
    g->ct.set<xp>(id, 0);

    // default to 10 for stats
    // later, we will decide this by race templating
    g->ct.set<strength>(id, 10);
    g->ct.set<dexterity>(id, 10);
    g->ct.set<intelligence>(id, 10);
    g->ct.set<wisdom>(id, 10);
    g->ct.set<constitution>(id, 10);
    g->ct.set<charisma>(id, 10);

    // set default hp/maxhp for now
    // later, we will decide this by race templating
    const int my_maxhp = 3;
    const int my_hp = my_maxhp;

    g->ct.set<maxhp>(id, my_maxhp);
    g->ct.set<hp>(id, my_hp);


    minfo("end create npc");

    return df_add_at(df, id, loc.x, loc.y);

    //g_add_zapping(g, id, false);
    //g_add_default_action(g, id, ENTITY_ACTION_WAIT);
    //g_add_inventory(g, id);
    //g_add_target(g, id, (vec3){-1, -1, -1});
    //g_add_target_path(g, id);
    //g_add_equipment(g, id);
    //g_add_base_attack_damage(g, id, (vec3){1, 4, 0});
    //g_add_vision_distance(g, id, 0);
    //g_add_light_radius(g, id, 0);
    //g_add_stats(g, id);
    //g_set_stat(g, id, STATS_LEVEL, 1);
    //g_set_stat(g, id, STATS_XP, 0);
    //g_set_stat(g, id, STATS_NEXT_LEVEL_XP, calc_next_lvl_xp(g, id));
    //g_set_stat(g, id, STATS_MAXHP, 1);
    //g_set_stat(g, id, STATS_HP, 1);
    //g_set_stat(g, id, STATS_HITDIE, 1);
    //g_set_stat(g, id, STATS_STR, 10);
    //g_set_stat(g, id, STATS_DEX, 10);
    //g_set_stat(g, id, STATS_CON, 10);
    //g_set_stat(g, id, STATS_ATTACK_BONUS, 0);
    //g_set_stat(g, id, STATS_AC, 10);
}
