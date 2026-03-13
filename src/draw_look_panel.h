#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

static inline void draw_look_panel(gamestate& g) {
    //To make this useful, we need to construct strings telling us info about the tile

    auto loc = g.ct.get<location>(g.hero_id).value_or((vec3){-1, -1, -1});
    auto df = g.d.get_floor(loc.z);
    tile_t& tile = df->tile_at(loc);

    // subtract 1 for the PLAYER
    const int entity_count = tile.entity_count() - 1;

    //const char* texts[] = {
    //    "Look Panel",
    //    TextFormat("There are %d things here", entity_count),
    //    TextFormat("npc (alive): %d", -1),
    //    TextFormat("npc (dead): %d", -1),
    //    TextFormat("item: %d", -1),
    //    TextFormat("box: %d", -1),
    //    TextFormat("door: %d", -1)};
    const int texts_size = 7;

    const char* dummy_text = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

    //float some_offset_x = 10;

    const int font_size = DEFAULT_LOOK_PANEL_FONT_SIZE;

    const int pad_w = 10;
    const int pad_h = 10;

    const int m = MeasureText(dummy_text, font_size);

    const float w = m + pad_w * 2;
    //float h = font_size * texts_size + pad_h * 2;
    const float h = font_size * texts_size + pad_h * (texts_size + 2);

    //const float some_offset_y = h;

    const float base_x = 10;
    const float base_y = g.targetheight - h;

    Rectangle r = {base_x, base_y, w, h};

    DrawRectanglePro(r, Vector2{0, 0}, 0.0f, g.window_box_bgcolor);
    DrawRectangleLinesEx(r, 1, g.window_box_fgcolor);

    //int off_y = 0;
    //for (int i = 0; i < texts_size; i++) {
    //    DrawText(texts[i], r.x + pad_w, r.y + pad_h + off_y, font_size, WHITE);
    //    off_y += font_size + 5;
    //}

    DrawText("Look Panel", r.x + pad_w, r.y + pad_h + (font_size + 5) * 0, font_size, g.window_box_fgcolor);
    DrawText(TextFormat("There are %d things here", entity_count), r.x + pad_w, r.y + pad_h + (font_size + 5) * 1, font_size, g.window_box_fgcolor);
    DrawText(TextFormat("living npc %d", tile.get_cached_live_npc()), r.x + pad_w, r.y + pad_h + (font_size + 5) * 2, font_size, g.window_box_fgcolor);
    DrawText(TextFormat("dead npc %d", tile.get_cached_dead_npc()), r.x + pad_w, r.y + pad_h + (font_size + 5) * 3, font_size, g.window_box_fgcolor);
    DrawText(TextFormat("items %d (%d)", (int)tile.get_item_count(), tile.get_cached_item()), r.x + pad_w, r.y + pad_h + (font_size + 5) * 4, font_size, g.window_box_fgcolor);
    DrawText(TextFormat("door %d", tile.get_cached_door()), r.x + pad_w, r.y + pad_h + (font_size + 5) * 5, font_size, g.window_box_fgcolor);
    DrawText(TextFormat("box %d", tile.get_cached_box()), r.x + pad_w, r.y + pad_h + (font_size + 5) * 6, font_size, g.window_box_fgcolor);
}
