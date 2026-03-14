/** @file scene.h
 *  @brief High-level scene identifiers for top-level game flow.
 */

#pragma once

/// @brief Primary scenes that the game loop can present.
typedef enum {
    SCENE_COMPANY = 0,
    SCENE_TITLE,
    SCENE_MAIN_MENU,
    SCENE_CHARACTER_CREATION,
    SCENE_GAMEPLAY,
} scene_t;
