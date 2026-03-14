/** @file damagetype.h
 *  @brief Damage-type identifiers for combat calculations.
 */

#pragma once

/// @brief Supported high-level damage categories.
typedef enum {
    DAMAGE_NONE,
    DAMAGE_CUTTING,
    //DAMAGE_PIERCING,
    //DAMAGE_BLUNT,
    DAMAGE_COUNT

} damagetype_t;
