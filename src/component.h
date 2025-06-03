#pragma once

//#include "direction_component.h"
//#include "ac_component.h"
#include "arrow.h"
#include "arrow_component.h"
#include "attacking_component.h"
#include "attrib_effect.h"
#include "base_attack_damage_component.h"
#include "block_success_component.h"
#include "blocking_component.h"
#include "damage_component.h"
#include "damaged_component.h"
//#include "dead_component.h"
//#include "default_action_component.h"
//#include "direction.h"
//#include "entity_actions.h"
#include "entityid.h"
//#include "entitytype.h"
#include "equipment_component.h"
#include "equipment_slot.h"
#include "int_component.h"
#include "inventory_component.h"
#include "item.h"
#include "loc_component.h"
#include "location.h"
#include "name_component.h"
#include "potion.h"
//#include "potion_component.h"
//#include "itemtype_component.h"
#include "quiver_component.h"
#include "race.h"
#include "shield.h"
//#include "shieldtype_component.h"
#include "spell_effect_component.h"
#include "sprite_move_component.h"
#include "stats_component.h"
#include "stats_slot.h"
#include "target_component.h"
#include "target_path_component.h"
#include "update_component.h"
#include "weapon.h"
//#include "weapontype_component.h"
#include "zapping_component.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum component_t
{
    C_NONE,
    C_ID,
    C_NAME,
    C_TYPE,
    C_RACE,
    C_DIRECTION,
    C_ITEMTYPE,
    C_WEAPONTYPE,
    C_SHIELDTYPE,
    C_POTIONTYPE,
    C_AC,
    C_LOCATION,
    C_SPRITE_MOVE,
    C_DEAD,
    C_UPDATE,
    C_ATTACKING,
    C_BLOCKING,
    C_BLOCK_SUCCESS,
    C_DAMAGED,
    C_ZAPPING,
    C_INVENTORY,
    C_DEFAULT_ACTION,
    C_TARGET,
    C_TARGET_PATH,
    C_EQUIPMENT,
    C_STATS,
    C_DAMAGE,
    C_BASE_ATTACK_DAMAGE,
    C_SPELL_EFFECT,
    C_COUNT
} component;

static inline const char* component2str(component c) {
    switch (c) {
    case C_NONE: return "C_NONE";
    case C_ID: return "C_ID";
    case C_NAME: return "C_NAME";
    case C_TYPE: return "C_TYPE";
    case C_RACE: return "C_RACE";
    case C_DIRECTION: return "C_DIRECTION";
    case C_LOCATION: return "C_LOCATION";
    case C_SPRITE_MOVE: return "C_SPRITE_MOVE";
    case C_DEAD: return "C_DEAD";
    case C_UPDATE: return "C_UPDATE";
    case C_ATTACKING: return "C_ATTACKING";
    case C_BLOCKING: return "C_BLOCKING";
    case C_BLOCK_SUCCESS: return "C_BLOCK_SUCCESS";
    case C_DAMAGED: return "C_DAMAGED";
    case C_INVENTORY: return "C_INVENTORY";
    case C_DEFAULT_ACTION: return "C_DEFAULT_ACTION";
    case C_TARGET: return "C_TARGET";
    case C_TARGET_PATH: return "C_TARGET_PATH";
    case C_EQUIPMENT: return "C_EQUIPMENT";
    case C_STATS: return "C_STATS";
    case C_ITEMTYPE: return "C_ITEMTYPE";
    case C_WEAPONTYPE: return "C_WEAPONTYPE";
    case C_SHIELDTYPE: return "C_SHIELDTYPE";
    case C_POTIONTYPE: return "C_POTIONTYPE";
    case C_DAMAGE: return "C_DAMAGE";
    case C_AC: return "C_AC";
    case C_ZAPPING: return "C_ZAPPING";
    case C_SPELL_EFFECT: return "C_SPELL_EFFECT";
    case C_BASE_ATTACK_DAMAGE: return "C_BASE_ATTACK_DAMAGE";
    case C_COUNT: return "C_COUNT";
    default: break;
    }
    return "C_UNKNOWN";
}

// string components
void init_name_component(name_component* comp, entityid id, const char* name);

// int components
void init_int_component(int_component* comp, entityid id, int data);

void init_arrowtype_component(arrowtype_component* comp, entityid id, arrowtype type);
void init_quiver_component(quiver_component* comp, entityid id, int capacity);

// bool components
//void init_dead_component(dead_component* comp, entityid id, bool dead);
void init_update_component(update_component* comp, entityid id, bool update);
void init_attacking_component(attacking_component* comp, entityid id, bool attacking);
void init_blocking_component(blocking_component* comp, entityid id, bool blocking);
void init_block_success_component(block_success_component* comp, entityid id, bool block_success);
void init_damaged_component(damaged_component* comp, entityid id, bool damaged);
void init_zapping_component(zapping_component* comp, entityid id, bool zapping);

// loc components
void init_loc_component(loc_component* comp, entityid id, loc_t loc);
void init_sprite_move_component(sprite_move_component* comp, entityid id, loc_t loc);
void init_target_component(target_component* comp, entityid id, loc_t target);

// list components
void init_inventory_component(inventory_component* comp, entityid id);
void init_equipment_component(equipment_component* comp, entityid id);
void init_stats_component(stats_component* comp, entityid id);
void init_target_path_component(target_path_component* comp, entityid id, loc_t* target_path, int target_path_length);

// roll components
void init_damage_component(damage_component* comp, entityid id, roll r);
void init_base_attack_damage_component(base_attack_damage_component* comp, entityid id, roll r);

// complex components
void init_spell_effect_component(spell_effect_component* comp, entityid id, spell_effect effect);
