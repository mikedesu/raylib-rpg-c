#pragma once

typedef enum itemtype_t
{
    ITEM_NONE,
    ITEM_WEAPON,
    ITEM_SHIELD,
    ITEM_POTION,
    ITEM_TYPE_COUNT
} itemtype;

static inline const char* itemtype_to_string(itemtype type) {
    switch (type) {
    case ITEM_NONE: return "ITEM_NONE";
    case ITEM_WEAPON: return "ITEM_WEAPON";
    case ITEM_SHIELD: return "ITEM_SHIELD";
    case ITEM_POTION: return "ITEM_POTION";
    case ITEM_TYPE_COUNT: return "ITEM_TYPE_COUNT";
    default: return "UNKNOWN_ITEM_TYPE";
    }
}
