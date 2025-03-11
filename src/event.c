#include "event.h"
#include <stdbool.h>
#include <stdlib.h>

void event_init(event_t* event, event_type_t type, entityid initiator, int xdir, int ydir) {
    event->type = type;
    event->initiator = initiator;
    event->recipient = -1; // Default to no recipient
    event->xdir = xdir;
    event->ydir = ydir;
    event->range = (xdir != 0 || ydir != 0) ? abs(xdir) + abs(ydir) : 0; // Default range
    event->num_recipients = 0; // No additional recipients yet
}

bool event_add_recipient(event_t* event, entityid id) {
    if (event->num_recipients == 0 && event->recipient == -1) {
        event->recipient = id; // Use primary recipient if unused
        return true;
    }
    if (event->num_recipients >= MAX_EVENT_ACTORS) {
        return false; // Array full
    }
    event->recipients[event->num_recipients++] = id;
    return true;
}

void event_set_range(event_t* event, int range) {
    event->range = range;
}
