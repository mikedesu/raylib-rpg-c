#pragma once

#include "massert.h"
#include <string.h>
#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

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




const unordered_map<race_t, string> rn = {{RACE_NONE, "race_none"},
                                          {RACE_COUNT, "race_count"},
                                          {RACE_HUMAN, "human"},
                                          {RACE_HALFLING, "halfling"},
                                          {RACE_GOBLIN, "goblin"},
                                          {RACE_ELF, "elf"},
                                          {RACE_DWARF, "dwarf"},
                                          {RACE_ORC, "orc"},
                                          {RACE_GREEN_SLIME, "green slime"},
                                          {RACE_BAT, "bat"},
                                          {RACE_WOLF, "wolf"},
                                          {RACE_WARG, "warg"},
                                          {RACE_ZOMBIE, "zombie"},
                                          {RACE_SKELETON, "skeleton"},
                                          {RACE_RAT, "rat"}};




const static inline string race2str(const race_t r) {
    massert(r >= RACE_NONE && r < RACE_COUNT, "Invalid race name");
    massert(rn.find(r) != rn.end(), "race doesn't exist in map!: %d", r);
    return rn.at(r);
}


const static inline race_t str2race(const string str) {
    for (auto i : rn)
        if (str == i.second)
            return i.first;
    return RACE_NONE;
}
