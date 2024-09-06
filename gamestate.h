#pragma once

#include "debugpanel.h"

typedef struct gamestate {
    unsigned int framecount;
    int winwidth;
    int winheight;

    mycolor clearcolor;

    //unsigned int fontsize;
    //unsigned int debugx;
    //unsigned int debugy;
    //char debugtxtbfr[256];

    debugpanel dp;

} gamestate;

#define GAMESTATESIZE (sizeof(unsigned int) * 1 + sizeof(int) * 2 + MYCOLORSIZE + DEBUGPANELSIZE)

gamestate* gamestateinit();
void gamestatefree(gamestate* gamestate);
void gamestateinitdebugpanel(gamestate* g);
