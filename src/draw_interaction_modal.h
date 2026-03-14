/** @file draw_interaction_modal.h
 *  @brief NPC/prop interaction modal drawing helper.
 */

#pragma once

#include "gamestate.h"
#include "libgame_defines.h"
#include <algorithm>

/** @brief Draw the active interaction modal title and body text. */
static inline void draw_interaction_modal(gamestate& g) {
    if (!g.display_interaction_modal || g.interaction_body.empty()) {
        return;
    }

    constexpr int title_font_size = 24;
    constexpr int body_font_size = DEFAULT_MSG_WINDOW_FONT_SIZE;
    constexpr int line_spacing = 6;
    constexpr int padding = DEFAULT_PAD;
    constexpr int min_width = 360;
    vector<string> lines;
    if (!g.interaction_title.empty()) {
        lines.push_back(g.interaction_title);
    }

    size_t line_start = 0;
    while (line_start <= g.interaction_body.size()) {
        const size_t line_end = g.interaction_body.find('\n', line_start);
        const string line = line_end == string::npos ? g.interaction_body.substr(line_start) : g.interaction_body.substr(line_start, line_end - line_start);
        lines.push_back(line);
        if (line_end == string::npos) {
            break;
        }
        line_start = line_end + 1;
    }

    int max_text_width = 0;
    for (size_t i = 0; i < lines.size(); i++) {
        const int font_size = i == 0 && !g.interaction_title.empty() ? title_font_size : body_font_size;
        max_text_width = std::max(max_text_width, MeasureText(lines[i].c_str(), font_size));
    }

    const int box_width = std::max(min_width, max_text_width + padding * 2);
    const int title_height = g.interaction_title.empty() ? 0 : title_font_size;
    const int body_lines = static_cast<int>(lines.size()) - static_cast<int>(!g.interaction_title.empty());
    const int body_height = body_lines > 0 ? body_lines * body_font_size + (body_lines - 1) * line_spacing : 0;
    const int section_gap = (!g.interaction_title.empty() && body_lines > 0) ? line_spacing + 4 : 0;
    const int box_height = padding * 2 + title_height + body_height + section_gap;
    const int box_x = (DEFAULT_TARGET_WIDTH - box_width) / 2;
    const int box_y = (DEFAULT_TARGET_HEIGHT - box_height) / 2;

    DrawRectangle(box_x, box_y, box_width, box_height, g.window_box_bgcolor);
    DrawRectangleLinesEx(Rectangle{static_cast<float>(box_x), static_cast<float>(box_y), static_cast<float>(box_width), static_cast<float>(box_height)}, 2, g.window_box_fgcolor);

    int text_y = box_y + padding;
    if (!g.interaction_title.empty()) {
        const int title_width = MeasureText(g.interaction_title.c_str(), title_font_size);
        const int title_x = box_x + (box_width - title_width) / 2;
        DrawText(g.interaction_title.c_str(), title_x, text_y, title_font_size, g.window_box_fgcolor);
        text_y += title_font_size + section_gap;
    }

    const int body_x = box_x + padding;
    for (size_t i = g.interaction_title.empty() ? 0 : 1; i < lines.size(); i++) {
        DrawText(lines[i].c_str(), body_x, text_y, body_font_size, g.window_box_fgcolor);
        text_y += body_font_size + line_spacing;
    }
}
