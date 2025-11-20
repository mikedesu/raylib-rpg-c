#include "libdraw_message_box.h"
#include "libgame_defines.h"

void draw_message_box(shared_ptr<gamestate> g) {
    if (!g->msg_system_is_active || g->msg_system->size() == 0) {
        return;
    }
    const string msg = g->msg_system->at(0);
    const Color message_bg = (Color){0x33, 0x33, 0x33, 0xff};
    const int font_size = 10;
    char tmp[1024] = {0};
    snprintf(tmp, sizeof(tmp), "%s", msg.c_str());
    const int measure = MeasureText(tmp, font_size);
    const float text_width = measure;
    const float text_height = font_size;
    const int w = DEFAULT_TARGET_WIDTH;
    const float x = (w - text_width) / 2.0 - g->pad;
    const float y = (DEFAULT_TARGET_HEIGHT - text_height) / 2.0 - g->pad;
    const Rectangle box = {x, y, text_width + g->pad * 2, text_height + g->pad * 2};

    DrawRectangleRec(box, message_bg);
    DrawRectangleLinesEx(box, 1, WHITE);
    DrawText(tmp, box.x + g->pad, box.y + g->pad, font_size, WHITE);
}
