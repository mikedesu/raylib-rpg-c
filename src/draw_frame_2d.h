#pragma once


#include "camera_lock_on.h"
#include "draw_action_menu.h"
#include "draw_chest_menu.h"
#include "draw_controls_menu.h"
#include "draw_damage_numbers.h"
#include "draw_dungeon_floor.h"
#include "draw_handle_debug_panel.h"
#include "draw_help_menu.h"
#include "draw_hud.h"
#include "draw_interaction_modal.h"
#include "draw_inventory_menu.h"
#include "draw_level_up_modal.h"
#include "draw_mini_inventory_menu.h"
#include "draw_look_panel.h"
#include "draw_message_box.h"
#include "draw_message_history.h"
#include "draw_option_menu.h"
#include "draw_keyboard_profile_prompt.h"
#include "draw_sound_menu.h"
#include "draw_window_color_menu.h"
#include "gamestate.h"


static inline void libdraw_drawframe_2d(gamestate& g, int vision_dist, int light_rad) {
    //BeginShaderMode(shader_color_noise);
    //float time = (float)GetTime(); // Current time in seconds
    //SetShaderValue(shader_color_noise, GetShaderLocation(shader_color_noise, "time"), &time, SHADER_UNIFORM_FLOAT);
    //EndShaderMode();

    camera_lock_on(g);

    BeginMode2D(g.cam2d);
    ClearBackground(BLACK);

    //const int vision_dist = g.ct.get<vision_distance>(g.hero_id).value_or(0);
    //const int light_rad = g.ct.get<light_radius>(g.hero_id).value_or(0);

    draw_dungeon_floor(g, vision_dist, light_rad);
    draw_damage_numbers(g);

    EndMode2D();
    draw_hud(g);
    draw_look_panel(g);
    draw_message_history(g);
    draw_message_box(g);
    draw_interaction_modal(g);
    draw_level_up_modal(g);

    if (g.display_inventory_menu) {
        if (g.use_mini_inventory_menu()) {
            auto maybe_inventory = g.ct.get<inventory>(g.hero_id);
            auto items = maybe_inventory.value_or(nullptr);
            if (items) {
                draw_mini_inventory_menu(g, items, "Inventory", "E equip  Enter use  Q drop  Esc close", true);
            }
        }
        else {
            draw_inventory_menu(g);
        }
    }

    if (g.display_chest_menu) {
        if (g.use_mini_inventory_menu()) {
            const entityid source_id = g.chest_deposit_mode ? g.hero_id : g.active_chest_id;
            auto maybe_inventory = g.ct.get<inventory>(source_id);
            auto items = maybe_inventory.value_or(nullptr);
            if (items) {
                draw_mini_inventory_menu(
                    g,
                    items,
                    g.chest_deposit_mode ? "Chest Deposit" : "Chest",
                    g.chest_deposit_mode ? "Tab chest  Enter deposit  D close" : "Tab hero  Enter take  D close",
                    g.chest_deposit_mode);
            }
        }
        else {
            draw_chest_menu(g);
        }
    }

    if (g.display_action_menu) {
        draw_action_menu(g);
    }

    if (g.display_option_menu) {
        draw_option_menu(g);
    }

    if (g.display_sound_menu) {
        draw_sound_menu(g);
    }

    if (g.display_window_color_menu) {
        draw_window_color_menu(g);
    }

    if (g.display_controls_menu) {
        draw_controls_menu(g);
    }

    if (g.display_help_menu) {
        draw_help_menu(g);
    }

    draw_keyboard_profile_prompt(g);
    //else if (g.display_gameplay_settings_menu) {
    //    draw_gameplay_settings_menu(g);
    //}
    //minfo("handle debug panel");
#ifdef DEBUG
    handle_debug_panel(g);
#endif
    //draw_version(g);
    //int x = 0;


    //if (g.gameover) {
    //    draw_gameover_menu(g);
    //}
    //minfo("END drawframe 2d");
}
