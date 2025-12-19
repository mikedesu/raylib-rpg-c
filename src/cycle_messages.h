#pragma once

#include "gamestate.h"

static inline void cycle_messages(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");

    if (g->msg_system.size() > 0) {
        string msg = g->msg_system.front();
        int len = msg.length();
        // measure the length of the message as calculated by MeasureText
        if (len > g->msg_history_max_len_msg) {
            g->msg_history_max_len_msg = len;
            int font_size = 10;
            int measure = MeasureText(msg.c_str(), font_size);
            g->msg_history_max_len_msg_measure = measure;
        }
        g->msg_history.push_back(g->msg_system.front());
        g->msg_system.erase(g->msg_system.begin());
    }

    if (g->msg_system.size() == 0)
        g->msg_system_is_active = false;
}
