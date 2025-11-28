#include "ComponentTraits.h"
#include "get_racial_modifiers.h"
#include "liblogic_create_npc.h"
#include "liblogic_stat_bonus.h"
#include "race.h"
#include <raylib.h>

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
    g->ct.set<block_success>(id, false);
    g->ct.set<damaged>(id, false);
    g->ct.set<txalpha>(id, 0);
    g->ct.set<inventory>(id, make_shared<vector<entityid>>());
    g->ct.set<equipped_weapon>(id, ENTITYID_INVALID);
    g->ct.set<aggro>(id, false);

    // here we have some hard decisions to make about how to template-out NPC creation
    // all NPCs begin at level 1. level-up mechanisms will be determined elsewhere
    g->ct.set<level>(id, 1);
    g->ct.set<xp>(id, 0);


    // stats racial modifiers for stats
    int str_m = get_racial_modifiers(rt, 0);
    int dex_m = get_racial_modifiers(rt, 1);
    int int_m = get_racial_modifiers(rt, 2);
    int wis_m = get_racial_modifiers(rt, 3);
    int con_m = get_racial_modifiers(rt, 4);
    int cha_m = get_racial_modifiers(rt, 5);

    // default to 3-18 for stats
    const int strength_ = GetRandomValue(3, 18) + str_m;
    const int dexterity_ = GetRandomValue(3, 18) + dex_m;
    const int intelligence_ = GetRandomValue(3, 18) + int_m;
    const int wisdom_ = GetRandomValue(3, 18) + wis_m;
    const int constitution_ = GetRandomValue(3, 18) + con_m;
    const int charisma_ = GetRandomValue(3, 18) + cha_m;

    g->ct.set<strength>(id, strength_);
    g->ct.set<dexterity>(id, dexterity_);
    g->ct.set<intelligence>(id, intelligence_);
    g->ct.set<wisdom>(id, wisdom_);
    g->ct.set<constitution>(id, constitution_);
    g->ct.set<charisma>(id, charisma_);

    // set default hp/maxhp for now
    // later, we will decide this by race templating
    vec3 hitdie = {1, 8, 0};
    switch (rt) {
    case RACE_HUMAN: hitdie.y = 8; break;
    case RACE_ELF: hitdie.y = 6; break;
    case RACE_DWARF: hitdie.y = 10; break;
    case RACE_HALFLING: hitdie.y = 6; break;
    case RACE_GOBLIN: hitdie.y = 6; break;
    case RACE_ORC: hitdie.y = 8; break;
    case RACE_BAT: hitdie.y = 3; break;
    case RACE_GREEN_SLIME: hitdie.y = 4; break;
    case RACE_WOLF: hitdie.y = 6; break;
    case RACE_WARG: hitdie.y = 12; break;
    default: break;
    }

    const int my_maxhp = GetRandomValue(1, hitdie.y) + get_stat_bonus(constitution_);
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
