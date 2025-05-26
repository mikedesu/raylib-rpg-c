#pragma once
typedef enum equipment_slot_t
{
    EQUIP_SLOT_WEAPON,
    EQUIP_SLOT_SHIELD,
    EQUIP_SLOT_WAND,
    // add more equipment slots here
    //EQUIPMENT_SLOT_ARMOR,
    //EQUIPMENT_SLOT_HELMET,
    //EQUIPMENT_SLOT_RING,
    //EQUIPMENT_SLOT_AMULET,
    EQUIPMENT_SLOT_COUNT
} equipment_slot;
#define MAX_EQUIPMENT_SLOTS EQUIPMENT_SLOT_COUNT
