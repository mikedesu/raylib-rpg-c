/** @file gamestate_flag.h
 *  @brief High-level gameplay update phases used by the main loop.
 */

#pragma once

/// @brief Coarse state-machine phases for player input, animation, and NPC turns.
typedef enum {
    GAMESTATE_FLAG_NONE = 0,
    GAMESTATE_FLAG_PLAYER_INPUT = 1,
    GAMESTATE_FLAG_PLAYER_ANIM = 2,
    GAMESTATE_FLAG_NPC_TURN = 3,
    GAMESTATE_FLAG_NPC_ANIM = 4,
    GAMESTATE_FLAG_COUNT = 5
} gamestate_flag_t;
