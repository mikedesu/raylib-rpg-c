#include "gamestate.h"
#include "inputstate.h"
#include "libdraw.h"
#include "liblogic.h"

#ifdef WEB
#include <emscripten/emscripten.h>
#endif

inputstate is;
gamestate g;

void gameloop() {
    inputstate_update(is);
    liblogic_tick(g, is);
    libdraw_drawframe(g);
    if (g.do_restart) {
        msuccess("Restarting game...");
        libdraw_close();
        liblogic_close(g);
        gamestateinitptr(g);
        liblogic_init(g);
        libdraw_init(g);
        g.do_restart = false; // Reset restart flag
        g.restart_count++;
    }
}

int main() {
    gamestateinitptr(g);
    liblogic_init(g);
    libdraw_init(g);
#ifndef WEB
    while (!libdraw_windowshouldclose(g))
        gameloop();
#else
    emscripten_set_main_loop(gameloop, 0, 1);
#endif
    libdraw_close();
    liblogic_close(g);
    gamestate_free(g);
    return 0;
}
