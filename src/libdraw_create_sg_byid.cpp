#include "libdraw_create_sg_byid.h"
#include "libdraw_create_spritegroup.h"
#include "tx_keys_boxes.h"
#include "tx_keys_doors.h"
#include "tx_keys_monsters.h"
#include "tx_keys_npcs.h"
#include "tx_keys_potions.h"
#include "tx_keys_shields.h"
#include "tx_keys_weapons.h"

void create_sg_byid(shared_ptr<gamestate> g, entityid id) {
    minfo("BEGIN create_sg_byid");
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    //entitytype_t type = g_get_type(g, id);

    minfo("checking type for id %d", id);
    entitytype_t type = g->ct.get<entitytype>(id).value_or(ENTITY_NONE);
    minfo("type for id %d is %d", id, type);
    massert(type != ENTITY_NONE, "entity type is none");
    if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
        minfo("type is player");
        //race_t race = g_get_race(g, id);
        race_t r = g->ct.get<race>(id).value_or(RACE_NONE);
        switch (r) {
        case RACE_HUMAN: create_spritegroup(g, id, TX_HUMAN_KEYS, TX_HUMAN_COUNT, -12, -12); break;
        case RACE_ORC: create_spritegroup(g, id, TX_ORC_KEYS, TX_ORC_COUNT, -12, -12); break;
        case RACE_ELF: create_spritegroup(g, id, TX_ELF_KEYS, TX_ELF_COUNT, -12, -12); break;
        case RACE_DWARF: create_spritegroup(g, id, TX_DWARF_KEYS, TX_DWARF_COUNT, -12, -12); break;
        case RACE_HALFLING: create_spritegroup(g, id, TX_HALFLING_KEYS, TX_HALFLING_COUNT, -12, -12); break;
        case RACE_GOBLIN: create_spritegroup(g, id, TX_GOBLIN_KEYS, TX_GOBLIN_COUNT, -12, -12); break;
        case RACE_WOLF: create_spritegroup(g, id, TX_WOLF_KEYS, TX_WOLF_COUNT, -12, -12); break;
        case RACE_BAT: create_spritegroup(g, id, TX_BAT_KEYS, TX_BAT_COUNT, -12, -12); break;
        case RACE_WARG: create_spritegroup(g, id, TX_WARG_KEYS, TX_WARG_COUNT, -12, -12); break;
        case RACE_GREEN_SLIME: create_spritegroup(g, id, TX_GREEN_SLIME_KEYS, TX_GREEN_SLIME_COUNT, -12, -12); break;
        default: merror("unknown race %d", r);
        }
    }
    // doors
    else if (type == ENTITY_DOOR) {
        minfo("type is door");
        create_spritegroup(g, id, TX_WOODEN_DOOR_KEYS, TX_WOODEN_DOOR_COUNT, -12, -12);
    }
    // wooden box
    else if (type == ENTITY_WOODEN_BOX) {
        minfo("type is wooden box");
        create_spritegroup(g, id, TX_WOODEN_BOX_KEYS, TX_WOODEN_BOX_COUNT, -12, -12);
    }

    // items
    else if (type == ENTITY_ITEM) {
        minfo("type is item");
        //itemtype item_type = g_get_item_type(g, id);
        itemtype_t item_type = g->ct.get<itemtype>(id).value_or(ITEM_NONE);

        if (item_type == ITEM_POTION) {
            minfo("itemtype is potion");
            //potiontype_t potion_type = g_get_potion_type(g, id);
            potiontype_t potion_type = g->ct.get<potiontype>(id).value_or(POTION_NONE);

            switch (potion_type) {
            case POTION_HP_SMALL: create_spritegroup(g, id, TX_POTION_HP_SMALL_KEYS, TX_POTION_HP_SMALL_COUNT, -12, -12); break;
            case POTION_HP_MEDIUM: create_spritegroup(g, id, TX_POTION_HP_MEDIUM_KEYS, TX_POTION_HP_MEDIUM_COUNT, -12, -12); break;
            case POTION_HP_LARGE: create_spritegroup(g, id, TX_POTION_HP_LARGE_KEYS, TX_POTION_HP_LARGE_COUNT, -12, -12); break;
            case POTION_MP_SMALL: create_spritegroup(g, id, TX_POTION_MP_SMALL_KEYS, TX_POTION_MP_SMALL_COUNT, -12, -12); break;
            case POTION_MP_MEDIUM: create_spritegroup(g, id, TX_POTION_MP_MEDIUM_KEYS, TX_POTION_MP_MEDIUM_COUNT, -12, -12); break;
            case POTION_MP_LARGE: create_spritegroup(g, id, TX_POTION_MP_LARGE_KEYS, TX_POTION_MP_LARGE_COUNT, -12, -12); break;
            default: merror("unknown potion type %d", potion_type); return;
            }
        } else if (item_type == ITEM_WEAPON) {
            minfo("itemtype is weapon");
            //weapontype weapon_type = g_get_weapon_type(g, id);
            weapontype_t weapon_type = g->ct.get<weapontype>(id).value_or(WEAPON_NONE);
            minfo("weapontype is %d", weapon_type);

            switch (weapon_type) {
            case WEAPON_DAGGER: create_spritegroup(g, id, TX_DAGGER_KEYS, TX_DAGGER_COUNT, -12, -12); break;
            case WEAPON_SWORD: create_spritegroup(g, id, TX_SWORD_KEYS, TX_SWORD_COUNT, -12, -12); break;
            case WEAPON_AXE: create_spritegroup(g, id, TX_AXE_KEYS, TX_AXE_COUNT, -12, -12); break;
            case WEAPON_BOW: create_spritegroup(g, id, TX_BOW_KEYS, TX_BOW_COUNT, -12, -12); break;
            case WEAPON_TWO_HANDED_SWORD: create_spritegroup(g, id, TX_TWO_HANDED_SWORD_KEYS, TX_TWO_HANDED_SWORD_COUNT, -12, -12); break;
            case WEAPON_WARHAMMER: create_spritegroup(g, id, TX_WARHAMMER_KEYS, TX_WARHAMMER_COUNT, -12, -12); break;
            case WEAPON_FLAIL: create_spritegroup(g, id, TX_WHIP_KEYS, TX_WHIP_COUNT, -12, -12); break;
            default: merror("unknown weapon type %d", weapon_type); return;
            }
        } else if (item_type == ITEM_SHIELD) {
            create_spritegroup(g, id, TX_BUCKLER_KEYS, TX_BUCKLER_COUNT, -12, -12);
        }
        //} else if (item_type == ITEM_WAND) {
        //    create_spritegroup(g, id, TX_WAND_BASIC_KEYS, TX_WAND_BASIC_COUNT, -12, -12);
        //} else if (item_type == ITEM_RING) {
        //    ringtype ring_type = g_get_ring_type(g, id);
        //    switch (ring_type) {
        //    case RING_GOLD: create_spritegroup(g, id, TX_GOLD_RING_KEYS, TX_GOLD_RING_COUNT, -12, -12); break;
        //    case RING_SILVER: create_spritegroup(g, id, TX_SILVER_RING_KEYS, TX_SILVER_RING_COUNT, -12, -12); break;
        //    default: merror("unknown ring type %d", ring_type); return;
        //    }
        //} else {
        //    merror("unknown item type %d", item_type);
    }
    minfo("END create_sg_byid");
}
