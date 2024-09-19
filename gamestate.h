#pragma once

#include "controlmode.h"
#include "debugpanel.h"
#include "display.h"
#include "fadestate.h"
#include "sizemacros.h"
#include <raylib.h>
#include <stdbool.h>
#include <time.h>

#define GAMESTATE_SIZEOFTIMEBUF 64
#define GAMESTATE_SIZEOFDEBUGPANELBUF 1024

typedef struct gamestate {
    int framecount;

    time_t timebegan;
    char timebeganbuf[GAMESTATE_SIZEOFTIMEBUF];
    struct tm* timebegantm;

    time_t currenttime;
    char currenttimebuf[GAMESTATE_SIZEOFTIMEBUF];
    struct tm* currenttimetm;

    bool debugpanelon;
    debugpanel_t debugpanel;

    Camera2D cam2d;

    controlmode_t controlmode;

    int fadealpha;
    fadestate_t fadestate;

    display_t display;
    //time_t starttime;
    //struct tm* starttm;


} gamestate;


//gamestate* gamestateinitptr();
gamestate* gamestateinitptr(const int windowwidth,
                            const int windowheight,
                            const int targetwidth,
                            const int targetheight);
void gamestatefree(gamestate* g);
//void gamestateinitdebugpanel(gamestate* g);
//void gamestateinitcompanyscene(gamestate* g);
void gamestateupdatecurrenttime(gamestate* g);
