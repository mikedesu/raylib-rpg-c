#pragma once

#include "companyscene.h"
#include "debugpanel.h"
#include "display.h"
#include "sceneint.h"
#include "sizemacros.h"
#include <stdbool.h>
#include <time.h>

typedef struct gamestate {
    bool dodebugpanel;
    bool dofps;

    int framecount;
    int winwidth;
    int winheight;
    int fadealpha;
    int fadealphadir;

    mycolor clearcolor;

    debugpanel dp;

    display d;

    sceneint currentscene;

    companyscene* cs;

    time_t starttime;
    struct tm* starttm;

} gamestate;


gamestate* gamestateinitptr();
void gamestatefree(gamestate* g);
void gamestateinitdebugpanel(gamestate* g);
