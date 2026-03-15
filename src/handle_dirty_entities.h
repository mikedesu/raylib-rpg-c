/** @file handle_dirty_entities.h
 *  @brief Helpers for creating spritegroups for newly dirtied entities.
 */

#pragma once

#include "gamestate.h"

/** @brief Create/update spritegroups for newly added or dirtied entities. */
void libdraw_handle_dirty_entities(gamestate& g);
