#pragma once

typedef enum
{
    ENTITY_NONE,
    ENTITY_PLAYER,
    ENTITY_NPC,
    ENTITY_ITEM,
    ENTITY_DOOR,
    ENTITY_SPELL_EFFECT,
    ENTITY_TYPE_COUNT
} entitytype_t;

static inline const char* entitytype_to_string(entitytype_t type) {
    switch (type) {
    case ENTITY_NONE: return "ENTITY_NONE";
    case ENTITY_PLAYER: return "ENTITY_PLAYER";
    case ENTITY_NPC: return "ENTITY_NPC";
    case ENTITY_ITEM: return "ENTITY_ITEM";
    case ENTITY_DOOR: return "ENTITY_DOOR";
    case ENTITY_SPELL_EFFECT: return "ENTITY_SPELL_EFFECT";
    default: return "UNKNOWN_ENTITY_TYPE";
    }
}
