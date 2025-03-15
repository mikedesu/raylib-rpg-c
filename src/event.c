#include "event.h"
#include "mprint.h"
#include <stdbool.h>
#include <stdlib.h>


const bool event_type_is_valid(const event_type_t type) {
    if (type < EVENT_NONE || type >= EVENT_COUNT) {
        merror("event_type_is_valid: type is out of bounds");
        return false;
    }
    return true;
}


void event_init(event_t* event, event_type_t type, entityid actor, entityid target, int xdir, int ydir) {
    if (!event) {
        merror("event_init: event is NULL");
        return;
    }

    if (!event_type_is_valid(type)) {
        merror("event_init: type is invalid");
        return;
    }

    if (actor < 0) {
        merror("event_init: actor_id is out of bounds");
        return;
    }

    if (target < 0) {
        merror("event_init: target_id is out of bounds");
        return;
    }

    event->type = type;
    event->actor = actor;
    event->target = target;
    event->xdir = xdir;
    event->ydir = ydir;
}

void event_zero(event_t* event) {
    if (!event) {
        merror("event_zero: event is NULL");
        return;
    }

    event->type = EVENT_NONE;
    event->actor = -1;
    event->target = -1;
    event->xdir = 0;
    event->ydir = 0;
}
