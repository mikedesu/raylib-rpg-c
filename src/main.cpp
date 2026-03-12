/** @file main.cpp
 *  @brief Application entrypoint, process-lifetime globals, and outer game loop control.
 */

#include "gamestate.h"
#include "inputstate.h"
#include "libdraw.h"
#include "libgame_defines.h"
#ifdef WEB
#include <emscripten/emscripten.h>
#endif

/// @brief Global input snapshot updated once per outer frame.
inputstate is;

/// @brief Global gameplay state shared across the single-translation-unit runtime.
gamestate g;

/// @brief Renderer-owned sprite batches keyed by entity id.
unordered_map<entityid, spritegroup*> spritegroups;
/// @brief Global texture metadata table loaded during renderer initialization.
textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];
/// @brief Shader cache keyed by project-defined shader id.
unordered_map<int, Shader> shaders;
/// @brief Off-screen target for the title scene.
RenderTexture2D title_target_texture;
/// @brief Off-screen target for the character creation scene.
RenderTexture2D char_creation_target_texture;
/// @brief Off-screen target for the gameplay scene.
RenderTexture2D main_game_target_texture;
/// @brief Off-screen target for HUD composition.
RenderTexture2D hud_target_texture;
/// @brief Shared presentation target composed into the OS window each frame.
RenderTexture2D target;
/// @brief Source rectangle used when blitting the shared render target.
Rectangle target_src = {0, 0, DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT};
/// @brief Destination rectangle used when composing into the shared target.
Rectangle target_dest = {0, 0, DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT};
/// @brief Window-space destination rectangle for final presentation.
Rectangle win_dest = {0, 0, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};
/// @brief Currently loaded background music stream.
Music music;

/// @brief Runtime music volume multiplier exposed to gameplay and renderer code.
float music_volume = DEFAULT_MUSIC_VOLUME;
/// @brief Runtime master volume multiplier shared across audio systems.
float master_volume = DEFAULT_MASTER_VOLUME;

/// @brief Global animation speed tuning value used by sprite update code.
int ANIM_SPEED = DEFAULT_ANIM_SPEED;

/**
 * @brief Advance one outer frame of the application loop.
 *
 * Pulls fresh input, advances gameplay simulation, renders the current frame,
 * and performs an in-process gameplay restart when requested.
 */
static inline void gameloop() {
    inputstate_update(is);
    g.tick(is);
    drawframe(g);
    if (g.do_restart) {
        msuccess("Restarting game...");
        g.restart_game();
    }
}

/**
 * @brief Initialize the runtime and enter the platform-specific outer loop.
 *
 * Native builds run an explicit loop until gameplay requests shutdown. Web
 * builds hand control to Emscripten's browser-managed loop instead.
 *
 * @return Process exit code.
 */
int main() {
    g.logic_init();
    libdraw_init(g);
#ifndef WEB
    while (!libdraw_windowshouldclose(g)) {
        gameloop();
    }
#else
    emscripten_set_main_loop(gameloop, 0, 1);
#endif
    libdraw_close();
    g.logic_close();
    return 0;
}
