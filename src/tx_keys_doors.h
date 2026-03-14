/** @file tx_keys_doors.h
 *  @brief Texture-key arrays for door entities.
 */

#pragma once

#include "libgame_defines.h"
#include "texture_ids.h"

// wooden door

/// @brief Number of texture keys used by the wooden door spritegroup.
#define TX_WOODEN_DOOR_COUNT 2

/// @brief Texture-key array used to build the wooden door spritegroup.
static int TX_WOODEN_DOOR_KEYS[TX_WOODEN_DOOR_COUNT] = {TX_DOOR_WOODEN_CLOSED_00, TX_DOOR_WOODEN_OPEN_00};
