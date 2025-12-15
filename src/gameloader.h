#pragma once
#include "gamestate.h"
#include "inputstate.h"
#include "libdraw.h"
#include "liblogic.h"

using std::make_shared;
using std::shared_ptr;

shared_ptr<inputstate> is = make_shared<inputstate>();
shared_ptr<gamestate> g = gamestateinitptr();


#ifndef WEB
static inline void gamerun() {
    shared_ptr<inputstate> is = make_shared<inputstate>();
    shared_ptr<gamestate> g = gamestateinitptr();
    liblogic_init(g);
    libdraw_init(g);
    while (!libdraw_windowshouldclose(g)) {
        gameloop();
        //inputstate_update(is);
        //liblogic_tick(is, g);
        //libdraw_drawframe(g);
        //if (g->do_restart) {
        //    minfo("Restarting game...");
        //    libdraw_close_partial();
        //    liblogic_close(g);
        //    gamestatefree(g);
        //    g = gamestateinitptr();
        //    liblogic_init(g);
        //    libdraw_init_rest(g);
        //    g->do_restart = false;
        //    msuccess("game restarted!");
        //}
    }
    libdraw_close();
    liblogic_close(g);
    gamestate_free(g);
}
#else
void gamerun() {
    shared_ptr<inputstate> is = make_shared<inputstate>();
    shared_ptr<gamestate> g = gamestateinitptr();
    // i think liblogic will need init before libdraw because
    // we will want to create entries in the entitymap AFTER it is init'd
    liblogic_init(g);
    libdraw_init(g);
    emscripten_set_main_loop(gameloop, 0, 1);
    libdraw_close();
    liblogic_close(g);
    gamestate_free(g);
}
#endif
