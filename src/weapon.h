/** @file weapon.h
 *  @brief Weapon subtype identifiers used by item entities and combat logic.
 */

#pragma once

/// @brief Supported weapon subtypes for equipment and combat calculations.
typedef enum {
    WEAPON_NONE,
    WEAPON_DAGGER,
    WEAPON_SHORT_SWORD,
    WEAPON_LONG_SWORD,
    WEAPON_GREAT_SWORD,
    WEAPON_AXE,
    WEAPON_SPEAR,
    //WEAPON_BOW,
    //WEAPON_TWO_HANDED_SWORD,
    //WEAPON_WARHAMMER,
    //WEAPON_FLAIL,
    WEAPON_TYPE_COUNT
} weapontype_t;
