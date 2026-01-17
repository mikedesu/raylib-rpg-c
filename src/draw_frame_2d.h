#pragma once


#include "camera_lock_on.h"
#include "draw_action_menu.h"
#include "draw_dungeon_floor.h"
#include "draw_handle_debug_panel.h"
#include "draw_help_menu.h"
#include "draw_hud.h"
#include "draw_inventory_menu.h"
#include "draw_look_panel.h"
#include "draw_message_box.h"
#include "draw_message_history.h"
#include "draw_option_menu.h"
#include "gamestate.h"


static inline void libdraw_drawframe_2d(gamestate& g) {
    //BeginShaderMode(shader_color_noise);
    //float time = (float)GetTime(); // Current time in seconds
    //SetShaderValue(shader_color_noise, GetShaderLocation(shader_color_noise, "time"), &time, SHADER_UNIFORM_FLOAT);
    //EndShaderMode();
    camera_lock_on(g);
    BeginMode2D(g.cam2d);
    ClearBackground(BLACK);
    draw_dungeon_floor(g);
    EndMode2D();
    draw_hud(g);
    draw_look_panel(g);
    draw_message_history(g);
    draw_message_box(g);
    if (g.display_inventory_menu)
        draw_inventory_menu(g);
    if (g.display_action_menu)
        draw_action_menu(g);
    if (g.display_option_menu)
        draw_option_menu(g);
        //else if (g.display_gameplay_settings_menu) {
        //    draw_gameplay_settings_menu(g);
        //}
        //minfo("handle debug panel");
#ifdef DEBUG
    handle_debug_panel(g);
#endif
    //draw_version(g);
    //int x = 0;
    if (g.display_help_menu) {
        //minfo("draw help menu");
        draw_help_menu(g);
    }
    //if (g.gameover) {
    //    draw_gameover_menu(g);
    //}
    //minfo("END drawframe 2d");
}
