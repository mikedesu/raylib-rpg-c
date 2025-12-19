#pragma once


typedef enum
{
    SPELLTYPE_NONE,
    SPELLTYPE_FIRE,
    SPELLTYPE_COUNT,
} spelltype_t;


typedef enum
{
    SPELLSTATE_NONE,
    SPELLSTATE_CAST,
    SPELLSTATE_PERSIST,
    SPELLSTATE_END,
    SPELLSTATE_COUNT,
} spellstate_t;
