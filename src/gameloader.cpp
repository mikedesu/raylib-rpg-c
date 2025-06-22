#include "gameloader.h"
#include "inputstate.h"
#include "massert.h"
#include "mprint.h"
#include <cstdio>
#include <raylib.h>
#include <stdbool.h>

void gamerun() {
    printf("Starting game...\n");

    printf("Initializing input state...\n");
    inputstate is = {0};
    printf("Initializing gamestate...\n");
    gamestate* g = gamestateinitptr();

    printf("Initializing liblogic...\n");
    liblogic_init(g);
    printf("Initializing libdraw...\n");
    libdraw_init(g);

    while (!libdraw_windowshouldclose(g)) {
        //libdraw_update_input(&is);
        //liblogic_tick(&is, g);
        //libdraw_update_sprites_pre(g);
        //libdraw_drawframe(g);
        //libdraw_update_sprites_post(g);

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
    /*
    */

    minfo("Closing libdraw...");
    libdraw_close();
    msuccess("libdraw closed");

    minfo("Closing liblogic...");
    liblogic_close(g);
    msuccess("liblogic closed");

    minfo("Freeing gamestate...");
    gamestatefree(g);
    msuccess("gamestate freed");
}
