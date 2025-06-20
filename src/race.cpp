#include "massert.h"
#include "race.h"

// we need a mapping of race_t to string so we can get a nice string representing a race
// example: RACE_HUMAN -> "Human"
// RACE_NONE -> "None"
// RACE_ORC -> "Orc"
//

//const char* race_strings[RACE_COUNT] = {"None", "Human", "Orc", "Elf", "Dwarf", "Halfling", "Goblin"};

//typedef enum
//{
//    RACE_NONE = 0,
//    RACE_GREEN_SLIME,
//    RACE_BAT,
//    RACE_GOBLIN,
//    RACE_HALFLING,
//    RACE_WOLF,
//    RACE_HUMAN,
//    RACE_ELF,
//    RACE_DWARF,
//    RACE_ORC,
//    RACE_WARG,
//    RACE_COUNT
//} race_t;
