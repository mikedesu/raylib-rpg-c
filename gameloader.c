#include "gameloader.h"
#include "gamestate.h"
#include "mprint.h"
#include "symaddrpair.h"

#include <dlfcn.h>
#include <raylib.h>
#include <rlgl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

long last_write_time = 0;

unsigned int old_framecount = 0;

const int default_window_width = 960;
const int default_window_height = 540;

const char* libname = "./libgame.so";
const char* lockfile = "./libgame.so.lockfile";
const char* templib = "./templibgame.so";

void* handle = NULL;

gamestate* g = NULL;

bool (*mywindowshouldclose)() = NULL;
gamestate* (*mylibgame_getgamestate)() = NULL;

void (*myinitwindow)() = NULL;
void (*myclosewindow)() = NULL;
void (*mylibgameinit)() = NULL;

void (*mylibgameclosesavegamestate)() = NULL;

void (*mylibgameclose)(gamestate*) = NULL;
void (*mylibgamedrawframe)(gamestate*) = NULL;
void (*mylibgamehandleinput)(gamestate*) = NULL;
void (*mylibgameinitwithstate)(gamestate*) = NULL;
void (*mylibgameupdategamestate)(gamestate*) = NULL;

//void (*mylibgameinitframecount)(unsigned int) = NULL;


// get the last write time of a file
time_t getlastwritetime(const char* filename) {
    struct stat file_stat;
    time_t retval = 0;
    if (stat(filename, &file_stat) == 0) {
        retval = file_stat.st_mtime;
    }
    return retval;
}


void openhandle() {
    handle = dlopen(libname, RTLD_LAZY);
    if (!handle) {
        merror("handle is NULL");
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        exit(1);
    }
}


void checksymbol(void* symbol, const char* name) {
    if (symbol == NULL) {
        merror("symbol is NULL");
        fprintf(stderr, "dlsym failed: %s\n", dlerror());
        exit(1);
    }
}


void loadsymbols() {
    minfo("begin loadsymbols");

#define NUM_VOID_FUNCTIONS 9

    symaddrpair_t pairs[NUM_VOID_FUNCTIONS] = {
        {"libgame_initwindow", &myinitwindow},
        {"libgame_closewindow", &myclosewindow},
        {"libgame_drawframe", &mylibgamedrawframe},
        {"libgame_init", &mylibgameinit},
        {"libgame_close", &mylibgameclose},
        {"libgame_handleinput", &mylibgamehandleinput},
        {"libgame_closesavegamestate", &mylibgameclosesavegamestate},
        {"libgame_initwithstate", &mylibgameinitwithstate},
        {"libgame_updategamestate", &mylibgameupdategamestate}};

    for (int i = 0; i < NUM_VOID_FUNCTIONS; i++) {
        minfo(pairs[i].name);
        *pairs[i].addr = dlsym(handle, pairs[i].name);
        checksymbol(*pairs[i].addr, pairs[i].name);
    }

    char* sym = NULL;

    // gamestate return
    sym = "libgame_getgamestate";
    minfo(sym);
    mylibgame_getgamestate = dlsym(handle, sym);
    checksymbol(mylibgame_getgamestate, sym);

    // boolean return
    sym = "libgame_windowshouldclose";
    minfo(sym);
    mywindowshouldclose = dlsym(handle, sym);
    checksymbol(mywindowshouldclose, sym);

    //////////////////////////////////////

    minfo("end loadsymbols");
}


void autoreload() {
    if (getlastwritetime(libname) > last_write_time) {
        last_write_time = getlastwritetime(libname);
        //while (FileExists(lockfile)) {
        while (access(lockfile, F_OK) == 0) {
            minfo("Library is locked\n");
            sleep(1);
        }
        minfo("getting old gamestate");
        g = mylibgame_getgamestate();
        // this time, we have to shut down the game and close the window
        // before we can reload and restart everything
        minfo("closing libgame");
        mylibgameclosesavegamestate(); // closes window
        minfo("unloading library");
        dlclose(handle);
        minfo("opening handle");
        openhandle();
        minfo("loading symbols");
        loadsymbols();
        mylibgameinitwithstate(g);
    }
}


void gamerun() {
    minfo("gamerun");
    minfo("opening handle");
    openhandle();
    minfo("loading symbols");
    loadsymbols();
    minfo("initing window");
    last_write_time = getlastwritetime(libname);

    mylibgameinit();
    g = mylibgame_getgamestate();

    minfo("entering gameloop");
    while (!mywindowshouldclose()) {
        mylibgamedrawframe(g);
        mylibgamehandleinput(g);
        mylibgameupdategamestate(g);
        autoreload();
    }
    minfo("closing libgame");
    mylibgameclose(g);
}
