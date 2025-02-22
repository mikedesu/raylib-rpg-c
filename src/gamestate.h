#pragma once

#include "controlmode.h"
#include "debugpanel.h"
//#include "display.h"
#include "entityid.h"
#include "fadestate.h"
#include "hashtable_entityid_spritegroup.h"
//#include "sizemacros.h"
#include "dungeon_floor.h"
#include "em.h"
#include "textureinfo.h"
#include <raylib.h>
#include <stdbool.h>
#include <time.h>

#define GAMESTATE_SIZEOFTIMEBUF 64
#define GAMESTATE_SIZEOFDEBUGPANELBUF 1024
#define GAMESTATE_SIZEOFTEXINFOARRAY 255

typedef struct gamestate {
    int framecount;
    int fadealpha;
    //int lock_timer;

    time_t timebegan;
    time_t currenttime;

    struct tm* timebegantm;
    struct tm* currenttimetm;

    char timebeganbuf[GAMESTATE_SIZEOFTIMEBUF];
    char currenttimebuf[GAMESTATE_SIZEOFTIMEBUF];

    bool debugpanelon;
    bool gridon;
    bool cam_lockon;
    //bool do_one_rotation;
    bool player_input_received;
    bool is_locked;
    bool processing_actions;

    debugpanel_t debugpanel;

    Camera2D cam2d;
    Camera3D cam3d;
    int camera_mode;

    bool is3d;

    controlmode_t controlmode;

    fadestate_t fadestate;

    //display_t display;
    int targetwidth;
    int targetheight;
    int windowwidth;
    int windowheight;
    int lock;

    Font font;

    hashtable_entityid_spritegroup_t* spritegroups;

    textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

    entityid hero_id;

    dungeon_floor_t* dungeon_floor;

    em_t* entitymap;

    int index_entityids;
    int max_entityids;
    entityid* entityids;


} gamestate;


gamestate* gamestateinitptr();
//gamestate* gamestateinitptr(const int windowwidth, const int windowheight, const int targetwidth, const int targetheight);
void gamestatefree(gamestate* g);
//void gamestate_update_current_time(gamestate* const g);
//
//
//

void gamestate_add_entityid(gamestate* const g, const entityid id);
