#include "libdraw_message_history.h"

void draw_message_history(shared_ptr<gamestate> g) {
    char tmp[1024] = {0};
    const int font_size = 10;
    //const Color message_bg = {0x33, 0x33, 0x33, 0xff};
    const Color message_bg = {0, 0, 0xff, 0xff};
    const int msg_count = g->msg_history->size();
    if (msg_count == 0) {
        // if there are no messages, we don't need to draw anything
        return;
    }
    const int max_measure = g->msg_history_max_len_msg_measure;
    const float w = max_measure + g->pad;
    const float h = (font_size + 2) * msg_count + g->pad;
    //x = DEFAULT_TARGET_WIDTH - w;
    const float x = 0;
    const float y = 0;
    const Rectangle box = {x, y, w, h};
    DrawRectangleRec(box, message_bg);
    DrawRectangleLinesEx(box, 1, WHITE);
    // now lets draw each message in the history
    for (int i = 0; i < msg_count; i++) {
        const string msg = g->msg_history->at(i);
        bzero(tmp, 1024);
        snprintf(tmp, sizeof(tmp), "%s", msg.c_str());
        const float msg_x = box.x + g->pad / 2.0;
        const float msg_y = box.y + g->pad / 2.0 + (i * (font_size + 2));
        //if (msg_y + font_size > box.y + box.height) {
        //    break; // stop drawing if we exceed the box height
        //}
        DrawText(tmp, msg_x, msg_y, font_size, WHITE);
    }
}
