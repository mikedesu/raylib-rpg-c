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

//#define GAMESTATESIZE                                                                              \
//    (sizeof(bool) + sizeof(int) * 5 + MYCOLORSIZE + DEBUGPANELSIZE + sizeof(companyscene*) +       \
     DISPLAYSIZE + sizeof(scene))


//#define GAMESTATESIZE                                                                              \
//    (SIZEOFBOOL(2) + SIZEOFINT(5) + MYCOLORSIZE + DEBUGPANELSIZE + sizeof(companyscene*) +         \
//     DISPLAYSIZE + sizeof(sceneint) + sizeof(time_t) + sizeof(struct tm*))


gamestate* gamestateinit();
void gamestatefree(gamestate* g);
void gamestateinitdebugpanel(gamestate* g);
