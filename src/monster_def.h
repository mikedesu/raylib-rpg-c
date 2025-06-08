#pragma once

//#include "entityid.h"
#include "race.h"
#include "stats_slot.h"
#include "vec3.h"
typedef struct monster_def_t {
    //entityid id;
    race_t race;
    vec3 hitdie;
    int stats[MAX_STATS_SLOTS];
} monster_def;

// example hard-coded definition
//static inline monster_def human_def() {
//    monster_def m;
//    m.race = RACE_HUMAN;
//    m.hitdie = (vec3){1, 8, 0};
//    m.stats[STATS_LEVEL] = 1;
//    m.stats[STATS_XP] = 0;
//    m.stats[STATS_NEXT_LEVEL_XP] = 1000;
//    m.stats[STATS_HITDIE] = 8;
//    m.stats[STATS_HP] = 10;
//    m.stats[STATS_MAXHP] = 10;
//    m.stats[STATS_AC] = 10;
//    m.stats[STATS_STR] = 10;
//    m.stats[STATS_CON] = 10;
//    m.stats[STATS_DEX] = 10;
//    m.stats[STATS_ATTACK_BONUS] = 0;
//    return m;
//}
