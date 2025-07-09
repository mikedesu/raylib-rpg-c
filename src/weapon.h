#pragma once


#include <string>


typedef enum weapontype_t
{
    WEAPON_NONE,
    WEAPON_DAGGER,
    WEAPON_SWORD,
    WEAPON_AXE,
    //WEAPON_BOW,
    WEAPON_TYPE_COUNT
} weapontype;


// we need some weapontype-to-string functions
static inline std::string weapontype2str(weapontype type) {
    if (type == WEAPON_NONE) return "none";
    if (type == WEAPON_DAGGER) return "dagger";
    if (type == WEAPON_SWORD) return "sword";
    if (type == WEAPON_AXE) return "axe";
    //if (type == WEAPON_BOW) return "bow";
    return "unknown";
}
