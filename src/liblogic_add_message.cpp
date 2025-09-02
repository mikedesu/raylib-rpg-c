#include "gamestate.h"

void add_message(shared_ptr<gamestate> g, const char* fmt, ...) {
    massert(g, "gamestate is NULL");
    massert(fmt, "format string is NULL");
    minfo("attempting to add message...");
    char buffer[MAX_MSG_LENGTH];
    va_list args;
    va_start(args, fmt);
    minfo("calling vsnprintf...");
    vsnprintf(buffer, MAX_MSG_LENGTH - 1, fmt, args);
    va_end(args);
    string s(buffer);
    g->msg_system->push_back(s);
    g->msg_system_is_active = true;
}
