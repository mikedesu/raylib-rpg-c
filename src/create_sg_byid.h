#pragma once

#include "entitytype.h"
#include "gamestate.h"
#include "item.h"
#include "libdraw_create_spritegroup.h"
#include "proptype.h"
#include "spell.h"
#include "tx_keys_boxes.h"
#include "tx_keys_doors.h"
#include "tx_keys_monsters.h"
#include "tx_keys_npcs.h"
#include "tx_keys_potions.h"
#include "tx_keys_props.h"
#include "tx_keys_shields.h"
#include "tx_keys_spells.h"
#include "tx_keys_weapons.h"

static inline void create_npc_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");

    const race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
    massert(r != RACE_NONE, "race is none for id %d", id);

    int* keys = NULL;
    int key_count = 0;

    switch (r) {
    case RACE_HUMAN: {
        keys = TX_HUMAN_KEYS;
        key_count = TX_HUMAN_COUNT;
    } break;
    case RACE_ORC: {
        keys = TX_ORC_KEYS;
        key_count = TX_ORC_COUNT;
    } break;
    case RACE_ELF: {
        keys = TX_ELF_KEYS;
        key_count = TX_ELF_COUNT;
    } break;
    case RACE_DWARF: {
        keys = TX_DWARF_KEYS;
        key_count = TX_DWARF_COUNT;
    } break;
    case RACE_HALFLING: {
        keys = TX_HALFLING_KEYS;
        key_count = TX_HALFLING_COUNT;
    } break;
    case RACE_GOBLIN: {
        keys = TX_GOBLIN_KEYS;
        key_count = TX_GOBLIN_COUNT;
    } break;
    case RACE_WOLF: {
        keys = TX_WOLF_KEYS;
        key_count = TX_WOLF_COUNT;
    } break;
    case RACE_BAT: {
        keys = TX_BAT_KEYS;
        key_count = TX_BAT_COUNT;
    } break;
    case RACE_WARG: {
        keys = TX_WARG_KEYS;
        key_count = TX_WARG_COUNT;
    } break;
    case RACE_GREEN_SLIME: {
        keys = TX_GREEN_SLIME_KEYS;
        key_count = TX_GREEN_SLIME_COUNT;
    } break;
    case RACE_SKELETON: {
        keys = TX_SKELETON_KEYS;
        key_count = TX_SKELETON_COUNT;
    } break;
    case RACE_RAT: {
        keys = TX_RAT_KEYS;
        key_count = TX_RAT_COUNT;
    } break;
    case RACE_ZOMBIE: {
        keys = TX_ZOMBIE_KEYS;
        key_count = TX_ZOMBIE_COUNT;
    } break;
    default: break;
    }

    massert(keys != NULL, "keys is null");
    massert(key_count > 0, "key_count is not > 0");

    //create_spritegroup(g, id, keys, key_count, -12, -12);
    create_sg(g, id, keys, key_count);
}

static inline void create_door_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    minfo("create_door_sg_byid: %d", id);
    //create_spritegroup(g, id, TX_WOODEN_DOOR_KEYS, TX_WOODEN_DOOR_COUNT, -12, -12);
    create_sg(g, id, TX_WOODEN_DOOR_KEYS, TX_WOODEN_DOOR_COUNT);
}

static inline void create_box_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    //create_spritegroup(g, id, TX_WOODEN_BOX_KEYS, TX_WOODEN_BOX_COUNT, -12, -12);
    create_sg(g, id, TX_WOODEN_BOX_KEYS, TX_WOODEN_BOX_COUNT);
}

static inline void create_potion_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<potiontype>(id).value_or(POTION_NONE)) {
    //case POTION_HP_SMALL: create_spritegroup(g, id, TX_POTION_HP_SMALL_KEYS, TX_POTION_HP_SMALL_COUNT, -12, -12); break;
    case POTION_HP_SMALL: create_sg(g, id, TX_POTION_HP_SMALL_KEYS, TX_POTION_HP_SMALL_COUNT); break;
    case POTION_HP_MEDIUM: create_sg(g, id, TX_POTION_HP_MEDIUM_KEYS, TX_POTION_HP_MEDIUM_COUNT); break;
    case POTION_HP_LARGE: create_sg(g, id, TX_POTION_HP_LARGE_KEYS, TX_POTION_HP_LARGE_COUNT); break;
    case POTION_MP_SMALL: create_sg(g, id, TX_POTION_MP_SMALL_KEYS, TX_POTION_MP_SMALL_COUNT); break;
    case POTION_MP_MEDIUM: create_sg(g, id, TX_POTION_MP_MEDIUM_KEYS, TX_POTION_MP_MEDIUM_COUNT); break;
    case POTION_MP_LARGE: create_sg(g, id, TX_POTION_MP_LARGE_KEYS, TX_POTION_MP_LARGE_COUNT); break;
    default: break;
    }
}

static inline void create_weapon_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<weapontype>(id).value_or(WEAPON_NONE)) {
    case WEAPON_DAGGER: create_sg(g, id, TX_DAGGER_KEYS, TX_DAGGER_COUNT); break;
    case WEAPON_SHORT_SWORD: create_sg(g, id, TX_SHORT_SWORD_KEYS, TX_SHORT_SWORD_COUNT); break;
    case WEAPON_AXE: create_sg(g, id, TX_AXE_KEYS, TX_AXE_COUNT); break;
    //case WEAPON_BOW: create_spritegroup(g, id, TX_BOW_KEYS, TX_BOW_COUNT, -12, -12); break;
    //case WEAPON_TWO_HANDED_SWORD: create_spritegroup(g, id, TX_TWO_HANDED_SWORD_KEYS, TX_TWO_HANDED_SWORD_COUNT, -12, -12); break;
    //case WEAPON_WARHAMMER: create_spritegroup(g, id, TX_WARHAMMER_KEYS, TX_WARHAMMER_COUNT, -12, -12); break;
    //case WEAPON_FLAIL: create_spritegroup(g, id, TX_WHIP_KEYS, TX_WHIP_COUNT, -12, -12); break;
    default: break;
    }
}

static inline void create_shield_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<shieldtype>(id).value_or(SHIELD_NONE)) {
    case SHIELD_BUCKLER: create_sg(g, id, TX_BUCKLER_KEYS, TX_BUCKLER_COUNT); break;
    case SHIELD_KITE: create_sg(g, id, TX_KITE_SHIELD_KEYS, TX_BUCKLER_COUNT); break;
    case SHIELD_TOWER: create_sg(g, id, TX_TOWER_SHIELD_KEYS, TX_TOWER_SHIELD_COUNT); break;
    default: break;
    }
}

static inline void create_spell_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<spelltype>(id).value_or(SPELLTYPE_NONE)) {
    case SPELLTYPE_FIRE: create_sg(g, id, TX_SPELL_FIRE_KEYS, TX_SPELL_FIRE_COUNT); break;
    default: break;
    }
}

static inline void create_item_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<itemtype>(id).value_or(ITEM_NONE)) {
    case ITEM_POTION: create_potion_sg_byid(g, id); break;
    case ITEM_WEAPON: create_weapon_sg_byid(g, id); break;
    case ITEM_SHIELD: create_shield_sg_byid(g, id); break;
    default: break;
    }
}

static inline void create_prop_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<proptype>(id).value_or(PROP_NONE)) {
    case PROP_DUNGEON_BANNER_00: create_sg(g, id, TX_PROP_DUNGEON_BANNER_00_KEYS, TX_PROP_DUNGEON_BANNER_00_COUNT); break;
    case PROP_DUNGEON_BANNER_01: create_sg(g, id, TX_PROP_DUNGEON_BANNER_01_KEYS, TX_PROP_DUNGEON_BANNER_01_COUNT); break;
    case PROP_DUNGEON_BANNER_02: create_sg(g, id, TX_PROP_DUNGEON_BANNER_02_KEYS, TX_PROP_DUNGEON_BANNER_02_COUNT); break;
    case PROP_DUNGEON_WOODEN_TABLE_00: create_sg(g, id, TX_PROP_DUNGEON_WOODEN_TABLE_00_KEYS, TX_PROP_DUNGEON_WOODEN_TABLE_00_COUNT); break;
    case PROP_DUNGEON_WOODEN_TABLE_01: create_sg(g, id, TX_PROP_DUNGEON_WOODEN_TABLE_01_KEYS, TX_PROP_DUNGEON_WOODEN_TABLE_01_COUNT); break;
    case PROP_DUNGEON_WOODEN_CHAIR_00: create_sg(g, id, TX_PROP_DUNGEON_WOODEN_CHAIR_00_KEYS, TX_PROP_DUNGEON_WOODEN_CHAIR_00_COUNT); break;
    case PROP_DUNGEON_STATUE_00: create_sg(g, id, TX_PROP_DUNGEON_STATUE_00_KEYS, TX_PROP_DUNGEON_STATUE_00_COUNT); break;
    default: break;
    }
}

static inline void create_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<entitytype>(id).value_or(ENTITY_NONE)) {
    case ENTITY_PLAYER:
    case ENTITY_NPC: create_npc_sg_byid(g, id); break;
    case ENTITY_DOOR: create_door_sg_byid(g, id); break;
    case ENTITY_BOX: create_box_sg_byid(g, id); break;
    case ENTITY_ITEM: create_item_sg_byid(g, id); break;
    case ENTITY_SPELL: create_spell_sg_byid(g, id); break;
    case ENTITY_PROP: create_prop_sg_byid(g, id); break;
    default: break;
    }
}
