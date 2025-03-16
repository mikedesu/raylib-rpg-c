#pragma once

#include "controlmode.h"
#include "debugpanel.h"
#include "dungeon.h"
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
    int camera_mode;
    int targetwidth;
    int targetheight;
    int windowwidth;
    int windowheight;
    int lock;
    controlmode_t controlmode;
    entityid hero_id;
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
    bool is3d;

    debugpanel_t debugpanel;

    Camera2D cam2d;
    Camera3D cam3d;

    fadestate_t fadestate;

    Font font;

    //dungeon_floor_t* dungeon_floor;

    dungeon_t* dungeon;

    em_t* entitymap;

    int index_entityids;
    int max_entityids;
    entityid* entityids;

    double last_frame_time;
    char frame_time_str[32];

} gamestate;


gamestate* gamestateinitptr();
void gamestatefree(gamestate* g);
void gamestate_add_entityid(gamestate* const g, const entityid id);

void gamestate_set_hero_id(gamestate* const g, const entityid id);
const entityid gamestate_get_hero_id(const gamestate* const g);

em_t* gamestate_get_entitymap(gamestate* const g);
