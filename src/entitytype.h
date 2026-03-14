/** @file entitytype.h
 *  @brief High-level entity category identifiers and label helpers.
 */

#pragma once

#include <string>

/// @brief Broad categories used to describe runtime entities.
typedef enum { ENTITY_NONE, ENTITY_PLAYER, ENTITY_NPC, ENTITY_ITEM, ENTITY_BOX, ENTITY_CHEST, ENTITY_DOOR, ENTITY_PROP, ENTITY_TYPE_COUNT } entitytype_t;

/** @brief Convert an entity category into a lowercase display/debug string. */
static inline std::string entitytype_to_str(entitytype_t t) {
    switch (t) {
    case ENTITY_NONE: return "none";
    case ENTITY_PLAYER: return "player";
    case ENTITY_NPC: return "npc";
    case ENTITY_ITEM: return "item";
    case ENTITY_BOX: return "box";
    case ENTITY_CHEST: return "chest";
    case ENTITY_DOOR: return "door";
    case ENTITY_PROP: return "prop";
    case ENTITY_TYPE_COUNT: return "entity type count";
    default: break;
    }
    return "errortype";
}
