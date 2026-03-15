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
