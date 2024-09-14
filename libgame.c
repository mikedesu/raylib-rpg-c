#include "gamestate.h"
#include "mprint.h"
//#include "utils.h"

#include <raylib.h>
#include <rlgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//--------------------------------------------------------------------
// libgame global variables
//--------------------------------------------------------------------

// debug panel time string
char timebeganbuf[64] = {0};
time_t timebegan = 0;
struct tm* timebegantm;
//char timebuf2[64] = {0};
//time_t now = 0;
//struct tm* tm2;

char debugpanelbuffer[1024] = {0};

gamestate* g = NULL;

Font gfont;

unsigned int framecount = 0;

int activescene = 0;

//--------------------------------------------------------------------
// function declarations
//--------------------------------------------------------------------

bool gamewindowshouldclose();

void gameinitwindow();
void gameclosewindow();
void updategamestate();
void drawframe();
void libgameinit();
void libgameclose();
void drawdebugpanel();
void libgamehandleinput();
void drawcompanysceneframe();
void drawtitlesceneframe();

void setdebugpaneltopleft(gamestate* g);
void setdebugpanelbottomleft(gamestate* g);
void setdebugpanelbottomright(gamestate* g);
void setdebugpaneltopright(gamestate* g);
void gameinitframecount(unsigned int fc);

unsigned int saveframecount();

//--------------------------------------------------------------------
// definitions
//--------------------------------------------------------------------

void libgamehandleinput() {
    if (IsKeyPressed(KEY_SPACE)) {
        mprint("key space pressed");
        activescene++;
        if (activescene > 1) {
            activescene = 0;
        }
    }
}


bool gamewindowshouldclose() {
    return WindowShouldClose();
}


void gameinitwindow() {
    const char* title = "project rpg v0.000001";
    const int w = 1920 / 2;
    const int h = 1080 / 2;
    mprint("begin gameinitwindow");
    // have to do inittitlescene after initwindow
    // cant load textures before initwindow
    InitWindow(w, h, title);

    while (!IsWindowReady())
        ;


    // this is hard-coded for now so we can auto-position the window
    // for easier config during streaming
    SetWindowMonitor(0);
    SetWindowPosition(1920, 0);
    //SetWindowPosition(0, 0);
    SetTargetFPS(60);
    SetExitKey(KEY_Q);
    mprint("end of gameinitwindow");
}


void gameinitframecount(unsigned int fc) {
    framecount = fc;
}


void gameclosewindow() {
    //rlglClose();
    CloseWindow();
}


void setdebugpaneltopleft(gamestate* g) {
    g->dp.x = 5;
    g->dp.y = 5;
}


void setdebugpanelbottomleft(gamestate* g) {
    g->dp.x = 5;
    g->dp.y = g->winheight - g->dp.h - 20;
}


void setdebugpanelbottomright(gamestate* g) {
    g->dp.x = g->winwidth - g->dp.w - 20;
    g->dp.y = g->winheight - g->dp.h - 20;
}


void setdebugpaneltopright(gamestate* g) {
    g->dp.x = g->winwidth - g->dp.w - 20;
    g->dp.y = 5;
}


void updategamestate() {
    //    mprint("updategamestateunsafe");
    //now = time(NULL);
    //if (start == 0) {
    //    //start = now;
    //    start = g->starttime;
    //    tm2 = localtime(&start);
    //    memset(timebuf2, 0, 64);
    //    strftime(timebuf2, 64, "%Y-%m-%d %H:%M:%S", tm2);
    //}
    //tm = localtime(&now);
    //memset(timebuf, 0, 64);
    //memset(g->dp.bfr, 0, DEBUGPANELBUFSIZE);
    //strftime(timebuf, 64, "%Y-%m-%d %H:%M:%S", tm);
    //snprintf(g->dp.bfr,
    //         DEBUGPANELBUFSIZE,
    //         "framecount:   %d\n"
    //         "start date:   %s\n"
    //         "current date: %s\n"
    //         "camera3d.pos: %.2f %.2f %.2f\n"
    //         "camera3d.target:   %.2f %.2f %.2f\n"
    //         "camera3d.proj: %d\ncameramode: %d\n",
    //         g->framecount,
    //         timebuf2,
    //         timebuf,
    //         g->cs->cam3d.position.x,
    //         g->cs->cam3d.position.y,
    //         g->cs->cam3d.position.z,
    //         g->cs->cam3d.target.x,
    //         g->cs->cam3d.target.y,
    //         g->cs->cam3d.target.z,
    //         g->cs->cam3d.projection,
    //         g->cs->cameramode);
    g->dp.w = 350;
    g->dp.h = 200;
    // top left
    //setdebugpaneltopleft(g);
    // top right
    setdebugpaneltopright(g);
    // bottom left
    //setdebugpanelbottomleft(g);
    // bottom right
    //setdebugpanelbottomright(g);
    g->dp.fontsize = 20;
    g->dp.fgcolor.r = 255;
    g->dp.fgcolor.g = 255;
    g->dp.fgcolor.b = 255;
    g->dp.bgcolor.r = 0x33;
    g->dp.bgcolor.g = 0x33;
    g->dp.bgcolor.b = 0x33;
    g->dp.bgcolor.a = 255;
    g->clearcolor.r = 0;
    g->clearcolor.g = 0;
    g->clearcolor.b = 0;
    //g->cs->cubecolor = (Color){0x33, 0x33, 0x33, 255};
    //g->cs->cubecolor = (Color){0x66, 0x66, 0x66, 255};
    //g->cs->cubewirecolor = (Color){0x33, 0x33, 0x33, 255};
    //g->cs->cubecolor = (Color){255, 255, 255, 255};
    //g->cs->cubepos = (Vector3){-1.5f, 0.5f, 4.5f};
    //if (g->framecount % 120 == 0) {
    //    g->cs->cubepos =
    //        (Vector3){GetRandomValue(-4, 4) + 0.5f, 0.5f, GetRandomValue(-4, 4) + 0.5f};
    //    g->cs->cubecolor =
    //        (Color){GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255};
    //}
    //g->cs->cubepos = (Vector3){0.0f, 0.5f, 1.0f};
    //static float cubedirx = 0.01f;
    //static Vector3 cubev = {0.05f, 0.00f, 0.01f};
    //g->cs->cubepos.x = 0.05f;
    //g->cs->cubepos.y = 0.0f;
    //g->cs->cubepos.z = 0.01f;
    //g->cs->cubepos = (Vector3){0.0f, 0.5f, 1.0f};
    //g->cs->cubepos.x += cubev.x;
    //g->cs->cubepos.y += cubev.y;
    //g->cs->cubepos.z += cubev.z;
    //if (g->cs->cubepos.x > 4.5f) {
    //    cubev.x = -cubev.x;
    //} else if (g->cs->cubepos.x < -4.5f) {
    //    cubev.x = -cubev.x;
    //}
    //if (g->cs->cubepos.y > 5.0f) {
    //    cubev.y = -cubev.y;
    //} else if (g->cs->cubepos.y < -5.0f) {
    //    cubev.y = -cubev.y;
    //}
    //if (g->cs->cubepos.z > 4.5f) {
    //    cubev.z = -cubev.z;
    //} else if (g->cs->cubepos.z < -4.5f) {
    //    cubev.z = -cubev.z;
    //}
    //static float dpx = 0.00f;
    //static float dtx = 0.00f;
    //static float dpy = 0.01f;
    //static float dty = 0.01f;
    //static float dpz = 0.00f;
    //static float dtz = 0.00f;
    //const float ymax = 10.0f;
    //const float ymin = 0.0f;
    //if (g->cs->cam3d.position.y >= 0.0f || g->cs->cam3d.position.y < 2.0f) {
    //    if (dty < 0) {
    //        dty = -0.005f;
    //    } else if (dty > 0) {
    //        dty = 0.005f;
    //    }
    //}
    // g->cs->cam3d.position = (Vector3){10.0f, 10.0f, -10.0f};
    //g->cs->cam3d.position = (Vector3){-10.0f, 5.0f, 9.0f};
    //g->cs->cam3d.position = (Vector3){-3.0f, 5.0f, 5.0f};
    //g->cs->cam3d.target = (Vector3){0.0f, 0.0f, 0.0f};
    //g->cs->cam3d.target = g->cs->cubepos;
    //g->cs->cam3d.target = (Vector3){0.0f, 0.0f, 0.0f};
    //g->cs->cam3d.position.x = 0.0f;
    //g->cs->cam3d.target.x = 0.0f;
    //g->cs->cam3d.position.y = 10.0f;
    //g->cs->cam3d.target.y = 0.0f;
    //g->cs->cam3d.position.z = 5.0f;
    //g->cs->cam3d.target.z = 0.0f;
    //g->cs->cam3d.position.y += dpy;
    //g->cs->cam3d.target.y += dty;
    //g->cs->cam3d.position.z += dz;
    //g->cs->cam3d.target.z += dz;
    //if (g->cs->cam3d.position.y > ymax || g->cs->cam3d.position.y < ymin) {
    //    dpy = -dpy;
    //    dty = -dty;
    //}
    //if (g->cs->cam3d.target.y > ymax || g->cs->cam3d.target.y < ymin) {
    //    dty = -dty;
    //}
    g->cs->scale = 4;
    const int scale = g->cs->scale;
    //g->cs->x = g->winwidth / 2 - g->cs->presents.width * scale / 2;
    //g->cs->y = g->winheight / 2 - g->cs->presents.height * scale / 2;
    g->cs->dodrawpresents = false;
    //g->cs->dodrawpresents = true;
}


void drawframe() {

    if (activescene == 0) {
        drawcompanysceneframe();
    } else if (activescene == 1) {
        drawtitlesceneframe();
    }

    //drawtitlesceneframe();
    //drawcompanysceneframe();

    //framecount = 0;
    framecount++;
}


void drawdebugpanel() {
    Color fgc = WHITE;
    Color bgc = {0, 0, 0, 128};
    Vector2 pos = (Vector2){10, 10};
    const int fontsize = 20;
    const int spacing = 1;

    bzero(debugpanelbuffer, 1024);
    snprintf(debugpanelbuffer, 1024, "framecount: %d", framecount);
    DrawTextEx(gfont, debugpanelbuffer, (Vector2){10, 10}, fontsize, spacing, fgc);
}


void drawtitlesceneframe() {
    BeginDrawing();
    const Color bgc = {0x66, 0x66, 0x66, 255};
    ClearBackground(bgc);
    const Color fgc = WHITE;
    const Color fgc2 = BLACK;
    char buffer[1024];
    char buffer2[1024];
    //snprintf("project.rpg", 1024, buffer); // lol copilot generated something very wrong lmfao
    //
    snprintf(buffer, 1024, "project");
    snprintf(buffer2, 1024, "rpg");
    Vector2 measurement = MeasureTextEx(gfont, buffer, 40, 2);
    Vector2 measurement2 = MeasureTextEx(gfont, buffer2, 40, 2);
    int x = GetScreenWidth() / 2.0f - measurement.x / 2.0f - 100;
    int y = GetScreenHeight() / 2.0f - measurement.y / 2.0f;
    Vector2 pos = (Vector2){x, y};
    x = GetScreenWidth() / 2.0f - measurement2.x / 2.0f + 100;
    y = GetScreenHeight() / 2.0f - measurement2.y / 2.0f;
    Vector2 pos2 = (Vector2){x, y};
    DrawTextEx(gfont, buffer, pos, 40, 4, fgc);
    DrawTextEx(gfont, buffer2, pos2, 40, 1, fgc2);
    drawdebugpanel();
    EndDrawing();
}


void drawcompanysceneframe() {
    BeginDrawing();
    const Color bgc = BLACK;
    const Color fgc = {0x66, 0x66, 0x66, 255};
    const int fontsize = 24;
    const int spacing = 1;
    ClearBackground(bgc);
    drawdebugpanel();
#define COMPANYNAME "@evildojo666"
    Vector2 m = MeasureTextEx(gfont, COMPANYNAME, fontsize, spacing);
    Vector2 p = {GetScreenWidth() / 2.0f - m.x / 2.0f, GetScreenHeight() / 2.0f - m.y / 2.0f};
    DrawTextEx(gfont, COMPANYNAME, p, fontsize, 1, fgc);
    EndDrawing();
}


void libgameinit() {
    mprint("libgameinit");
    gameinitwindow();
    gfont = LoadFont("fonts/hack.ttf");
    framecount = 0;
    SetTextureFilter(gfont.texture, TEXTURE_FILTER_BILINEAR);
    g = gamestateinitptr();
}


void libgameclose() {
    mprint("libgameclose");
    UnloadFont(gfont);
    gamestatefree(g);
    mprint("closing window");
    CloseWindow();
}


unsigned int saveframecount() {
    return framecount;
}
