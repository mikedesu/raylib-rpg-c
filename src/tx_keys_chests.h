/** @file tx_keys_chests.h
 *  @brief Texture-key arrays for chest entities.
 */

#pragma once

#include "libgame_defines.h"
#include "texture_ids.h"

/// @brief Number of texture keys used by the treasure chest spritegroup.
#define TX_TREASURE_CHEST_COUNT 2
/// @brief Texture-key array used to build the treasure chest spritegroup.
static int TX_TREASURE_CHEST_KEYS[TX_TREASURE_CHEST_COUNT] = {TX_CHEST_CLOSED_00, TX_CHEST_OPEN_00};
