#include "gameloader.h"
#include "gamestate.h"
#include "mprint.h"
#include <dlfcn.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

long last_write_time = 0;
//char frame_count_buffer[30] = {0};


const int default_window_width = 800;
const int default_window_height = 480;
const char* libname = "./libgame.so";
const char* lockfile = "./libgame.so.lockfile";
const char* templib = "./templibgame.so";


gamestate* g = NULL;
void* handle = NULL;


void (*myupdategamestate)(gamestate*) = NULL;


// get the last write time of a file
time_t getlastwritetime(const char* filename) {
    struct stat file_stat;
    time_t retval = 0;
    if(stat(filename, &file_stat) == 0) {
        retval = file_stat.st_mtime;
    }
    return retval;
}


void openhandle() {
    handle = dlopen(libname, RTLD_LAZY);
    if(!handle) {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        exit(1);
    }
}


void loadsymbols() {
    myupdategamestate = (void (*)(gamestate*))dlsym(handle, "updategamestate");
    if(myupdategamestate == NULL) {
        fprintf(stderr, "dlsym failed: %s\n", dlerror());
        exit(1);
    }
}


void myinitwindow() {
    InitWindow(default_window_width, default_window_height, "Game");
    //const int pad = 50;
    //const int x = GetMonitorWidth(GetCurrentMonitor()) / 2 - default_window_width / 2 + 500;
    //const int y = GetMonitorHeight(GetCurrentMonitor()) / 2 - default_window_height / 2;
    //const int y = pad;
    //SetWindowPosition(x, y);
    SetTargetFPS(60);
    SetExitKey(KEY_Q);

    g = gamestateinit();

    //snprintf(g->debugtxtbfr, 256, "framecount: %d", g->framecount);
    snprintf(g->dp.bfr, 256, "framecount: %d", g->framecount);

    //updateframecountbuffer();
}


void drawdebugpanel() {
    const int fontsize = 20;

    Color bgc = DARKGRAY;
    Color fgc = WHITE;

    const int pad = 10;
    int x = g->dp.x + pad;
    int y = g->dp.y + pad;
    const int w = g->dp.w;
    const int h = g->dp.h;

    DrawRectangle(x, y, w, h, bgc);
    //DrawText(g->dp.bfr, g->dp.x, g->dp.y, fontsize, fgc);

    x = g->dp.x + pad * 2;
    y = g->dp.y + pad * 2;

    DrawText(g->dp.bfr, x, y, fontsize, fgc);
}


void drawframe() {
    BeginDrawing();
    Color bgc = BLACK;
    ClearBackground(bgc);

    drawdebugpanel();

    DrawFPS(GetScreenWidth() - 100, 10);

    EndDrawing();
}


void gameloop() {
    while(!WindowShouldClose()) {
        myupdategamestate(g);
        drawframe();
        //updateframecountbuffer();
        //snprintf(g->debugtxtbfr, 256, "framecount: %d", g->framecount);

        autoreload();
    }
}


void autoreload() {
    if(getlastwritetime(libname) > last_write_time) {
        last_write_time = getlastwritetime(libname);
        while(FileExists(lockfile)) {
            printf("Library is locked\n");
            sleep(1);
        }
        // mprint("getting old gamestate");
        // gamestate* old_g = g;
        //  dont need to close window this time
        mprint("unloading library");
        dlclose(handle);
        mprint("opening handle");
        openhandle();
        mprint("loading symbols");
        loadsymbols();
        // mprint("reloading window with old gamestate");
        // myinitwindowwithgamestate(old_g);
    }
}


void gamerun() {
    // mprint("gamerun");
    // mprint("initing window");
    myinitwindow();
    openhandle();
    loadsymbols();
    mprint("entering gameloop");
    gameloop();
    mprint("closing window");
    CloseWindow();
}


bool mywindowshouldclose() {
    return WindowShouldClose();
}


bool myiskeypressed(int key) {
    return IsKeyPressed(key);
}


unsigned int getframecount() {
    return g->framecount;
}


gamestate* game_get_gamestate() {
    return g;
}


void game_gamestate_destroy(gamestate* gamestate) {
    if(gamestate) {
        free(gamestate);
    }
}
