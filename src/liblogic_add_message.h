#pragma once

#include "gamestate.h"

void add_message(shared_ptr<gamestate> g, const char* fmt, ...);
void add_message_history(shared_ptr<gamestate> g, const char* fmt, ...);
