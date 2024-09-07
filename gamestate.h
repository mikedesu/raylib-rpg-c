#pragma once

#include "companyscene.h"
#include "debugpanel.h"
#include "display.h"
#include <stdbool.h>

typedef struct gamestate {
    bool dodebugpanel;

    int framecount;
    int winwidth;
    int winheight;

    mycolor clearcolor;

    int fadealpha;
    int fadealphadir;

    debugpanel dp;

    companyscene* cs;
    //companyscene ts;
    //

    display d;

} gamestate;

//#define GAMESTATESIZE                                                                              \
//    (sizeof(bool) + sizeof(int) * 5 + MYCOLORSIZE + DEBUGPANELSIZE + sizeof(companyscene*))

#define GAMESTATESIZE                                                                              \
    (sizeof(bool) + sizeof(int) * 5 + MYCOLORSIZE + DEBUGPANELSIZE + sizeof(companyscene*) +       \
     DISPLAYSIZE)


//#define GAMESTATESIZE (sizeof(int) * 3 + MYCOLORSIZE + DEBUGPANELSIZE + sizeof(companyscene))
//#define GAMESTATESIZE (sizeof(int) * 3 + MYCOLORSIZE + DEBUGPANELSIZE)

gamestate* gamestateinit();
void gamestatefree(gamestate* g);
void gamestateinitdebugpanel(gamestate* g);
