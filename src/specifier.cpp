#include "specifier.h"

const char* specifier_get_str(specifier_t spec) {
    switch (spec) {
    case SPECIFIER_NONE: return "SPECIFIER_NONE";
    case SPECIFIER_SHIELD_ON_TILE: return "SPECIFIER_SHIELD_ON_TILE";
    case SPECIFIER_SHIELD_GUARD_FRONT: return "SPECIFIER_SHIELD_GUARD_FRONT";
    case SPECIFIER_SHIELD_GUARD_BACK: return "SPECIFIER_SHIELD_GUARD_BACK";
    //case SPECIFIER_SHIELD_GUARD_SUCCESS: return "SPECIFIER_SHIELD_GUARD_SUCCESS";
    case SPECIFIER_COUNT: return "SPECIFIER_COUNT";
    }
    return "SPECIFIER_UNKNOWN";
}
