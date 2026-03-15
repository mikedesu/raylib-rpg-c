/** @file libdraw_context.h
 *  @brief Shared renderer-global assets and frame targets used by the libdraw subsystem.
 */

#pragma once

#include "gamestate.h"
#include "libgame_defines.h"
#include "spritegroup.h"
#include "textureinfo.h"
#include <raylib.h>
#include <unordered_map>

using std::unordered_map;

/**
 * @brief Process-lifetime renderer state shared across the libdraw subsystem.
 *
 * This groups the renderer's mutable global state into one object so libdraw
 * callers depend on a single shared context instead of many unrelated globals.
 */
struct libdraw_context_t {
    /// @brief Active spritegroup instances keyed by owning entity id.
    unordered_map<entityid, spritegroup*> spritegroups;
    /// @brief Texture metadata table indexed by generated texture id.
    textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];
    /// @brief Loaded shaders keyed by project-specific shader identifiers.
    unordered_map<int, Shader> shaders;
    /// @brief Currently loaded background music stream.
    Music music = {};
    /// @brief Runtime music volume multiplier shared by libdraw audio helpers.
    float music_volume = DEFAULT_MUSIC_VOLUME;
    /// @brief Runtime master volume multiplier shared across audio systems.
    float master_volume = DEFAULT_MASTER_VOLUME;
    /// @brief Off-screen target used to compose the title screen.
    RenderTexture2D title_target_texture = {};
    /// @brief Off-screen target used to compose the character-creation scene.
    RenderTexture2D char_creation_target_texture = {};
    /// @brief Off-screen target used to compose the gameplay world scene.
    RenderTexture2D main_game_target_texture = {};
    /// @brief Off-screen target used to compose the HUD overlay.
    RenderTexture2D hud_target_texture = {};
    /// @brief Final compositing target presented to the window.
    RenderTexture2D target = {};
    /// @brief Source rectangle used when blitting the final render target.
    Rectangle target_src = {0, 0, DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT};
    /// @brief Destination rectangle used when scaling the final render target.
    Rectangle target_dest = {0, 0, DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT};
    /// @brief Window-space destination rectangle for direct presentation.
    Rectangle win_dest = {0, 0, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};
    /// @brief Global animation speed divisor used by sprite update code.
    int anim_speed = DEFAULT_ANIM_SPEED;
};

/// @brief Shared libdraw renderer context instance.
#ifdef CXXTEST_RUNNING
inline libdraw_context_t libdraw_ctx;
#else
extern libdraw_context_t libdraw_ctx;
#endif
