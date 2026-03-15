/** @file event_type.h
 *  @brief Turn-scoped gameplay event categories and queue payloads.
 */

#pragma once

#include "attack_result.h"
#include "entityid.h"
#include "vec3.h"

/// @brief High-level gameplay events that may be queued and resolved in order.
typedef enum {
    EVENT_NONE = 0,
    EVENT_MOVE_INTENT,
    EVENT_PUSH_INTENT,
    EVENT_ATTACK_INTENT,
    EVENT_PROVOKE_NPC,
    EVENT_ATTACK_BLOCK,
    EVENT_ATTACK_DAMAGE,
    EVENT_ATTACK_DEATH,
    EVENT_ATTACK_AWARD_XP,
    EVENT_ATTACK_DROP_INVENTORY,
    EVENT_ATTACK_PLAYER_DEATH,
    EVENT_ATTACK_WEAPON_DURABILITY,
    EVENT_ATTACK_SHIELD_DURABILITY,
    EVENT_PULL_INTENT,
    EVENT_OPEN_DOOR_INTENT,
    EVENT_OPEN_CHEST_INTENT,
    EVENT_INTERACT_INTENT,
    EVENT_PICKUP_INTENT,
    EVENT_USE_ITEM_INTENT,
    EVENT_EQUIP_ITEM_INTENT,
    EVENT_DROP_ITEM_INTENT,
    EVENT_CHEST_TRANSFER_INTENT,
    EVENT_TRAVERSE_STAIRS_INTENT,
    EVENT_REFRESH_PRESSURE_PLATES,
    EVENT_PRESSURE_PLATE_SET_DOOR,
    EVENT_COUNT
} event_type_t;

/**
 * @brief Queue payload for one gameplay event.
 *
 * The queue is intentionally narrow during the migration. Unused fields stay at
 * sentinel defaults until more event domains move over.
 */
struct gameplay_event_t {
    event_type_t type = EVENT_NONE;
    entityid actor_id = ENTITYID_INVALID;
    entityid target_id = ENTITYID_INVALID;
    entityid item_id = ENTITYID_INVALID;
    vec3 delta = vec3{0, 0, 0};
    vec3 target_loc = vec3{-1, -1, -1};
    int floor = -1;
    int amount = 0;
    bool state = false;
    attack_result_t attack_result = ATTACK_RESULT_NONE;
};

/// @brief Resolution summary for a processed gameplay event.
struct gameplay_event_result_t {
    event_type_t type = EVENT_NONE;
    entityid actor_id = ENTITYID_INVALID;
    bool handled = false;
    bool succeeded = false;
    attack_result_t attack_result = ATTACK_RESULT_NONE;
};
