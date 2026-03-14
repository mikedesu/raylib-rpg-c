/** @file attack_result.h
 *  @brief Outcome categories for resolved attack attempts.
 */

#pragma once

/// @brief Coarse result categories returned from attack resolution.
typedef enum
{
    ATTACK_RESULT_NONE,
    ATTACK_RESULT_HIT,
    ATTACK_RESULT_MISS,
    ATTACK_RESULT_BLOCK,
    ATTACK_RESULT_COUNT,
} attack_result_t;
