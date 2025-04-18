#pragma once

typedef enum {
    ENTITY_NONE,
    ENTITY_PLAYER,
    ENTITY_NPC,
    ENTITY_WEAPON,
    ENTITY_SHIELD,
    ENTITY_ITEM,
    ENTITY_TYPE_COUNT
} entitytype_t;

static inline const char* entitytype_to_string(entitytype_t type) {
    switch (type) {
    case ENTITY_NONE:
        return "ENTITY_NONE";
    case ENTITY_PLAYER:
        return "ENTITY_PLAYER";
    case ENTITY_NPC:
        return "ENTITY_NPC";
    case ENTITY_WEAPON:
        return "ENTITY_WEAPON";
    case ENTITY_SHIELD:
        return "ENTITY_SHIELD";
    case ENTITY_ITEM:
        return "ENTITY_ITEM";
    default:
        return "UNKNOWN_ENTITY_TYPE";
    }
}
