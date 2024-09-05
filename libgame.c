#include "gamestate.h"
#include "mprint.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void updategamestate(gamestate* state);


void updategamestate(gamestate* g) {
    // for right now, we will just update the frame count
    if(g) {
        // we can update these while the game is running to re-position the debug text
        //memset(g->debugtxtbfr, 0, 256);
        //snprintf(g->debugtxtbfr, 256, "evildojo %d", g->framecount);


        time_t now = time(NULL);
        struct tm* tm = localtime(&now);
        char timebuf[64] = {0};
        strftime(timebuf, 64, "%Y-%m-%d %H:%M:%S", tm);

        memset(g->dp.bfr, 0, 256);
        //snprintf(g->dp.bfr,
        //         256,
        //         "framecount %d\ndate: %s\nfuck god hail satan\n@evildojo666",
        //         //"hail satan\n666\npraise the devil");
        //         g->framecount,
        //         timebuf);

        snprintf(g->dp.bfr, 256, "Present Day\n    Present Time");

        g->dp.w = 350;
        g->dp.h = 100;
        g->dp.x = g->winwidth / 2 - g->dp.w / 2;
        g->dp.y = g->winheight / 2 - g->dp.h / 2;
        //g->dp.x = 10;
        //g->dp.y = 10;
        g->dp.fontsize = 40;

        g->dp.fgcolor.r = 255;
        g->dp.fgcolor.g = 255;
        g->dp.fgcolor.b = 255;

        g->dp.bgcolor.r = rand() % 255;
        g->dp.bgcolor.g = rand() % 255;
        g->dp.bgcolor.b = rand() % 255;


        //g->debugx = 10;
        //g->debugy = 10;
        //g->fontsize = 20;
    }
}
