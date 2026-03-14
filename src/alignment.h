/** @file alignment.h
 *  @brief Alignment identifiers and cycling/string helpers.
 */

#pragma once

#include <string>

/// @brief Moral and ethical alignment values used by characters and NPCs.
typedef enum {
    ALIGNMENT_NONE = 0,
    ALIGNMENT_GOOD_LAWFUL,
    ALIGNMENT_GOOD_NEUTRAL,
    ALIGNMENT_GOOD_CHAOTIC,
    ALIGNMENT_NEUTRAL_LAWFUL,
    ALIGNMENT_NEUTRAL_NEUTRAL,
    ALIGNMENT_NEUTRAL_CHAOTIC,
    ALIGNMENT_EVIL_LAWFUL,
    ALIGNMENT_EVIL_NEUTRAL,
    ALIGNMENT_EVIL_CHAOTIC,
    ALIGNMENT_COUNT,
} alignment_t;

/** @brief Convert an alignment enum into a user-facing string. */
static inline std::string alignment_to_str(alignment_t alignment) {
    switch (alignment) {
    case ALIGNMENT_NONE: return "none";
    case ALIGNMENT_GOOD_LAWFUL: return "lawful good";
    case ALIGNMENT_GOOD_NEUTRAL: return "neutral good";
    case ALIGNMENT_GOOD_CHAOTIC: return "chaotic good";
    case ALIGNMENT_NEUTRAL_LAWFUL: return "lawful neutral";
    case ALIGNMENT_NEUTRAL_NEUTRAL: return "true neutral";
    case ALIGNMENT_NEUTRAL_CHAOTIC: return "chaotic neutral";
    case ALIGNMENT_EVIL_LAWFUL: return "lawful evil";
    case ALIGNMENT_EVIL_NEUTRAL: return "neutral evil";
    case ALIGNMENT_EVIL_CHAOTIC: return "chaotic evil";
    case ALIGNMENT_COUNT: return "alignment count";
    default: break;
    }
    return "invalid alignment";
}

/** @brief Return the next selectable non-none alignment, wrapping at the end. */
static inline alignment_t alignment_next(alignment_t alignment) {
    if (alignment < ALIGNMENT_GOOD_LAWFUL || alignment >= ALIGNMENT_EVIL_CHAOTIC) {
        return ALIGNMENT_GOOD_LAWFUL;
    }
    return static_cast<alignment_t>(alignment + 1);
}

/** @brief Return the previous selectable non-none alignment, wrapping at the start. */
static inline alignment_t alignment_prev(alignment_t alignment) {
    if (alignment <= ALIGNMENT_GOOD_LAWFUL || alignment >= ALIGNMENT_COUNT) {
        return ALIGNMENT_EVIL_CHAOTIC;
    }
    return static_cast<alignment_t>(alignment - 1);
}
