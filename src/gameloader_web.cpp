#include "gameloader.h"
#include "gamestate.h"
#include "inputstate.h"
#include "libdraw.h"
#include "liblogic.h"
#include "mprint.h"
#include <emscripten/emscripten.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

inputstate is = {0};
gamestate* g = NULL;

void gameloop() {
    libdraw_update_input(&is);
    liblogic_tick(&is, g);
    libdraw_update_sprites_pre(g);
    libdraw_drawframe(g);
    libdraw_update_sprites_post(g);
    if (g->do_restart) {
        msuccess("Restarting game...");
        libdraw_close();
        //dlclose(draw_handle);
        liblogic_close(g);
        //dlclose(logic_handle);
        gamestatefree(g);
        g = gamestateinitptr();
        //load_draw_symbols();
        //load_logic_symbols();
        liblogic_init(g);
        libdraw_init(g);
        g->do_restart = false; // Reset restart flag
    }
}

void gamerun() {
    g = gamestateinitptr();
    // i think liblogic will need init before libdraw because
    // we will want to create entries in the entitymap AFTER it is init'd
    liblogic_init(g);
    libdraw_init(g);
    emscripten_set_main_loop(gameloop, 0, 1);
    libdraw_close();
    liblogic_close(g);
    gamestatefree(g);
}
