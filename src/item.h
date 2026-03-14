/** @file item.h
 *  @brief High-level item category identifiers used across inventory and factories.
 */

#pragma once

/// @brief Broad item categories carried by entity components and inventory logic.
typedef enum { ITEM_NONE, ITEM_WEAPON, ITEM_SHIELD, ITEM_POTION, ITEM_ARROW, ITEM_QUIVER, ITEM_WAND, ITEM_RING, ITEM_FOOD, ITEM_TYPE_COUNT } itemtype_t;
