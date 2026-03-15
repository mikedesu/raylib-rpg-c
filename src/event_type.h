/** @file event_type.h
 *  @brief Turn-scoped gameplay event categories and queue payloads.
 */

#pragma once

#include "entityid.h"
#include "vec3.h"

/// @brief High-level gameplay events that may be queued and resolved in order.
typedef enum {
    EVENT_NONE = 0,
    EVENT_MOVE_INTENT,
    EVENT_ATTACK_INTENT,
    EVENT_PULL_INTENT,
    EVENT_OPEN_DOOR_INTENT,
    EVENT_TRAVERSE_STAIRS_INTENT,
    EVENT_REFRESH_PRESSURE_PLATES,
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
    vec3 delta = vec3{0, 0, 0};
    vec3 target_loc = vec3{-1, -1, -1};
    int floor = -1;
};

/// @brief Resolution summary for a processed gameplay event.
struct gameplay_event_result_t {
    event_type_t type = EVENT_NONE;
    entityid actor_id = ENTITYID_INVALID;
    bool handled = false;
    bool succeeded = false;
};
