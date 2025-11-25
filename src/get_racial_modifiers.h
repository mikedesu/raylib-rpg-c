#pragma once

#include "race.h"

static inline int get_racial_modifiers(race_t r, int stat) {
    switch (r) {
    case RACE_HUMAN:
        if (stat >= 0 && stat < 6) return 0;
    case RACE_ELF:
        switch (stat) {
        case 0: return 0;
        case 1: return 2;
        case 2: return 0;
        case 3: return 0;
        case 4: return -2;
        case 5: return 0;
        default: return 0;
        }
    case RACE_DWARF:
        switch (stat) {
        case 0: return 0;
        case 1: return 0;
        case 2: return 0;
        case 3: return 0;
        case 4: return 2;
        case 5: return -2;
        default: return 0;
        }
        // other race cases below...
    default: break;
    }


    return 0;
}
