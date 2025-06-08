#pragma once

#include "massert.h"
#include <string.h>

typedef enum
{
    RACE_NONE = 0,
    RACE_HALFLING,
    RACE_GOBLIN,
    RACE_HUMAN,
    RACE_ELF,
    RACE_DWARF,
    RACE_ORC,
    RACE_GREEN_SLIME,
    RACE_BAT,
    RACE_WOLF,
    RACE_WARG,
    RACE_COUNT
} race_t;

static inline const char* get_race_str(race_t race) {
    //const char* race_strings[RACE_COUNT] = {"None", "Green Slime", "Bat", "Goblin", "Halfling", "Wolf", "Human", "Elf", "Dwarf", "Orc", "Warg"};
    const char* race_strings[RACE_COUNT] = {"none", "halfling", "goblin", "human", "elf", "dwarf", "orc", "green_slime", "bat", "wolf", "warg"};
    massert(race >= RACE_NONE && race < RACE_COUNT, "Invalid race name");
    return race_strings[race];
}

// for lowercase names found in monsters.csv
static inline race_t str2race(const char* str) {
    const char* names[11] = {"none", "halfling", "goblin", "human", "elf", "dwarf", "orc", "green_slime", "bat", "wolf", "warg"};
    const int num_races = sizeof(names) / sizeof(names[0]);
    for (int i = 0; i < num_races; i++) {
        const char* name = names[i];
        int name_len = strlen(name);
        if (!strncmp(str, name, name_len)) return (race_t)i;
    }
    return RACE_NONE;
}
