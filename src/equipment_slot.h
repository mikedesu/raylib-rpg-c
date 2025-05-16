#pragma once
#define MAX_EQUIPMENT_SLOTS 2
typedef enum equipment_slot_t
{
    EQUIP_SLOT_WEAPON,
    EQUIP_SLOT_SHIELD,
    // add more equipment slots here
    //EQUIPMENT_SLOT_ARMOR,
    //EQUIPMENT_SLOT_HELMET,
    //EQUIPMENT_SLOT_RING,
    //EQUIPMENT_SLOT_AMULET,
    EQUIPMENT_SLOT_COUNT
} equipment_slot;
