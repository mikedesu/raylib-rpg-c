#include "gamestate.h"
#include "mprint.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// debug panel time string
char timebuf[64] = {0};
char timebuf2[64] = {0};
time_t now = 0;
time_t start = 0;
struct tm* tm;
struct tm* tm2;

void updategamestate(gamestate* g);
void updategamestateunsafe(gamestate* g);
void setdebugpaneltopleft(gamestate* g);
void setdebugpanelbottomleft(gamestate* g);
void setdebugpanelbottomright(gamestate* g);
void setdebugpaneltopright(gamestate* g);


void updategamestate(gamestate* g) {
    // for right now, we will just update the frame count
    if (g) {
        // we can update these while the game is running to re-position the debug text
        updategamestateunsafe(g);
    }
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


void updategamestateunsafe(gamestate* g) {
    //    mprint("updategamestateunsafe");
    now = time(NULL);
    if (start == 0) {
        //start = now;
        start = g->starttime;
        tm2 = localtime(&start);
        memset(timebuf2, 0, 64);
        strftime(timebuf2, 64, "%Y-%m-%d %H:%M:%S", tm2);
    }
    tm = localtime(&now);
    memset(timebuf, 0, 64);
    memset(g->dp.bfr, 0, DEBUGPANELBUFSIZE);
    strftime(timebuf, 64, "%Y-%m-%d %H:%M:%S", tm);


    snprintf(g->dp.bfr,
             DEBUGPANELBUFSIZE,
             "framecount:   %d\n"
             "start date:   %s\n"
             "current date: %s\n"
             "camera3d.position: %.2f %.2f %.2f\n"
             "camera3d.target:   %.2f %.2f %.2f\n",
             g->framecount,
             timebuf2,
             timebuf,
             g->cs->cam3d.position.x,
             g->cs->cam3d.position.y,
             g->cs->cam3d.position.z,
             g->cs->cam3d.target.x,
             g->cs->cam3d.target.y,
             g->cs->cam3d.target.z);
    g->dp.w = 350;
    g->dp.h = 120;

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
    g->cs->cubecolor = (Color){0x66, 0x66, 0x66, 255};
    g->cs->cubewirecolor = (Color){0x33, 0x33, 0x33, 255};
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
    static Vector3 cubev = {0.05f, 0.00f, 0.01f};

    g->cs->cubepos.x += cubev.x;
    g->cs->cubepos.y += cubev.y;
    g->cs->cubepos.z += cubev.z;

    if (g->cs->cubepos.x > 4.5f) {
        cubev.x = -cubev.x;
    } else if (g->cs->cubepos.x < -4.5f) {
        cubev.x = -cubev.x;
    }

    //if (g->cs->cubepos.y > 5.0f) {
    //    cubev.y = -cubev.y;
    //} else if (g->cs->cubepos.y < -5.0f) {
    //    cubev.y = -cubev.y;
    //}

    if (g->cs->cubepos.z > 4.5f) {
        cubev.z = -cubev.z;
    } else if (g->cs->cubepos.z < -4.5f) {
        cubev.z = -cubev.z;
    }


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
    g->cs->cam3d.position = (Vector3){0.0f, 4.0f, 10.0f};
    g->cs->cam3d.target = (Vector3){0.0f, 0.0f, 0.0f};
    g->cs->cam3d.target = (Vector3){0.0f, 0.0f, 0.0f};
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

    g->cs->x = g->winwidth / 2 - g->cs->presents.width * scale / 2;
    g->cs->y = g->winheight / 2 - g->cs->presents.height * scale / 2;


    //g->cs->dodrawpresents = false;
    g->cs->dodrawpresents = true;
}
