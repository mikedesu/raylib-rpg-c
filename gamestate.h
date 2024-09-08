#pragma once

#include "companyscene.h"
#include "debugpanel.h"
#include "display.h"
#include "scenes.h"
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


    scene currentscene;

    companyscene* cs;

    display d;

} gamestate;

#define GAMESTATESIZE                                                                              \
    (sizeof(bool) + sizeof(int) * 5 + MYCOLORSIZE + DEBUGPANELSIZE + sizeof(companyscene*) +       \
     DISPLAYSIZE + sizeof(scene))

gamestate* gamestateinit();
void gamestatefree(gamestate* g);
void gamestateinitdebugpanel(gamestate* g);
