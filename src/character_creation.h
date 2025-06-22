#pragma once

#include "race.h"
#include <string>

using std::string;

typedef struct character_creation_t {
    //char name[1024];
    string name;

    race_t race;

    int hitdie;
    int strength;
    int constitution;
    int dexterity;

} character_creation;
