#pragma once

#include "arrow.h"
#include "arrow_component.h"
#include "attacking_component.h"
#include "block_success_component.h"
#include "blocking_component.h"
#include "damaged_component.h"
#include "entityid.h"
#include "equipment_component.h"
#include "equipment_slot.h"
#include "int_component.h"
#include "inventory_component.h"
#include "item.h"
#include "name_component.h"
#include "potion.h"
#include "quiver_component.h"
#include "race.h"
#include "rect_component.h"
#include "shield.h"
#include "stats_component.h"
#include "stats_slot.h"
#include "target_path_component.h"
#include "update_component.h"
#include "vec3_component.h"
#include "weapon.h"
#include "zapping_component.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
//#include "spell_effect_component.h"
//#include "attrib_effect.h"
//#include "damage_component.h"

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
    C_VISION_DISTANCE,
    C_LIGHT_RADIUS,
    C_RINGTYPE,
    //C_SPELL_EFFECT,
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
    case C_VISION_DISTANCE: return "C_VISION_DISTANCE";
    case C_LIGHT_RADIUS: return "C_LIGHT_RADIUS";
    case C_RINGTYPE: return "C_RINGTYPE";
    //case C_SPELL_EFFECT: return "C_SPELL_EFFECT";
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

// loc components: 3 integer
void init_vec3_component(vec3_component* comp, entityid id, vec3 v);

// Rectangle component
void init_rect_component(rect_component* comp, entityid id, Rectangle rect);

// list components
void init_inventory_component(inventory_component* comp, entityid id);
void init_equipment_component(equipment_component* comp, entityid id);
void init_stats_component(stats_component* comp, entityid id);
void init_target_path_component(target_path_component* comp, entityid id, vec3* target_path, int target_path_length);

// complex components
//void init_spell_effect_component(spell_effect_component* comp, entityid id, spell_effect effect);
