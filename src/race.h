#pragma once

#include "massert.h"
#include <string.h>
#include <string>

using std::string;

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
    RACE_ZOMBIE,
    RACE_SKELETON,
    RACE_RAT,
    RACE_COUNT
} race_t;

static inline string race2str(race_t race) {
    string race_strings[RACE_COUNT] = {
        "none", "halfling", "goblin", "human", "elf", "dwarf", "orc", "green_slime", "bat", "wolf", "warg", "zombie", "skeleton", "rat"};
    massert(race >= RACE_NONE && race < RACE_COUNT, "Invalid race name");
    return race_strings[race];
}

// for lowercase names found in monsters.csv
static inline race_t str2race(string str) {
    //string names[11] = {"none", "halfling", "goblin", "human", "elf", "dwarf", "orc", "green_slime", "bat", "wolf", "warg"};
    string names[RACE_COUNT] = {
        "none", "halfling", "goblin", "human", "elf", "dwarf", "orc", "green_slime", "bat", "wolf", "warg", "zombie", "skeleton", "rat"};
    const int num_races = sizeof(names) / sizeof(names[0]);
    for (int i = 0; i < num_races; i++) {
        string name = names[i];
        if (str == name) {
            return (race_t)i;
        }
    }
    return RACE_NONE;
}
