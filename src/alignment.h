#pragma once

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

// Functions that will return string representations of each alignment
// i.e. ALIGNMENT_EVIL_LAWFUL returns "lawful evil"
// ALIGNMENT_NEUTRAL_NEUTRAL returns "true neutral"
