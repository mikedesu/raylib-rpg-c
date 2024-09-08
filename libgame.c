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

void updategamestate(gamestate* g) {
    // for right now, we will just update the frame count
    if (g) {
        // we can update these while the game is running to re-position the debug text
        updategamestateunsafe(g);
    }
}

void updategamestateunsafe(gamestate* g) {
    //    mprint("updategamestateunsafe");
    now = time(NULL);
    if (start == 0) {
        start = now;
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
             "framecount:   %d\nstart date:   %s\ncurrent date: %s\ncamera3d:    %f %f %f\n",
             g->framecount,
             timebuf2,
             timebuf,
             g->cs->cam3d.position.x,
             g->cs->cam3d.position.y,
             g->cs->cam3d.position.z);
    g->dp.w = 350;
    g->dp.h = 80;
    g->dp.x = 5;
    g->dp.y = 5;
    g->dp.fontsize = 16;

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

    g->cs->cubecolor = (Color){0x33, 0x33, 0x33, 255};


    g->cs->cam3d.position.z += 0.05f;
    g->cs->cam3d.target.z += 0.05f;
    if (g->cs->cam3d.position.z > 25.0f) {
        g->cs->cam3d.position.z = 0.0f;
        g->cs->cam3d.target.z = 0.0f;
    }


    g->cs->scale = 4;
    const int scale = g->cs->scale;
    g->cs->x = g->winwidth / 2 - g->cs->presents.width * scale / 2;
    g->cs->y = g->winheight / 2 - g->cs->presents.height * scale / 2;
}
