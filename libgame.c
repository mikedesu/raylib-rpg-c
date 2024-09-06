#include "gamestate.h"
#include "mprint.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// debug panel time string
char timebuf[64] = {0};
time_t now;
struct tm* tm;

void updategamestate(gamestate* g);
void updategamestateunsafe(gamestate* g);

void updategamestate(gamestate* g) {
    // for right now, we will just update the frame count
    if(g) {
        // we can update these while the game is running to re-position the debug text
        updategamestateunsafe(g);
    }
}

void updategamestateunsafe(gamestate* g) {
    now = time(NULL);
    tm = localtime(&now);
    memset(timebuf, 0, 64);
    memset(g->dp.bfr, 0, DEBUGPANELBUFSIZE);
    strftime(timebuf, 64, "%Y-%m-%d %H:%M:%S", tm);
    snprintf(g->dp.bfr, DEBUGPANELBUFSIZE, "framecount %d\ndate: %s\n", g->framecount, timebuf);
    g->dp.w = 350;
    g->dp.h = 100;
    g->dp.x = g->dp.y = 10;
    g->dp.fontsize = 20;
    g->dp.fgcolor.r = g->dp.fgcolor.g = g->dp.fgcolor.b = 255;
    g->dp.bgcolor.r = g->dp.bgcolor.g = g->dp.bgcolor.b = 0;
    g->dp.bgcolor.a = 128;

    //g->clearcolor.r = g->clearcolor.g = g->clearcolor.b = 255;
    //g->clearcolor.r = g->clearcolor.g = g->clearcolor.b = 0x33;
    g->clearcolor.r = g->clearcolor.g = g->clearcolor.b = 0;

    g->ts->scale = 4;
    int scale = g->ts->scale;
    g->ts->x = g->winwidth / 2 - g->ts->presents.width * scale / 2;
    g->ts->y = g->winheight / 2 - g->ts->presents.height * scale / 2;
}
