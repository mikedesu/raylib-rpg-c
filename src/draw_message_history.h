#pragma once


#include "gamestate.h"
#include "libgame_defines.h"


using std::min;


static inline void draw_message_history(gamestate& g) {
    char tmp[1024] = {0};
    const int font_size = DEFAULT_MSG_HISTORY_FONT_SIZE;
    constexpr int max_messages = 30;
    const Color message_bg = Fade((Color){0, 0, 0xff, 0xff}, 0.5f);
    const int msg_count = g.msg_history.size();
    if (msg_count == 0) {
        // if there are no messages, we don't need to draw anything
        return;
    }
    const int max_measure = g.msg_history_max_len_msg_measure;

    const float w = max_measure + g.pad;
    //const float w = max_measure;

    const float h = (font_size + 2) * min(msg_count, max_messages) + g.pad;
    const float x = g.targetwidth - w;
    constexpr float y = 0;
    const Rectangle box = {x, y, w, h};
    DrawRectangleRec(box, message_bg);
    DrawRectangleLinesEx(box, 1, WHITE);

    // now lets draw each message in the history
    if (msg_count > max_messages) {
        int outer_count = 0;
        for (int i = msg_count - max_messages; i < msg_count; i++) {
            const string msg = g.msg_history.at(i);
            bzero(tmp, 1024);
            snprintf(tmp, sizeof(tmp), "%s", msg.c_str());
            const float msg_x = box.x + g.pad / 2.0;
            const float msg_y = box.y + g.pad / 2.0 + (outer_count * (font_size + 2));
            DrawText(tmp, msg_x, msg_y, font_size, WHITE);
            outer_count++;
        }
        return;
    }
    for (int i = 0; i < msg_count; i++) {
        const string msg = g.msg_history.at(i);
        bzero(tmp, 1024);
        snprintf(tmp, sizeof(tmp), "%s", msg.c_str());
        const float msg_x = box.x + g.pad / 2.0;
        const float msg_y = box.y + g.pad / 2.0 + (i * (font_size + 2));
        DrawText(tmp, msg_x, msg_y, font_size, WHITE);
    }
}
