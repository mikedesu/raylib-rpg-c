#pragma once

#include "arrow.h"
#include "entityid.h"
#include "equipment_slot.h"
#include "item.h"
#include "potion.h"
#include "race.h"
#include "shield.h"
#include "stats_slot.h"
#include "weapon.h"
#include <cstdio>
#include <cstdlib>
#include <raylib.h>


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
    C_EQUIPPED_WEAPON = 24,
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
} component;


static inline const char* component2str(component c) {
    switch (c) {
    case C_NONE: return "NONE";
    case C_ID: return "ID";
    case C_NAME: return "NAME";
    case C_TYPE: return "TYPE";
    case C_RACE: return "RACE";
    case C_DIRECTION: return "DIRECTION";
    case C_LOCATION: return "LOCATION";
    case C_SPRITE_MOVE: return "SPRITE_MOVE";
    case C_DEAD: return "DEAD";
    case C_UPDATE: return "UPDATE";
    case C_ATTACKING: return "ATTACKING";
    case C_BLOCKING: return "BLOCKING";
    case C_BLOCK_SUCCESS: return "BLOCK_SUCCESS";
    case C_DAMAGED: return "DAMAGED";
    case C_INVENTORY: return "INVENTORY";
    case C_DEFAULT_ACTION: return "DEFAULT_ACTION";
    case C_TARGET: return "TARGET";
    case C_TARGET_PATH: return "TARGET_PATH";
    case C_EQUIPPED_WEAPON: return "EQUIPPED_WEAPON";
    case C_STATS: return "STATS";
    case C_ITEMTYPE: return "ITEMTYPE";
    case C_WEAPONTYPE: return "WEAPONTYPE";
    case C_SHIELDTYPE: return "SHIELDTYPE";
    case C_POTIONTYPE: return "POTIONTYPE";
    case C_DAMAGE: return "DAMAGE";
    case C_AC: return "AC";
    case C_ZAPPING: return "ZAPPING";
    case C_VISION_DISTANCE: return "VISION_DISTANCE";
    case C_LIGHT_RADIUS: return "LIGHT_RADIUS";
    case C_LIGHT_RADIUS_BONUS: return "LIGHT_RADIUS_BONUS";
    case C_RINGTYPE: return "RINGTYPE";
    case C_BASE_ATTACK_DAMAGE: return "BASE_ATTACK_DAMAGE";
    case C_EXPLORED_LIST: return "EXPLORED_LIST";
    case C_VISIBLE_LIST: return "VISIBLE_LIST";
    case C_TX_ALPHA: return "TX_ALPHA";
    case C_PUSHABLE: return "PUSHABLE";
    case C_COUNT: return "COUNT";
    default: break;
    }
    return "UNKNOWN";
}
