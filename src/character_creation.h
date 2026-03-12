#pragma once

#include "alignment.h"
#include "race.h"
#include <string>

using std::string;

typedef struct character_creation_t {
    string name;

    race_t race;
    alignment_t alignment;

    int hitdie;
    int strength;
    int dexterity;
    int intelligence;
    int wisdom;
    int constitution;
    int charisma;

} character_creation;
