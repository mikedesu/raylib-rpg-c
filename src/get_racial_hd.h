#pragma once


#include "race.h"
static inline const int get_racial_hd(race_t rt) {
    switch (rt) {
    case RACE_HUMAN: return 8;
    case RACE_ELF: return 6;
    case RACE_DWARF: return 10;
    default: break;
    }
    return 8;
}
