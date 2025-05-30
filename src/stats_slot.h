#pragma once

typedef enum stats_slot_t
{
    STATS_NONE,
    STATS_LEVEL,
    STATS_XP,
    STATS_NEXT_LEVEL_XP,
    STATS_HITDIE,
    STATS_HP,
    STATS_MAXHP,
    STATS_AC,
    STATS_STR,
    STATS_CON,
    STATS_DEX,
    STATS_ATTACK_BONUS,
    //STATS_INT,
    //STATS_WIS,
    //STATS_CHA,
    STATS_COUNT
} stats_slot;

#define MAX_STATS_SLOTS STATS_COUNT
