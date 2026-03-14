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

/// @brief Active spritegroup instances keyed by owning entity id.
extern unordered_map<entityid, spritegroup*> spritegroups;
/// @brief Texture metadata table indexed by generated texture id.
extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];
/// @brief Loaded shaders keyed by project-specific shader identifiers.
extern unordered_map<int, Shader> shaders;
/// @brief Off-screen target used to compose the title screen.
extern RenderTexture2D title_target_texture;
/// @brief Off-screen target used to compose the character-creation scene.
extern RenderTexture2D char_creation_target_texture;
/// @brief Off-screen target used to compose the gameplay world scene.
extern RenderTexture2D main_game_target_texture;
/// @brief Off-screen target used to compose the HUD overlay.
extern RenderTexture2D hud_target_texture;
/// @brief Final compositing target presented to the window.
extern RenderTexture2D target;
/// @brief Source rectangle used when blitting the final render target.
extern Rectangle target_src;
/// @brief Destination rectangle used when scaling the final render target.
extern Rectangle target_dest;
/// @brief Window-space destination rectangle for direct presentation.
extern Rectangle win_dest;
/// @brief Currently loaded background music track used by the renderer/game loop.
extern Music music;
/// @brief Global animation speed divisor used by sprite update code.
extern int ANIM_SPEED;
