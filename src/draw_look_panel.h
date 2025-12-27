#pragma once

#include "gamestate.h"

static inline void draw_look_panel(gamestate& g) {
    // To make this useful,
    // we need to construct strings
    // telling us info about the tile

    auto loc = g.ct.get<location>(g.hero_id).value_or((vec3){-1, -1, -1});
    auto df = g.d.get_floor(loc.z);
    auto tile = df_tile_at(df, loc);

    // subtract 1 for the PLAYER
    const int entity_count = tile_entity_count(tile) - 1;

    const char* texts[] = {"Look Panel", "There is some tile here", TextFormat("There are %d things here", entity_count)};
    const int texts_size = 3;

    const char* dummy_text = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";


    float some_offset_y = 100;
    //float some_offset_x = 10;
    const int font_size = 10;
    const Color bgcolor = {0, 0, 255, 128};
    const int pad_w = 10;
    const int pad_h = 10;

    int m = MeasureText(dummy_text, font_size);

    float w = m + pad_w * 2;
    float h = font_size * texts_size + pad_h * 2;

    const float base_x = 10;
    const float base_y = g.targetheight - some_offset_y;

    Rectangle r = {base_x, base_y, w, h};

    DrawRectanglePro(r, (Vector2){0, 0}, 0.0f, bgcolor);
    DrawRectangleLinesEx(r, 1, WHITE);

    int off_y = 0;
    for (int i = 0; i < texts_size; i++) {
        DrawText(texts[i], r.x + pad_w, r.y + pad_h + off_y, font_size, WHITE);
        off_y += 10;
    }
}
