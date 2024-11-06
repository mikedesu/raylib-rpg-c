#include "specifier.h"

const char* specifier_get_str(const specifier_t spec) {
    switch (spec) {
    case SPECIFIER_NONE: return "SPECIFIER_NONE";
    case SPECIFIER_SHIELD_ON_TILE: return "SPECIFIER_SHIELD_ON_TILE";
    case SPECIFIER_SHIELD_BLOCK: return "SPECIFIER_SHIELD_BLOCK";
    case SPECIFIER_SHIELD_BLOCK_SUCCESS: return "SPECIFIER_SHIELD_BLOCK_SUCCESS";
    case SPECIFIER_COUNT: return "SPECIFIER_COUNT";
    }
    return "SPECIFIER_UNKNOWN";
}
