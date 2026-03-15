/** @file draw_character_creation_screen.h
 *  @brief Character-creation scene drawing helper.
 */

#pragma once

#include "gamestate.h"
#include "libdraw_context.h"
#include "libgame_defines.h"
#include "texture_ids.h"
#include "textureinfo.h"
#include <array>
#include <string>

/** @brief Draw the character-creation UI, stats, instructions, and race preview. */
void draw_character_creation_screen(gamestate& g);
