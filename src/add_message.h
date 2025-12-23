#pragma once

#include "gamestate.h"

static inline void add_message(gamestate& g, const char* fmt, ...) {
    massert(fmt, "format string is NULL");
    char buffer[MAX_MSG_LENGTH];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, MAX_MSG_LENGTH - 1, fmt, args);
    va_end(args);
    string s(buffer);
    g.msg_system.push_back(s);
    g.msg_system_is_active = true;
}

static inline void add_message_history(gamestate& g, const char* fmt, ...) {
    massert(fmt, "format string is NULL");
    char buffer[MAX_MSG_LENGTH];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, MAX_MSG_LENGTH - 1, fmt, args);
    va_end(args);
    string s(buffer);
    g.msg_history.push_back(s);
}
