#pragma once

#include <string>
typedef enum { ENTITY_NONE, ENTITY_PLAYER, ENTITY_NPC, ENTITY_ITEM, ENTITY_BOX, ENTITY_DOOR, ENTITY_SPELL, ENTITY_PROP, ENTITY_TYPE_COUNT } entitytype_t;

static inline std::string entitytype_to_str(entitytype_t t) {
    switch (t) {
    case ENTITY_NONE: return "none";
    case ENTITY_PLAYER: return "player";
    case ENTITY_NPC: return "npc";
    case ENTITY_ITEM: return "item";
    case ENTITY_BOX: return "box";
    case ENTITY_DOOR: return "door";
    case ENTITY_SPELL: return "spell";
    case ENTITY_PROP: return "prop";
    case ENTITY_TYPE_COUNT: return "entity type count";
    default: break;
    }
    return "errortype";
}
