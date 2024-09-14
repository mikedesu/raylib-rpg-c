//#include "companyscene.h"
//#include "debugpanel.h"
#include "gamestate.h"
#include "mprint.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// have to update this function when we introduce new fields to Gamestate
gamestate* gamestateinitptr() {
    mprint("gamestateinit\n");
    gamestate* g = (gamestate*)malloc(sizeof(gamestate));
    if (g == NULL) {
        fprintf(stderr, "Failed to allocate memory for gamestate: %s\n", strerror(errno));
        return NULL;
    }

    //g->dodebugpanel = true;
    //g->dofps = false;
    g->framecount = 0;
    g->timebegan = time(NULL);
    g->timebegantm = localtime(&(g->timebegan));
    bzero(g->timebeganbuf, 64);
    strftime(g->timebeganbuf, 64, "Start Time:   %Y-%m-%d %H:%M:%S", g->timebegantm);

    gamestateupdatecurrenttime(g);

    g->dp.x = 0;
    g->dp.y = 0;

    //g->currenttime = time(NULL);
    //g->currenttimetm = localtime(&(g->currenttime));
    //bzero(g->currenttimebuf, 64);
    //strftime(g->currenttimebuf, 64, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);

    //g->winwidth = 0;
    //g->winheight = 0;
    //g->clearcolor = (mycolor){0, 0, 0, 255};
    //g->fadealpha = 255;
    //g->fadealphadir = -1;
    //g->currentscene = SCENE_COMPANY;
    //g->starttime = time(NULL);
    //g->starttm = localtime(&(g->starttime));

    //gamestateinitdebugpanel(g);

    //g->cs = NULL;

    //gamestateinitcompanyscene(g);

    return g;
}


void gamestateupdatecurrenttime(gamestate* g) {
    if (g == NULL) {
        fprintf(stderr, "gamestateupdatecurrenttime: gamestate is NULL\n");
        return;
    }

    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&(g->currenttime));
    bzero(g->currenttimebuf, 64);
    strftime(g->currenttimebuf, 64, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
}


//void gamestateinitdebugpanel(gamestate* g) {
//    if (g == NULL) {
//        fprintf(stderr, "gamestateinitdebugpanel: gamestate is NULL\n");
//        return;
//    }
//g->dp.x = 0;
//g->dp.y = 0;
//g->dp.w = 200;
//g->dp.h = 100;
//g->dp.fontsize = 20;

//g->dp.fgcolor.r = 255;
//g->dp.fgcolor.g = 255;
//g->dp.fgcolor.b = 255;
//g->dp.fgcolor.a = 255;

//g->dp.bgcolor.r = 0x66;
//g->dp.bgcolor.g = 0x66;
//g->dp.bgcolor.b = 0x66;
//g->dp.bgcolor.a = 255;

//bzero(g->dp.bfr, DEBUGPANELBUFSIZE);
//}


//void gamestateinitcompanyscene(gamestate* g) {
//    if (g == NULL) {
//        fprintf(stderr, "gamestateinitcompanyscene: gamestate is NULL\n");
//        return;
//    }

//g->cs = companysceneinitptr();
//if (g->cs == NULL) {
//    fprintf(stderr, "gamestateinitcompanyscene: companyscene is NULL\n");
//    return;
//}
//}


// have to update this function when we introduce new fields to Gamestate
void gamestatefree(gamestate* s) {
    if (s != NULL) {
        //companyscenefree(s->cs);
        //free(s->timebegantm);
        free(s);
    }
}
