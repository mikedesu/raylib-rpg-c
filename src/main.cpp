#include "gamestate.h"
#include "inputstate.h"
#include "libdraw.h"
#ifdef WEB
#include <emscripten/emscripten.h>
#endif


inputstate is;
gamestate g;


static inline void gameloop() {
    inputstate_update(is);
    g.tick(is);
    drawframe(g);
    if (g.do_restart) {
        msuccess("Restarting game...");
        libdraw_close();
        g.logic_close();
        g.reset();
        g.logic_init();
        libdraw_init(g);
        g.do_restart = false; // Reset restart flag
        g.restart_count++;
    }
}


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
