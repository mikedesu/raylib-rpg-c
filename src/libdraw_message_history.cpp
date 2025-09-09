#include "libdraw_message_history.h"

void draw_message_history(shared_ptr<gamestate> g) {
    int font_size, msg_count, max_measure, pad;
    Color message_bg;
    //float x, y, w, h, text_width, text_height, msg_x, msg_y;
    float x, y, w, h, msg_x, msg_y;
    Rectangle box;
    char tmp[1024] = {0};
    string msg;
    font_size = 10;
    // message history will now be a box directly beneath the hud
    // it should be static in size, that is, unchanging and const
    // first, lets calc that box position and size
    message_bg = {0x33, 0x33, 0x33, 0xff};
    msg_count = g->msg_history->size();
    if (msg_count == 0) {
        // if there are no messages, we don't need to draw anything
        return;
    }
    //max_measure = -1;
    //for (int i = 0; i < msg_count; i++) {
    //    msg = g->msg_history->at(i);
    //    measure = MeasureText(msg.c_str(), font_size);
    //    if (measure > max_measure) {
    //        max_measure = measure;
    //    }
    //}
    max_measure = g->msg_history_max_len_msg_measure;
    w = max_measure + g->pad;
    //h = font_size * 5 + g->pad * 2;
    pad = 10;
    h = font_size * msg_count + pad * 2;
    //x = DEFAULT_TARGET_WIDTH - w;
    x = 0;
    y = font_size + g->pad + 5;
    box = {x, y, w, h};
    DrawRectangleRec(box, message_bg);
    DrawRectangleLinesEx(box, 1, WHITE);
    // now lets draw each message in the history
    for (int i = 0; i < msg_count; i++) {
        msg = g->msg_history->at(i);
        bzero(tmp, 1024);
        snprintf(tmp, sizeof(tmp), "%s", msg.c_str());
        //measure = MeasureText(tmp, font_size);
        //text_width = MeasureText(tmp, font_size);
        //text_height = font_size;
        //text_width = measure;
        msg_x = box.x + pad;
        msg_y = box.y + pad + (i * (font_size + 2));
        if (msg_y + font_size > box.y + box.height) {
            break; // stop drawing if we exceed the box height
        }
        DrawText(tmp, msg_x, msg_y, font_size, WHITE);
    }
}
