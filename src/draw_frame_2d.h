#pragma once

#include "draw_help_menu.h"
#include "draw_hud.h"
#include "draw_inventory_menu.h"
#include "draw_message_box.h"
#include "draw_message_history.h"
#include "gamestate.h"
#include "libdraw_camera_lock_on.h"
#include "libdraw_dungeon_floor.h"
#include "libdraw_handle_debug_panel.h"

static inline void libdraw_drawframe_2d(shared_ptr<gamestate> g) {
    //minfo("BEGIN drawframe 2d");
    //BeginShaderMode(shader_color_noise);
    //float time = (float)GetTime(); // Current time in seconds
    //SetShaderValue(shader_color_noise, GetShaderLocation(shader_color_noise, "time"), &time, SHADER_UNIFORM_FLOAT);
    //minfo("camera lock-on");
    camera_lock_on(g);
    ////minfo("Drawing frame");
    BeginMode2D(g->cam2d);
    ClearBackground(BLACK);
    //EndShaderMode();
    //minfo("draw dungeon floor");
    libdraw_draw_dungeon_floor(g);

    //libdraw_draw_player_target_box(g);

    EndMode2D();
    //if (g->frame_dirty) {
    //draw_hud_to_texture(g);
    //} else {
    //draw_hud_from_texture(g);
    //}
    //minfo("draw hud");
    draw_hud(g);
    //minfo("draw msg history");
    draw_message_history(g);
    //minfo("draw msg box");
    draw_message_box(g);
    if (g->display_inventory_menu) {
        //minfo("draw inventory menu");
        draw_inventory_menu(g);
    }
    //else if (g->display_gameplay_settings_menu) {
    //    draw_gameplay_settings_menu(g);
    //}
    //minfo("handle debug panel");
    handle_debug_panel(g);
    //draw_version(g);
    //int x = 0;
    if (g->display_help_menu) {
        //minfo("draw help menu");
        draw_help_menu(g);
    }
    //if (g->gameover) {
    //    draw_gameover_menu(g);
    //}
    //minfo("END drawframe 2d");
}
