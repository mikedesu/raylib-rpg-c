#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

static inline void draw_message_box(gamestate& g) {
    if (!g.display_confirm_prompt && (!g.msg_system_is_active || g.msg_system.size() == 0)) {
        return;
    }

    constexpr int w = DEFAULT_TARGET_WIDTH;
    constexpr int font_size = DEFAULT_MSG_WINDOW_FONT_SIZE;
    constexpr float text_height = font_size;
    constexpr float y = (DEFAULT_TARGET_HEIGHT - text_height) / 2.0 - DEFAULT_PAD;

    const string msg = g.display_confirm_prompt ? g.confirm_prompt_message : g.msg_system.at(0);


    char tmp[1024] = {0};
    snprintf(tmp, sizeof(tmp), "%s", msg.c_str());

    const int measure = MeasureText(tmp, font_size);
    const float text_width = measure;
    const float x = (w - text_width) / 2.0 - DEFAULT_PAD;

    //const float y = (DEFAULT_TARGET_HEIGHT - text_height) / 2.0 - g.pad;
    //const float box_width = text_width + g.pad * 2;
    const float box_width = text_width + DEFAULT_PAD * 2;
    const float box_height = text_height + g.pad * 2;

    const Rectangle box = {x, y, box_width, box_height};

    DrawRectangleRec(box, g.window_box_bgcolor);

    DrawRectangleLinesEx(box, 1, g.window_box_fgcolor);

    const int text_x = box.x + g.pad;
    const int text_y = box.y + g.pad;
    DrawText(tmp, text_x, text_y, font_size, g.window_box_fgcolor);
}
