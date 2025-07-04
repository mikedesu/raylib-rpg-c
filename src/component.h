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
#include "vec3_list_component.h"
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
    C_NONE = 0,
    C_ID = 1,
    C_NAME = 2,
    C_TYPE = 3,
    C_RACE = 4,
    C_DIRECTION = 5,
    C_ITEMTYPE = 6,
    C_WEAPONTYPE = 7,
    C_SHIELDTYPE = 8,
    C_POTIONTYPE = 9,
    C_AC = 10,
    C_LOCATION = 11,
    C_SPRITE_MOVE = 12,
    C_DEAD = 13,
    C_UPDATE = 14,
    C_ATTACKING = 15,
    C_BLOCKING = 16,
    C_BLOCK_SUCCESS = 17,
    C_DAMAGED = 18,
    C_ZAPPING = 19,
    C_INVENTORY = 20,
    C_DEFAULT_ACTION = 21,
    C_TARGET = 22,
    C_TARGET_PATH = 23,
    C_EQUIPMENT = 24,
    C_STATS = 25,
    C_DAMAGE = 26,
    C_BASE_ATTACK_DAMAGE = 27,
    C_VISION_DISTANCE = 28,
    C_LIGHT_RADIUS = 29,
    C_RINGTYPE = 30,
    C_LIGHT_RADIUS_BONUS = 31,
    C_EXPLORED_LIST = 32,
    C_VISIBLE_LIST = 33,
    C_TX_ALPHA = 34,
    C_PUSHABLE = 35,
    C_COUNT
    //C_SPELL_EFFECT,
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
    case C_LIGHT_RADIUS_BONUS: return "C_LIGHT_RADIUS_BONUS";
    case C_RINGTYPE: return "C_RINGTYPE";
    case C_BASE_ATTACK_DAMAGE: return "C_BASE_ATTACK_DAMAGE";
    case C_EXPLORED_LIST: return "C_EXPLORED_LIST";
    case C_VISIBLE_LIST: return "C_VISIBLE_LIST";
    case C_TX_ALPHA: return "C_TX_ALPHA";
    case C_PUSHABLE: return "C_PUSHABLE";

    case C_COUNT: return "C_COUNT";
    default: break;
    }
    return "C_UNKNOWN";
    //case C_SPELL_EFFECT: return "C_SPELL_EFFECT";
}

// string components
//void init_name_component(name_component* comp, entityid id, const char* name);
//void init_name_component(name_component* comp, entityid id, string name);

// int components
//void init_int_component(int_component* comp, entityid id, int data);

// loc components: 3 integer
//void init_vec3_component(vec3_component* comp, entityid id, vec3 v);
// vec3 list components
//void init_vec3_list_component(vec3_list_component* comp, entityid id, vec3* list, int list_length);

// Rectangle component
//void init_rect_component(rect_component* comp, entityid id, Rectangle rect);

// list components
//void init_inventory_component(inventory_component* comp, entityid id);
//void init_equipment_component(equipment_component* comp, entityid id);
//void init_stats_component(stats_component* comp, entityid id);
//void init_target_path_component(target_path_component* comp, entityid id, vec3* target_path, int target_path_length);

// complex components
//void init_spell_effect_component(spell_effect_component* comp, entityid id, spell_effect effect);
