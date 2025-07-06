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
    shared_ptr<inputstate> is = make_shared<inputstate>();
    shared_ptr<gamestate> g = gamestateinitptr();
    liblogic_init(g);
    libdraw_init(g);
    while (!libdraw_windowshouldclose(g)) {
        //libdraw_update_input(is);
        inputstate_update(is);
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
    libdraw_close();
    liblogic_close(g);
    gamestate_free(g);
}
