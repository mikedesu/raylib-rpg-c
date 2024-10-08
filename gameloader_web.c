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

#define NUM_VOID_FUNCTIONS 7

long last_write_time = 0;
const char* libname = "./libgame.so";
const char* lockfile = "./libgame.so.lockfile";
const char* templib = "./templibgame.so";

void* handle = NULL;

gamestate* g_ = NULL;

void gameloop();

bool (*mywindowshouldclose)() = NULL;
gamestate* (*mylibgame_getgamestate)() = NULL;
void (*mylibgameinit)() = NULL;
void (*mylibgameclosesavegamestate)() = NULL;
void (*mylibgameclose)(gamestate*) = NULL;
void (*mylibgamedrawframe)(gamestate*) = NULL;
void (*mylibgamehandleinput)(gamestate*) = NULL;
void (*mylibgameinitwithstate)(gamestate*) = NULL;
void (*mylibgameupdategamestate)(gamestate*) = NULL;
bool (*mylibgame_external_check_reload)() = NULL;




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
