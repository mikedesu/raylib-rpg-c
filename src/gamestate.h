#pragma once

#include "controlmode.h"
#include "debugpanel.h"
#include "display.h"
#include "dungeonfloor.h"
#include "entityid.h"
#include "fadestate.h"
#include "hashtable_entityid_entity.h"
#include "hashtable_entityid_spritegroup.h"
#include "sizemacros.h"
#include "textureinfo.h"
#include "vectorentityid.h"
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
    bool gridon;
    debugpanel_t debugpanel;

    bool cam_lockon;
    Camera2D cam2d;

    controlmode_t controlmode;

    int fadealpha;
    fadestate_t fadestate;

    display_t display;
    //time_t starttime;
    //struct tm* starttm;
    Font font;

    //hashtable_entityid_entity_t* entities;
    hashtable_entityid_spritegroup_t* spritegroups;

    //entityid hero_id;
    //entityid torch_id;

#define GAMESTATE_SIZEOFTEXINFOARRAY 255
    textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

    dungeonfloor_t* dungeonfloor;
    vectorentityid_t entityids;

    bool do_one_rotation;

    bool player_input_received;


    bool is_locked;
    int lock_timer;

    //bool is_updating_smooth_move;
    //int smooth_move_index;

} gamestate;


//gamestate* gamestateinitptr();
gamestate* gamestateinitptr(const int windowwidth, const int windowheight, const int targetwidth, const int targetheight);
void gamestatefree(gamestate* g);
//void gamestateinitdebugpanel(gamestate* g);
//void gamestateinitcompanyscene(gamestate* g);
void gamestate_update_current_time(gamestate* g);
