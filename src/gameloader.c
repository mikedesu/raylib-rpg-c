#include "gameloader.h"
#include "gamestate.h"
#include "mprint.h"
#include "symaddrpair.h"
//#include "libgame.h"
#include <assert.h>
#include <dlfcn.h>
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
gamestate* g = NULL;


bool (*mywindowshouldclose)() = NULL;
gamestate* (*mylibgame_getgamestate)() = NULL;
void (*mylibgameinit)() = NULL;
void (*mylibgameclosesavegamestate)() = NULL;
void (*mylibgameclose)(gamestate*) = NULL;
void (*mylibgamedrawframe)(gamestate*) = NULL;
void (*mylibgamehandle_input)(gamestate*) = NULL;

//void (*mylibgameinitwithstate)(gamestate*) = NULL;
void (*mylibgameinitwithstate)(gamestate* const) = NULL;

void (*mylibgameupdategamestate)(gamestate*) = NULL;
bool (*mylibgame_external_check_reload)() = NULL;



// get the last write time of a file
int getlastwritetime(const char* filename) {
    struct stat file_stat;
    int retval = 0;
    if (stat(filename, &file_stat) == 0) {
        retval = file_stat.st_mtime;
    }
    return retval;
}




void openhandle() {
    //minfo("openhandle");
    handle = dlopen(libname, RTLD_LAZY);
    if (!handle) {
        //merror("handle is NULL");
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        exit(1);
    }
}




void checksymbol(void* symbol, const char* name) {
    if (symbol == NULL) {
        //merror("symbol is NULL");
        fprintf(stderr, "dlsym failed: %s\n", dlerror());
        exit(1);
    }
}




void loadsymbols() {
#ifdef DEBUG
    minfo("begin loadsymbols");
#endif
    symaddrpair_t pairs[NUM_VOID_FUNCTIONS] = {
        {"libgame_drawframe", &mylibgamedrawframe},
        {"libgame_init", &mylibgameinit},
        {"libgame_close", &mylibgameclose},
        {"libgame_handle_input", &mylibgamehandle_input},
        {"libgame_closesavegamestate", &mylibgameclosesavegamestate},
        {"libgame_initwithstate", &mylibgameinitwithstate},
        {"libgame_update_gamestate", &mylibgameupdategamestate},
    };
    for (int i = 0; i < NUM_VOID_FUNCTIONS; i++) {
        //minfo(pairs[i].name);
        *pairs[i].addr = dlsym(handle, pairs[i].name);
        checksymbol(*pairs[i].addr, pairs[i].name);
    }

    char* sym = NULL;

    // gamestate return
    sym = "libgame_getgamestate";
    //minfo(sym);
    mylibgame_getgamestate = dlsym(handle, sym);
    checksymbol(mylibgame_getgamestate, sym);

    // boolean return
    sym = "libgame_windowshouldclose";
    //minfo(sym);
    mywindowshouldclose = dlsym(handle, sym);
    checksymbol(mywindowshouldclose, sym);

    // boolean return
    sym = "libgame_external_check_reload";
    mylibgame_external_check_reload = dlsym(handle, sym);
    checksymbol(mylibgame_external_check_reload, sym);

#ifdef DEBUG
    msuccess("end loadsymbols");
#endif
}




void autoreload() {
    time_t result = getlastwritetime(libname);
    if (result > last_write_time) {
        minfo("libgame.so updated, hot reloading...");
        last_write_time = result;
        while (access(lockfile, F_OK) == 0) {
            minfo("Library is locked\n");
            sleep(1);
        }
        g = mylibgame_getgamestate();
        // this time, we have to shut down the game and close the window
        // before we can reload and restart everything
        mylibgameclosesavegamestate(); // closes window
        dlclose(handle);
        openhandle();
        loadsymbols();
        mylibgameinitwithstate(g);
        msuccess("re-entering gameloop");
    }
}



void autoreload_every_n_sec(const int n) {
    assert(n > 0);
    if (g->currenttime % n == 0) {
        autoreload();
    }
}




void gamerun() {
    openhandle(); // if building for web, turn off
    loadsymbols(); // if building for web, turn off
    last_write_time = getlastwritetime(libname); // if building for web, turn off
    mylibgameinit(); // if building for web, turn off
    g = mylibgame_getgamestate();
    while (!mywindowshouldclose()) {
        mylibgameupdategamestate(g); // if building for web, turn off
        mylibgamedrawframe(g); // if building for web, turn off
        mylibgamehandle_input(g); // if building for web, turn off
        autoreload_every_n_sec(4); // if building for web, turn off
    }
    mylibgameclose(g);
}
