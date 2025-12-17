#pragma once

#include "entitytype.h"
#include "gamestate.h"
#include "item.h"
#include "libdraw_create_spritegroup.h"
#include "tx_keys_boxes.h"
#include "tx_keys_doors.h"
#include "tx_keys_monsters.h"
#include "tx_keys_npcs.h"
#include "tx_keys_potions.h"
#include "tx_keys_shields.h"
#include "tx_keys_weapons.h"


static inline void create_npc_sg_byid(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g->ct.get<race>(id).value_or(RACE_NONE)) {
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
    default: break;
    }
}

static inline void create_door_sg_byid(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    create_spritegroup(g, id, TX_WOODEN_DOOR_KEYS, TX_WOODEN_DOOR_COUNT, -12, -12);
}

static inline void create_box_sg_byid(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    create_spritegroup(g, id, TX_WOODEN_BOX_KEYS, TX_WOODEN_BOX_COUNT, -12, -12);
}


static inline void create_potion_sg_byid(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g->ct.get<potiontype>(id).value_or(POTION_NONE)) {
    case POTION_HP_SMALL: create_spritegroup(g, id, TX_POTION_HP_SMALL_KEYS, TX_POTION_HP_SMALL_COUNT, -12, -12); break;
    case POTION_HP_MEDIUM: create_spritegroup(g, id, TX_POTION_HP_MEDIUM_KEYS, TX_POTION_HP_MEDIUM_COUNT, -12, -12); break;
    case POTION_HP_LARGE: create_spritegroup(g, id, TX_POTION_HP_LARGE_KEYS, TX_POTION_HP_LARGE_COUNT, -12, -12); break;
    case POTION_MP_SMALL: create_spritegroup(g, id, TX_POTION_MP_SMALL_KEYS, TX_POTION_MP_SMALL_COUNT, -12, -12); break;
    case POTION_MP_MEDIUM: create_spritegroup(g, id, TX_POTION_MP_MEDIUM_KEYS, TX_POTION_MP_MEDIUM_COUNT, -12, -12); break;
    case POTION_MP_LARGE: create_spritegroup(g, id, TX_POTION_MP_LARGE_KEYS, TX_POTION_MP_LARGE_COUNT, -12, -12); break;
    default: break;
    }
}


static inline void create_weapon_sg_byid(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g->ct.get<weapontype>(id).value_or(WEAPON_NONE)) {
    case WEAPON_DAGGER: create_spritegroup(g, id, TX_DAGGER_KEYS, TX_DAGGER_COUNT, -12, -12); break;
    case WEAPON_SWORD: create_spritegroup(g, id, TX_SWORD_KEYS, TX_SWORD_COUNT, -12, -12); break;
    case WEAPON_AXE: create_spritegroup(g, id, TX_AXE_KEYS, TX_AXE_COUNT, -12, -12); break;
    //case WEAPON_BOW: create_spritegroup(g, id, TX_BOW_KEYS, TX_BOW_COUNT, -12, -12); break;
    //case WEAPON_TWO_HANDED_SWORD: create_spritegroup(g, id, TX_TWO_HANDED_SWORD_KEYS, TX_TWO_HANDED_SWORD_COUNT, -12, -12); break;
    //case WEAPON_WARHAMMER: create_spritegroup(g, id, TX_WARHAMMER_KEYS, TX_WARHAMMER_COUNT, -12, -12); break;
    //case WEAPON_FLAIL: create_spritegroup(g, id, TX_WHIP_KEYS, TX_WHIP_COUNT, -12, -12); break;
    default: break;
    }
}


static inline void create_shield_sg_byid(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");

    switch (g->ct.get<shieldtype>(id).value_or(SHIELD_NONE)) {
    case SHIELD_BUCKLER: create_spritegroup(g, id, TX_BUCKLER_KEYS, TX_BUCKLER_COUNT, -12, -12); break;
    case SHIELD_KITE: create_spritegroup(g, id, TX_KITE_SHIELD_KEYS, TX_BUCKLER_COUNT, -12, -12); break;
    case SHIELD_TOWER: create_spritegroup(g, id, TX_TOWER_SHIELD_KEYS, TX_TOWER_SHIELD_COUNT, -12, -12); break;
    default: break;
    }
}


static inline void create_item_sg_byid(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");

    switch (g->ct.get<itemtype>(id).value_or(ITEM_NONE)) {
    case ITEM_POTION: create_potion_sg_byid(g, id); break;
    case ITEM_WEAPON: create_weapon_sg_byid(g, id); break;
    case ITEM_SHIELD: create_shield_sg_byid(g, id); break;
    default: break;
    }
}


static inline void create_sg_byid(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");

    switch (g->ct.get<entitytype>(id).value_or(ENTITY_NONE)) {
    case ENTITY_PLAYER:
    case ENTITY_NPC: create_npc_sg_byid(g, id); break;
    case ENTITY_DOOR: create_door_sg_byid(g, id); break;
    case ENTITY_WOODEN_BOX: create_box_sg_byid(g, id); break;
    case ENTITY_ITEM: create_item_sg_byid(g, id); break;

    default: break;
    }
}
