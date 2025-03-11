#pragma once

#include "direction.h"
#include "dungeonfloorid.h"
#include "entityid.h"
#include "event_type.h"
#include <stdbool.h>


#define MAX_EVENT_ACTORS 16 // Maximum number of additional initiators or recipients

typedef struct event_t {
    event_type_t type; // Event type (e.g., EVENT_MOVE, EVENT_ATTACK)
    entityid initiator; // Primary actor initiating the event (e.g., hero)
    entityid recipient; // Primary target, if any (e.g., enemy), -1 if none
    int xdir; // X-direction (e.g., -1 for left, 1 for right)
    int ydir; // Y-direction (e.g., -1 for up, 1 for down)
    int range; // Range of the event (e.g., 1 for melee, 2 for spear)
    entityid recipients[MAX_EVENT_ACTORS]; // Array for multiple recipients
    int num_recipients; // Number of recipients in the array
} event_t;

void event_init(event_t* event, event_type_t type, entityid initiator, int xdir, int ydir);

bool event_add_recipient(event_t* event, entityid id);

void event_set_range(event_t* event, int range);
