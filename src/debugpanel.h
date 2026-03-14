/** @file debugpanel.h
 *  @brief Debug overlay layout and text-buffer storage definitions.
 */

#pragma once

#include <raylib.h>

//#define DEBUGPANELBUFSIZE 256
/// @brief Maximum size of the debug-panel text buffer.
#define DEBUGPANELBUFSIZE 496
// game begins black-screening at 497, not sure why
//#define DEBUGPANELBUFSIZE 497

/// @brief Runtime configuration and text storage for the on-screen debug panel.
typedef struct debugpanel {
    int x;
    int y;
    int w;
    int h;

    int pad_left;
    int pad_right;
    int pad_top;
    int pad_bottom;

    int font_size;
    //Vector2 pos;
    Color fg_color;
    Color bg_color;
    //Rectangle textbox;
    //Rectangle box;
    char buffer[DEBUGPANELBUFSIZE];
} debugpanel_t;
