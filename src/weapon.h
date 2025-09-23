#pragma once


#include <string>


typedef enum
{
    WEAPON_NONE,
    WEAPON_DAGGER,
    WEAPON_SWORD,
    WEAPON_AXE,
    WEAPON_BOW,
    WEAPON_TWO_HANDED_SWORD,
    WEAPON_WARHAMMER,
    WEAPON_FLAIL,
    WEAPON_TYPE_COUNT
} weapontype_t;


// we need some weapontype-to-string functions
//static inline std::string weapontype2str(weapontype type) {
//    if (type == WEAPON_NONE) return "none";
//    if (type == WEAPON_DAGGER) return "dagger";
//    if (type == WEAPON_SWORD) return "sword";
//    if (type == WEAPON_AXE) return "axe";
//    if (type == WEAPON_BOW) return "bow";
//    if (type == WEAPON_TWO_HANDED_SWORD) return "two-handed sword";
//    if (type == WEAPON_WARHAMMER) return "warhammer";
//    if (type == WEAPON_FLAIL) return "flail";
//    return "unknown";
//}
