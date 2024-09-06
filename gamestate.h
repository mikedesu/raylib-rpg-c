#pragma once

#include "debugpanel.h"
#include "titlescene.h"

typedef struct gamestate {
    int framecount;
    int winwidth;
    int winheight;

    mycolor clearcolor;

    debugpanel dp;

    titlescene* ts;
    //titlescene ts;

} gamestate;

#define GAMESTATESIZE (sizeof(int) * 3 + MYCOLORSIZE + DEBUGPANELSIZE + sizeof(titlescene*))
//#define GAMESTATESIZE (sizeof(int) * 3 + MYCOLORSIZE + DEBUGPANELSIZE + sizeof(titlescene))
//#define GAMESTATESIZE (sizeof(int) * 3 + MYCOLORSIZE + DEBUGPANELSIZE)

gamestate* gamestateinit();
void gamestatefree(gamestate* g);
void gamestateinitdebugpanel(gamestate* g);
