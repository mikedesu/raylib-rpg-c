#include "gameloader.h"
#include "gamestate.h"
#include "mprint.h"
#include <dlfcn.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

char frame_count_buffer[30] = {0};

const int default_window_width = 1280;
const int default_window_height = 720;

gamestate* g = NULL;

long last_write_time = 0;

const char* libname = "./libgame.so";
const char* lockfile = "./libgame.so.lockfile";
const char* templib = "./templibgame.so";

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
    //myupdategamestate = (void (*)())dlsym(handle, "updategamestate");
    myupdategamestate = (void (*)(gamestate*))dlsym(handle, "updategamestate");
    //MyGameRun = dlsym(handle, "GameRun");
    //if(MyGame_gamestate_destroy == NULL || MyGameRun == NULL) {
    if(myupdategamestate == NULL) {
        fprintf(stderr, "dlsym failed: %s\n", dlerror());
        exit(1);
    }
}

void myinitwindow() {
    InitWindow(default_window_width, default_window_height, "Game");
    SetTargetFPS(60);
    g = gamestate_init();
    updateframecountbuffer();
}

void myinitwindowwithgamestate(gamestate* state) {
    mprint("myinitwindowwithgamestate");
    InitWindow(default_window_width, default_window_height, "Game");
    SetTargetFPS(60);
    mprint("freeing old gamestate");
    if(g != NULL) {
        gamestate_destroy(g);
    }
    mprint("setting new gamestate");
    g = state;
    mprint("updating frame count buffer");
    updateframecountbuffer();
    mprint("done with myinitwindowwithgamestatex");
}

void updateframecountbuffer() {
    sprintf(frame_count_buffer, "666: %d", g->framecount);
}

void drawframe() {
    BeginDrawing();
    Color bgc = BLACK;
    Color fgc = WHITE;
    ClearBackground(bgc);
    const int fontsize = 60;
    DrawText(frame_count_buffer, GetScreenWidth() / 4, GetScreenHeight() / 4, fontsize, fgc);
    DrawFPS(GetScreenWidth() - 100, 10);
    EndDrawing();
}

void gameloop() {
    while(!WindowShouldClose()) {
        myupdategamestate(g);
        drawframe();
        updateframecountbuffer();
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
        //mprint("getting old gamestate");
        //gamestate* old_g = g;
        // dont need to close window this time
        mprint("unloading library");
        dlclose(handle);
        mprint("opening handle");
        openhandle();
        mprint("loading symbols");
        loadsymbols();
        //mprint("reloading window with old gamestate");
        //myinitwindowwithgamestate(old_g);
    }
}

//        old_gamestate = MyGame_get_gamestate();
//        mPrint("Closing window and unloading library");
//        MyCloseWindow();
//        mPrint("Unloading library");
//        dlclose(handle);
//        mPrint("Opening handle");
//        OpenHandle();
//        mPrint("Loading symbols");
//        LoadSymbols();
//        mPrint("Reloading window with old gamestate");
//        MyInitWindowWithGamestate(old_gamestate);
//    }
//}

void gamerun() {
    //mprint("gamerun");
    //mprint("initing window");
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
    if(gamestate)
        free(gamestate);
}
