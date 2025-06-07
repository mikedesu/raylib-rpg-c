#pragma once

#include "race.h"

typedef struct character_creation_t {
    char name[1024];

    race_t race;

    int hitdie;
    int strength;
    int constitution;
    int dexterity;

} character_creation;
