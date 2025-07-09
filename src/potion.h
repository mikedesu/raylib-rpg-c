#pragma once


#include <string>


typedef enum potion_type_t
{
    POTION_NONE,
    POTION_HP_SMALL,
    POTION_HP_MEDIUM,
    POTION_HP_LARGE,
    POTION_MP_SMALL,
    POTION_MP_MEDIUM,
    POTION_MP_LARGE,
    POTION_TYPE_COUNT
} potiontype;


// we need some potiontype-to-string functions
static inline std::string potiontype2str(potiontype type) {
    if (type == POTION_NONE) return "none";
    if (type == POTION_HP_SMALL) return "small hp potion";
    if (type == POTION_HP_MEDIUM) return "medium hp potion";
    if (type == POTION_HP_LARGE) return "large hp potion";
    if (type == POTION_MP_SMALL) return "small mp potion";
    if (type == POTION_MP_MEDIUM) return "medium mp potion";
    if (type == POTION_MP_LARGE) return "large mp potion";
    return "unknown";
}
