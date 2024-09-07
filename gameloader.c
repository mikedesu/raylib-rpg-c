#include "display.h"
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

const int default_window_width = 960;
const int default_window_height = 540;
const char* libname = "./libgame.so";
const char* lockfile = "./libgame.so.lockfile";
const char* templib = "./templibgame.so";

display mydisplay;

gamestate* g = NULL;
void* handle = NULL;

void (*myupdategamestate)(gamestate*) = NULL;
void (*myupdategamestateunsafe)(gamestate*) = NULL;

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
    myupdategamestate = (void (*)(gamestate*))dlsym(handle, "updategamestate");
    myupdategamestateunsafe = (void (*)(gamestate*))dlsym(handle, "updategamestateunsafe");
    checksymbol(myupdategamestate, "updategamestate");
    checksymbol(myupdategamestateunsafe, "updategamestateunsafe");
}


void initrendertexture() {
    mydisplay.target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    SetTextureFilter(mydisplay.target.texture, TEXTURE_FILTER_BILINEAR);
}


void myinitwindow() {
    // have to do inittitlescene after initwindow
    g = gamestateinit();
    g->winwidth = default_window_width;
    g->winheight = default_window_height;
    // cant load textures before initwindow
    InitWindow(default_window_width, default_window_height, "Game");

    g->cs = companysceneinitptr();

    SetTargetFPS(60);
    SetExitKey(KEY_Q);
    mydisplay.font = LoadFontEx("fonts/hack.ttf", 20, 0, 250);
    if (myupdategamestate == NULL) {
        fprintf(stderr, "dlsym failed or has not been loaded yet: %s\n", dlerror());
        loadsymbols();
    }
    myupdategamestate(g);

    mprint("end of myinitwindow");
}


void drawdebugpanel(gamestate* g) {
    if (g) {
        const int fontsize = 20, pad = 10;
        Color bgc = (Color){g->dp.bgcolor.r, g->dp.bgcolor.g, g->dp.bgcolor.b, g->dp.bgcolor.a},
              fgc = (Color){g->dp.fgcolor.r, g->dp.fgcolor.g, g->dp.fgcolor.b, 255},
              borderc = (Color){g->dp.fgcolor.r, g->dp.fgcolor.g, g->dp.fgcolor.b, 255};
        int w = g->dp.w, h = g->dp.h, x = g->dp.x + pad, y = g->dp.y + pad;
        DrawRectangle(x, y, w, h, bgc);
        DrawRectangleLines(x, y, w, h, borderc);
        x = g->dp.x + pad * 2, y = g->dp.y + pad * 2;
        DrawTextEx(mydisplay.font, g->dp.bfr, (Vector2){x, y}, g->dp.fontsize, 0, fgc);
    }
}


void drawcompanyscene(gamestate* g) {
    if (g) {
        companyscene* cs = g->cs;
        float w = cs->presents.width, h = cs->presents.height;
        float x = cs->x;
        float y = cs->y;
        float s = cs->scale;
        Rectangle src = {0, 0, w, h};
        Rectangle dst = {x, y, w * s, h * s};
        DrawTexturePro(g->cs->presents, src, dst, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
    }
}


void drawfade(gamestate* s) {
    if (s) {
        drawfadeunsafe(s);
    }
}


void drawfadeunsafe(gamestate* g) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, g->fadealpha});
    g->fadealpha += g->fadealphadir;
    if (g->fadealpha >= 255) {
        g->fadealphadir = -1;
    } else if (g->fadealpha <= 0) {
        g->fadealphadir = 1;
    }
}


void drawframe(gamestate* s) {
    if (s) {
        drawframeunsafe(s);
    }
}


void drawframeunsafe(gamestate* s) {
    if (s) {
        BeginDrawing();
        BeginTextureMode(mydisplay.target);
        //ClearBackground(WHITE);
        Color clearcolor =
            (Color){s->clearcolor.r, s->clearcolor.g, s->clearcolor.b, s->clearcolor.a};
        ClearBackground(clearcolor);
        drawcompanyscene(s);
        drawfade(s);

        const int dw = GetScreenWidth(), dh = GetScreenHeight(), w = mydisplay.target.texture.width,
                  h = mydisplay.target.texture.height;

        // draw a box on top of the screen
        // this box will serve as our 'fade'
        if (s->dodebugpanel) {
            drawdebugpanel(s);
            DrawFPS(dw - 100, 10);
        }
        EndTextureMode();

        Rectangle src = {0.0f, 0.0f, (float)w, (float)-h};
        Rectangle dst = {0.0f, 0.0f, (float)dw, (float)dh};

        DrawTexturePro(mydisplay.target.texture, src, dst, mydisplay.origin, 0.0f, WHITE);
        EndDrawing();
        g->framecount++;
    }
}


void handleinput(gamestate* g) {
    if (g) {
        if (IsKeyPressed(KEY_D)) {
            g->dodebugpanel = !g->dodebugpanel;
        }
    }
}


void gameloop(gamestate* g) {
    if (g) {
        while (!WindowShouldClose()) {
            //myupdategamestate(g);
            drawframe(g);
            myupdategamestateunsafe(g);
            handleinput(g);
            autoreload();
        }
    }
}


void autoreload() {
    if (getlastwritetime(libname) > last_write_time) {
        last_write_time = getlastwritetime(libname);
        while (FileExists(lockfile)) {
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


void unloaddisplay() {
    UnloadRenderTexture(mydisplay.target);
    UnloadFont(mydisplay.font);
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
    gameloop(g);
    mprint("closing window");
    gamestatefree(g);
    unloaddisplay();
    CloseWindow();
}
