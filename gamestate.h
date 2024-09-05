#pragma once

#include "debugpanel.h"

typedef struct gamestate {
    unsigned int framecount;
    //unsigned int fontsize;
    //unsigned int debugx;
    //unsigned int debugy;
    //char debugtxtbfr[256];

    debugpanel dp;

} gamestate;

#define GAMESTATESIZE (sizeof(unsigned int) * 1 + DEBUGPANELSIZE)

gamestate* gamestateinit();
void gamestatefree(gamestate* gamestate);
