/** @file shield.h
 *  @brief Shield subtype identifiers used by equipment and defense logic.
 */

#pragma once

/// @brief Supported shield subtypes for inventory and armor calculations.
typedef enum
{
    SHIELD_NONE,
    SHIELD_BUCKLER,
    SHIELD_KITE,
    SHIELD_TOWER,
    SHIELD_TYPE_COUNT
} shieldtype_t;
