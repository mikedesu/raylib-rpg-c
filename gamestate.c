#include "gamestate.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// have to update this function when we introduce new fields to Gamestate
gamestate* gamestateinit() {
    gamestate* g = (gamestate*)malloc(GAMESTATESIZE);
    if(g == NULL) {
        fprintf(stderr, "Failed to allocate memory for gamestate: %s\n", strerror(errno));
        return NULL;
    }

    g->framecount = 0;
    g->winwidth = 0;
    g->winheight = 0;
    gamestateinitdebugpanel(g);

    return g;
}


void gamestateinitdebugpanel(gamestate* g) {
    if(g == NULL) {
        fprintf(stderr, "gamestateinitdebugpanel: gamestate is NULL\n");
        return;
    }

    g->dp.x = 0;
    g->dp.y = 0;
    g->dp.w = 200;
    g->dp.h = 100;
    g->dp.fontsize = 20;

    g->dp.fgcolor.r = 255;
    g->dp.fgcolor.g = 255;
    g->dp.fgcolor.b = 255;

    g->dp.bgcolor.r = 0x66;
    g->dp.bgcolor.g = 0x66;
    g->dp.bgcolor.b = 0x66;

    bzero(g->dp.bfr, 256);
}


// have to update this function when we introduce new fields to Gamestate
void gamestatefree(gamestate* state) {
    if(state == NULL) {
        fprintf(stderr, "Gamestate_destroy: gamestate is NULL\n");
        return;
    }
    free(state);
}
