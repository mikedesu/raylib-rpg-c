/** @file draw_title_screen.h
 *  @brief Title-screen and main-menu drawing helper.
 */

#pragma once

#include "gamestate.h"
#include "libdraw_context.h"
#include "texture_ids.h"
#include "textureinfo.h"
#include <raylib.h>

/** @brief Draw the title screen and optionally its menu selection list. */
void draw_title_screen(gamestate& g, bool show_menu);
