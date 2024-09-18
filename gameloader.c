#include "gameloader.h"
#include "gamestate.h"
#include "mprint.h"

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

bool (*mywindowshouldclose)(void) = NULL;
void (*myinitwindow)() = NULL;
void (*myclosewindow)() = NULL;
void (*mylibgamedrawframe)() = NULL;
void (*mylibgameinit)() = NULL;
void (*mylibgameinitwithstate)(void*) = NULL;
void (*mylibgameclose)() = NULL;
void (*mylibgameclosesavegamestate)() = NULL;
void (*mylibgamehandleinput)() = NULL;
void (*mylibgameinitframecount)(unsigned int) = NULL;
void (*mylibgameupdategamestate)(gamestate*) = NULL;

gamestate* (*mylibgame_getgamestate)() = NULL;


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
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        exit(1);
    }
}


void checksymbol(void* symbol, const char* name) {
    if (symbol == NULL) {
        fprintf(stderr, "dlsym failed: %s\n", dlerror());
        exit(1);
    }
}


void loadsymbols() {
    mprint("begin loadsymbols");

    char* sym = "libgame_windowshouldclose";
    mprint(sym);
    mywindowshouldclose = (bool (*)(void))dlsym(handle, sym);
    checksymbol(mywindowshouldclose, sym);

    sym = "gameinitwindow";
    mprint(sym);
    myinitwindow = dlsym(handle, sym);
    checksymbol(myinitwindow, sym);

    sym = "gameclosewindow";
    mprint(sym);
    myclosewindow = dlsym(handle, sym);
    checksymbol(myclosewindow, sym);

    sym = "libgamedrawframe";
    mprint(sym);
    mylibgamedrawframe = dlsym(handle, sym);
    checksymbol(mylibgamedrawframe, sym);

    mprint("libgameinit");
    mylibgameinit = dlsym(handle, "libgameinit");
    checksymbol(mylibgameinit, "libgameinit");

    mprint("libgameclose");
    mylibgameclose = dlsym(handle, "libgameclose");
    checksymbol(mylibgameclose, "libgameclose");

    mprint("libgamehandleinput");
    mylibgamehandleinput = dlsym(handle, "libgamehandleinput");
    checksymbol(mylibgamehandleinput, "libgamehandleinput");

    mprint("libgame_getgamestate");
    mylibgame_getgamestate = dlsym(handle, "libgame_getgamestate");
    checksymbol(mylibgame_getgamestate, "libgame_getgamestate");

    mprint("libgameclosesavegamestate");
    mylibgameclosesavegamestate = dlsym(handle, "libgameclosesavegamestate");
    checksymbol(mylibgameclosesavegamestate, "libgameclosesavegamestate");

    mprint("libgameinitwithstate");
    mylibgameinitwithstate = (void (*)(void*))dlsym(handle, "libgameinitwithstate");
    checksymbol(mylibgameinitwithstate, "libgameinitwithstate");


    mprint("libgameupdategamestate");
    mylibgameupdategamestate = (void (*)(gamestate*))dlsym(handle, "libgameupdategamestate");
    checksymbol(mylibgameupdategamestate, "libgameupdategamestate");

    //////////////////////////////////////

    mprint("end loadsymbols");
}


void autoreload() {
    if (getlastwritetime(libname) > last_write_time) {
        last_write_time = getlastwritetime(libname);
        //while (FileExists(lockfile)) {
        while (access(lockfile, F_OK) == 0) {
            printf("Library is locked\n");
            sleep(1);
        }
        mprint("getting old gamestate");
        g = mylibgame_getgamestate();
        // this time, we have to shut down the game and close the window
        // before we can reload and restart everything
        mprint("closing libgame");
        mylibgameclosesavegamestate(); // closes window
        mprint("unloading library");
        dlclose(handle);
        mprint("opening handle");
        openhandle();
        mprint("loading symbols");
        loadsymbols();
        mylibgameinitwithstate(g);
    }
}


void gamerun() {
    mprint("gamerun");
    mprint("opening handle");
    openhandle();
    mprint("loading symbols");
    loadsymbols();
    mprint("initing window");
    last_write_time = getlastwritetime(libname);
    mylibgameinit();
    mprint("entering gameloop");
    while (!mywindowshouldclose()) {
        mylibgamedrawframe();
        mylibgamehandleinput();
        mylibgameupdategamestate(g);
        autoreload();
    }
    mprint("closing libgame");
    mylibgameclose();
}
