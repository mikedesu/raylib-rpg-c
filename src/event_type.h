/** @file event_type.h
 *  @brief Event categories for animation, combat, and inventory actions.
 */

#pragma once

/// @brief High-level event types that can describe gameplay actions.
typedef enum {
    EVENT_NONE, // No event
    EVENT_MOVE, // Walking to a new tile
    EVENT_ATTACK, // Attacking a target
    EVENT_BLOCK, // Blocking an attack
    EVENT_DODGE, // Dodging an attack (included from your list)
    EVENT_EQUIP_SWORD, // Equipping a sword (manual or auto)
    EVENT_EQUIP_SHIELD, // Equipping a shield
    EVENT_UNEQUIP_SWORD, // Unequipping a sword
    EVENT_USE_ITEM, // Using an item
    EVENT_PICKUP_ITEM, // Picking up a sword (or other item)
    EVENT_DAMAGE, // Receiving damage from an attack
    EVENT_COUNT // More event types can be added later, e.g., EVENT_DROP_ITEM
} event_type_t;
