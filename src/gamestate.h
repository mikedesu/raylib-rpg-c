#pragma once

#include "controlmode.h"
#include "debugpanel.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "em.h"
#include "entityid.h"
#include "fadestate.h"
#include "gamestate_flag.h"
#include "keybinding.h"
#include <raylib.h>
#include <stdbool.h>
#include <time.h>

#define GAMESTATE_SIZEOFTIMEBUF 64
#define GAMESTATE_SIZEOFDEBUGPANELBUF 1024
#define GAMESTATE_SIZEOFTEXINFOARRAY 255
#define MAX_MESSAGES 32
#define MAX_MSG_LENGTH 256

#define DEFAULT_MAX_HISTORY_SIZE 1024

typedef struct {
    char messages[MAX_MESSAGES][MAX_MSG_LENGTH];
    int count; // Total messages in queue
    int index; // Current message being shown
    bool is_active; // Blocks input when true
} message_system;

typedef struct {
    char** messages;
    int count;
    int max_count;
} message_history;

typedef struct gamestate {
    int framecount;
    int fadealpha;
    int camera_mode;
    int targetwidth;
    int targetheight;
    int windowwidth;
    int windowheight;
    int lock;
    int turn_count;
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
    bool gameover;
    bool player_changing_direction;

    debugpanel_t debugpanel;

    Camera2D cam2d;
    Camera3D cam3d;

    fadestate_t fadestate;

    Font font;

    dungeon_t* dungeon;

    em_t* entitymap;

    int index_entityids;
    int max_entityids;
    entityid* entityids;

    double last_frame_time;
    char frame_time_str[32];

    // flag can be:
    // PLAYER_TURN,
    // UPDATING_ANIM,
    // NPC_TURN,
    gamestate_flag_t flag;

    entityid entity_turn;

    message_system msg_system;

    message_history msg_history;

    keybinding_list_t keybinding_list;
} gamestate;

gamestate* gamestateinitptr();

entityid gamestate_get_entityid_unsafe(const gamestate* const g, const int index);
entityid gamestate_get_hero_id(const gamestate* const g);
int gamestate_get_entityid_index(const gamestate* const g, const entityid id);
int gamestate_get_next_npc_entityid_from_index(const gamestate* const g, const int index);

em_t* gamestate_get_entitymap(gamestate* const g);

bool gamestate_add_entityid(gamestate* const g, entityid id);
bool gamestate_init_msg_history(gamestate* const g);
bool gamestate_free_msg_history(gamestate* const g);

bool gamestate_add_msg_history(gamestate* const g, const char* msg);

void gamestatefree(gamestate* g);
void gamestate_dungeon_destroy(gamestate* const g);
void gamestate_init_entityids(gamestate* const g);
void gamestate_set_hero_id(gamestate* const g, const entityid id);
void gamestate_incr_entity_turn(gamestate* const g);
void gamestate_load_keybindings(gamestate* const g);
