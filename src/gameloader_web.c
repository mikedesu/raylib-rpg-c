#include "gameloader.h"
#include "gamestate.h"
#include "libgame.h"
#include "mprint.h"
#include <dlfcn.h>
#include <emscripten/emscripten.h>
#include <raylib.h>
#include <rlgl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

gamestate* g_ = NULL;

void gameloop();

void gameloop() {
    libgame_handleinput(g_);
    libgame_updategamestate(g_);
    libgame_drawframe(g_);
}

void gamerun() {
    minfo("gamerun");
    libgame_init();
    g_ = libgame_getgamestate();
    minfo("entering gameloop");
    emscripten_set_main_loop(gameloop, 0, 1);
    minfo("closing libgame");
    libgame_close(g_);
    msuccess("libgame closed");
}
