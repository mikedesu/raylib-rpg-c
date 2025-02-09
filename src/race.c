#include "race.h"


// we need a mapping of race_t to string so we can get a nice string representing a race
// example: RACE_HUMAN -> "Human"
// RACE_NONE -> "None"
// RACE_ORC -> "Orc"
//


const char* race_strings[RACE_COUNT] = {
    "None",
    "Human",
    "Orc",
};
