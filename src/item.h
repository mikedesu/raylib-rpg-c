#pragma once

typedef enum itemtype_t
{
    ITEM_NONE,
    ITEM_WEAPON,
    ITEM_SHIELD,
    ITEM_POTION,
    ITEM_ARROW,
    ITEM_QUIVER,
    ITEM_WAND,
    ITEM_RING,
    ITEM_TYPE_COUNT
} itemtype;

static inline const char* itemtype_to_string(itemtype type) {
    switch (type) {
    case ITEM_NONE: return "NONE";
    case ITEM_WEAPON: return "WEAPON";
    case ITEM_SHIELD: return "SHIELD";
    case ITEM_POTION: return "POTION";
    case ITEM_ARROW: return "ARROW";
    case ITEM_QUIVER: return "QUIVER";
    case ITEM_WAND: return "WAND";
    case ITEM_TYPE_COUNT: return "ITEM_TYPE_COUNT";
    default: return "UNKNOWN_ITEM_TYPE";
    }
}
