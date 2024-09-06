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


const int default_window_width = 960;
const int default_window_height = 540;
const char* libname = "./libgame.so";
const char* lockfile = "./libgame.so.lockfile";
const char* templib = "./templibgame.so";

gamestate* g = NULL;
void* handle = NULL;

void (*myupdategamestate)(gamestate*) = NULL;
void (*myupdategamestateunsafe)(gamestate*) = NULL;

Font font;
RenderTexture2D target;


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


void checksymbol(void* symbol, const char* name) {
    if(symbol == NULL) {
        fprintf(stderr, "dlsym failed: %s\n", dlerror());
        exit(1);
    }
}


void loadsymbols() {
    myupdategamestate = (void (*)(gamestate*))dlsym(handle, "updategamestate");
    myupdategamestateunsafe = (void (*)(gamestate*))dlsym(handle, "updategamestateunsafe");
    checksymbol(myupdategamestate, "updategamestate");
    checksymbol(myupdategamestateunsafe, "updategamestateunsafe");
}


void initrendertexture() {
    target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
}


void myinitwindow() {
    // have to do inittitlescene after initwindow
    g = gamestateinit();
    g->winwidth = default_window_width;
    g->winheight = default_window_height;
    // cant load textures before initwindow
    InitWindow(default_window_width, default_window_height, "Game");

    g->ts = titlesceneinitptr();

    SetTargetFPS(60);
    SetExitKey(KEY_Q);
    font = LoadFontEx("fonts/hack.ttf", 20, 0, 250);
    if(myupdategamestate == NULL) {
        fprintf(stderr, "dlsym failed or has not been loaded yet: %s\n", dlerror());
        loadsymbols();
    }
    myupdategamestate(g);

    mprint("end of myinitwindow");
}


void drawdebugpanel() {
    const int fontsize = 20, pad = 10;
    Color bgc = (Color){g->dp.bgcolor.r, g->dp.bgcolor.g, g->dp.bgcolor.b, g->dp.bgcolor.a},
          fgc = (Color){g->dp.fgcolor.r, g->dp.fgcolor.g, g->dp.fgcolor.b, 255},
          borderc = (Color){g->dp.fgcolor.r, g->dp.fgcolor.g, g->dp.fgcolor.b, 255};
    int w = g->dp.w, h = g->dp.h, x = g->dp.x + pad, y = g->dp.y + pad;
    DrawRectangle(x, y, w, h, bgc);
    DrawRectangleLines(x, y, w, h, borderc);

    x = g->dp.x + pad * 2, y = g->dp.y + pad * 2;
    DrawTextEx(font, g->dp.bfr, (Vector2){x, y}, g->dp.fontsize, 0, fgc);
}


void drawtitlescene() {
    titlescene* ts = g->ts;
    float w = ts->presents.width, h = ts->presents.height;
    float z = 0.0f;
    Rectangle src = {z, z, w, h};
    w = w * 4, h = h * 4;
    Rectangle dst = {z, z, w, h};
    DrawTexturePro(g->ts->presents, src, dst, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}


void drawframe() {
    BeginDrawing();
    BeginTextureMode(target);
    //ClearBackground(WHITE);
    Color clearcolor = (Color){g->clearcolor.r, g->clearcolor.g, g->clearcolor.b, g->clearcolor.a};
    ClearBackground(clearcolor);
    drawtitlescene();
    drawdebugpanel();
    DrawFPS(GetScreenWidth() - 100, 10);
    EndTextureMode();
    DrawTexturePro(
        target.texture,
        (Rectangle){0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height},
        (Rectangle){0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight()},
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE);
    EndDrawing();
    g->framecount++;
}


void gameloop() {
    while(!WindowShouldClose()) {
        //myupdategamestate(g);
        myupdategamestateunsafe(g);
        drawframe();
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
        mprint("unloading library");
        dlclose(handle);
        mprint("opening handle");
        openhandle();
        mprint("loading symbols");
        loadsymbols();
    }
}


void gamerun() {
    mprint("gamerun");
    mprint("opening handle");
    openhandle();
    mprint("loading symbols");
    loadsymbols();
    mprint("initing window");
    myinitwindow();
    mprint("initing rendertexture");
    initrendertexture();
    mprint("entering gameloop");
    gameloop();
    mprint("closing window");
    gamestatefree(g);
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


//void game_gamestate_destroy(gamestate* gamestate) {
//    if(gamestate) {
//        free(gamestate);
//    }
//}
