/** @file controlmode.h
 *  @brief Input-routing modes for gameplay and UI substates.
 */

#pragma once

/// @brief Active control context used to dispatch input within a scene.
typedef enum {
    CONTROLMODE_CAMERA = 0,
    CONTROLMODE_PLAYER,
    CONTROLMODE_INVENTORY,
    CONTROLMODE_CHEST,
    CONTROLMODE_ACTION_MENU,
    CONTROLMODE_OPTION_MENU,
    CONTROLMODE_SOUND_MENU,
    CONTROLMODE_WINDOW_COLOR_MENU,
    CONTROLMODE_CONTROLS_MENU,
    CONTROLMODE_HELP_MENU,
    CONTROLMODE_INTERACTION,
    CONTROLMODE_LEVEL_UP,
    CONTROLMODE_KEYBOARD_PROFILE,
    CONTROLMODE_CONFIRM_PROMPT,
    CONTROLMODE_COUNT
} controlmode_t;
