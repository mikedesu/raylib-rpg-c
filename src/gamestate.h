#pragma once

#include "controlmode.h"
#include "debugpanel.h"
#include "dungeon_floor.h"
#include "em.h"
#include "entityid.h"
#include "fadestate.h"
//#include "hashtable_entityid_spritegroup.h"
//#include "textureinfo.h"
#include <raylib.h>
#include <stdbool.h>
#include <time.h>

#define GAMESTATE_SIZEOFTIMEBUF 64
#define GAMESTATE_SIZEOFDEBUGPANELBUF 1024
#define GAMESTATE_SIZEOFTEXINFOARRAY 255

typedef struct gamestate {
    int framecount;
    int fadealpha;

    time_t timebegan;
    time_t currenttime;

    struct tm* timebegantm;
    struct tm* currenttimetm;

    char timebeganbuf[GAMESTATE_SIZEOFTIMEBUF];
    char currenttimebuf[GAMESTATE_SIZEOFTIMEBUF];

    bool debugpanelon;
    bool gridon;
    bool cam_lockon;
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

    int targetwidth;
    int targetheight;
    int windowwidth;
    int windowheight;
    int lock;

    Font font;

    //hashtable_entityid_spritegroup_t* spritegroups;

    //textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

    entityid hero_id;

    dungeon_floor_t* dungeon_floor;

    em_t* entitymap;

    int index_entityids;
    int max_entityids;
    entityid* entityids;


} gamestate;


gamestate* gamestateinitptr();
void gamestatefree(gamestate* g);
void gamestate_add_entityid(gamestate* const g, const entityid id);
