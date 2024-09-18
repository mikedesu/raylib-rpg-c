#pragma once

#include "companyscene.h"
#include "debugpanel.h"
#include "display.h"
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
    debugpanel dp;

    Camera2D cam2d;

    companyscene cs;

    //display d;
    //time_t starttime;
    //struct tm* starttm;


} gamestate;


gamestate* gamestateinitptr();
void gamestatefree(gamestate* g);
//void gamestateinitdebugpanel(gamestate* g);
//void gamestateinitcompanyscene(gamestate* g);
void gamestateupdatecurrenttime(gamestate* g);
