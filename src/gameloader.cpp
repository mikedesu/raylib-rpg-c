#include "gameloader.h"
#include "inputstate.h"
#include "massert.h"
#include "mprint.h"
#include <cstdio>
#include <memory>
#include <raylib.h>

using std::make_shared;
using std::shared_ptr;

void gamerun() {
    minfo("Initializing inputstate...");
    shared_ptr<inputstate> is = make_shared<inputstate>();
    msuccess("inputstate initialized");

    minfo("Initializing gamestate...");
    shared_ptr<gamestate> g = gamestateinitptr();
    msuccess("gamestate initialized");

    liblogic_init(g);
    minfo("Initializing libdraw...");
    libdraw_init(g);
    msuccess("libdraw initialized");

    while (!libdraw_windowshouldclose(g)) {
        libdraw_update_input(is);
        liblogic_tick(is, g);
        libdraw_update_sprites_pre(g);
        libdraw_drawframe(g);
        libdraw_update_sprites_post(g);
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

    minfo("Closing libdraw...");
    libdraw_close();
    msuccess("libdraw closed");

    minfo("Closing liblogic...");
    liblogic_close(g);
    msuccess("liblogic closed");

    minfo("Freeing gamestate...");
    gamestate_free(g);
    msuccess("gamestate freed");
}
