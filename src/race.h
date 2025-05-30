#pragma once

#include "massert.h"

typedef enum
{
    RACE_NONE = 0,
    RACE_GREEN_SLIME,
    RACE_BAT,
    RACE_GOBLIN,
    RACE_HALFLING,
    RACE_WOLF,
    RACE_HUMAN,
    RACE_ELF,
    RACE_DWARF,
    RACE_ORC,
    RACE_WARG,
    RACE_COUNT
} race_t;

static inline const char* get_race_str(race_t race) {
    const char* race_strings[RACE_COUNT] = {"None", "Green Slime", "Bat", "Goblin", "Halfling", "Wolf", "Human", "Elf", "Dwarf", "Orc", "Warg"};
    massert(race >= RACE_NONE && race < RACE_COUNT, "Invalid race name");
    return race_strings[race];
}
