/** @file hunger_points.h
 *  @brief Simple current/max hunger storage used by actor state.
 */

#pragma once

/// @brief Current and maximum hunger values for an entity.
typedef struct {
    int hunger_points;
    int max_hunger_points;
} hunger_points_t;
