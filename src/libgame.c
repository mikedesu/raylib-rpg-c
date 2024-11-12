#include "actions.h"
#include "controlmode.h"
#include "direction.h"
#include "entitytype.h"
#include "fadestate.h"
#include "gamestate.h"
#include "get_txkey_for_tiletype.h"
#include "hashtable_entityid_spritegroup.h"
#include "libgame.h"
#include "libgame_defines.h"
#include "mprint.h"
#include "race.h"
#include "scene.h"
#include "setdebugpanel.h"
#include "specifier.h"
#include "sprite.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"
#include "textureinfo.h"
#include "tiletype.h"
#include "tx_keys.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// Lua support
#include "libgame_lua.h"


//------------------------------------------------------------------
// libgame global variables
//------------------------------------------------------------------
gamestate* g = NULL;
lua_State* L = NULL;
RenderTexture target;
Rectangle target_src = {0, 0, 0, 0}, target_dest = {0, 0, 0, 0};
Vector2 target_origin = {0, 0};
int activescene = GAMEPLAYSCENE;




void libgame_draw_fade(const gamestate* const g) {
    if (g && g->fadealpha > 0) DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, g->fadealpha});
}




void libgame_handle_fade(gamestate* const g) {
    if (!g) return;
    const int fadespeed = 4;
    // modify the fadealpha
    g->fadealpha += g->fadestate == FADESTATEOUT && g->fadealpha < 255 ? fadespeed : g->fadestate == FADESTATEIN && g->fadealpha > 0 ? -fadespeed : 0;
    // handle scene rotation based on fadealpha
    if (g->fadealpha >= 255) {
        g->fadealpha = 255, g->fadestate = FADESTATEIN;
        activescene = activescene + 1 > 2 ? 0 : activescene + 1;
        //activescene++;
        //if (activescene > 2)
        //    activescene = 0;
    }
    // halt fade if fadealpha is 0
    if (g->fadealpha <= 0) { g->fadealpha = 0, g->fadestate = FADESTATENONE; }
    libgame_draw_fade(g);
}




//void libgame_entity_anim_incr(gamestate* const g, const entityid id) {
//    if (g) spritegroup_incr(hashtable_entityid_spritegroup_get(g->spritegroups, id));
//}




const bool libgame_entity_anim_set(gamestate* const g, const entityid id, const int index) {
    if (!g) return false;
    //if (g) {
    return spritegroup_set_current(hashtable_entityid_spritegroup_get(g->spritegroups, id), index);
    //}
}




void libgame_test_enemy_placement(gamestate* const g) {
    //minfo("test_enemy_placement begin");
    if (!g) return;
    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
    if (hero_id == -1) return;
    const int x = libgame_lua_get_entity_int(L, hero_id, "x") + 1, y = libgame_lua_get_entity_int(L, hero_id, "y");
    if (libgame_lua_tile_is_occupied_by_npc(L, x, y))
        merror("test_enemy_placement: tile is occupied by npc");
    else if (libgame_create_orc_lua(g, "orc", x, y) == -1)
        merror("test_enemy_placement: failed to create orc");
}




const int libgame_get_x_from_dir(const direction_t dir) {
    return dir == DIRECTION_RIGHT || dir == DIRECTION_DOWN_RIGHT || dir == DIRECTION_UP_RIGHT ? 1 : dir == DIRECTION_LEFT || dir == DIRECTION_DOWN_LEFT || dir == DIRECTION_UP_LEFT ? -1 : 0;
}




const int libgame_get_y_from_dir(const direction_t dir) {
    return dir == DIRECTION_DOWN || dir == DIRECTION_DOWN_LEFT || dir == DIRECTION_DOWN_RIGHT ? 1 : dir == DIRECTION_UP || dir == DIRECTION_UP_LEFT || dir == DIRECTION_UP_RIGHT ? -1 : 0;
}




const direction_t libgame_get_dir_from_xy(const int xdir, const int ydir) {
    return xdir == 1 && ydir == 0     ? DIRECTION_RIGHT
           : xdir == -1 && ydir == 0  ? DIRECTION_LEFT
           : xdir == 0 && ydir == 1   ? DIRECTION_DOWN
           : xdir == 0 && ydir == -1  ? DIRECTION_UP
           : xdir == 1 && ydir == 1   ? DIRECTION_DOWN_RIGHT
           : xdir == -1 && ydir == 1  ? DIRECTION_DOWN_LEFT
           : xdir == 1 && ydir == -1  ? DIRECTION_UP_RIGHT
           : xdir == -1 && ydir == -1 ? DIRECTION_UP_LEFT
                                      : DIRECTION_NONE;
}




void libgame_process_turn_alt(gamestate* const g) {
    const int current_action = libgame_lua_get_gamestate_int(L, "CurrentAction");
    const int action_count = libgame_lua_get_action_count(L);
    if (action_count <= 0) {
        merror("action count is 0");
        return;
    }
    //else if (current_action == -1) {
    //    merror("current action is -1");
    //    libgame_incr_current_action(g);
    //    return;
    //}


    //if (current_action >= action_count) {
    //    merror("current action is greater than action count");
    //    libgame_lua_clear_actions(L);
    //    libgame_incr_current_action(g);
    //    return;
    //}

    libgame_process_turn_action(g, current_action);
    libgame_incr_current_action(g);
}



void libgame_handleinput(gamestate* const g) {
    if (!g) return;
    //if (IsKeyPressed(KEY_SPACE) || GetTouchPointCount() > 0) {
    //    minfo("key space pressed");
    //    if (g->fadestate == FADESTATENONE) {
    //        g->fadestate = FADESTATEOUT;
    //    }
    //}
    if (IsKeyPressed(KEY_E)) libgame_test_enemy_placement(g);


    //if (IsKeyPressed(KEY_O)) { libgame_incr_current_action_key(g); }
    if (IsKeyPressed(KEY_P)) {

        libgame_process_turn_alt(g);

        //const int current_action = libgame_lua_get_gamestate_int(L, "CurrentAction");
        //const int action_count = libgame_lua_get_action_count(L);
        //if (action_count <= 0) {
        //    merror("action count is 0");
        //    return;
        //} else if (current_action == -1) {
        //    merror("current action is -1");
        //    libgame_incr_current_action(g);
        //    return;
        //} else if (current_action > action_count) {
        //    merror("current action is greater than action count");
        //    libgame_lua_clear_actions(L);
        //    libgame_incr_current_action(g);
        //    return;
        //}
        //libgame_process_turn_action(g, current_action);
        //libgame_incr_current_action(g);
    }


    libgame_handle_modeswitch(g);
    libgame_handle_debugpanel_switch(g);
    libgame_handle_grid_switch(g);
    libgame_handle_input_player(g);
    libgame_handle_caminput(g);
}




void libgame_handle_modeswitch(gamestate* const g) {
    if (g && IsKeyPressed(KEY_C)) g->controlmode = g->controlmode == CONTROLMODE_CAMERA ? CONTROLMODE_PLAYER : CONTROLMODE_CAMERA;
}




void libgame_handle_debugpanel_switch(gamestate* const g) {
    if (g && IsKeyPressed(KEY_D)) g->debugpanelon = !g->debugpanelon;
}




void libgame_handle_grid_switch(gamestate* const g) {
    if (g && IsKeyPressed(KEY_G)) g->gridon = !g->gridon;
}




void libgame_update_spritegroup(gamestate* const g, const entityid id, const specifier_t spec, const direction_t dir) {
    if (!g) return;
    spritegroup_t* group = hashtable_entityid_spritegroup_get_by_specifier(g->spritegroups, id, spec);
    if (!group) return;
    const int old_ctx = group->sprites[group->current]->currentcontext;
    int ctx = old_ctx;
    ctx = dir == DIRECTION_NONE                                      ? old_ctx
          : dir == DIRECTION_DOWN_RIGHT                              ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIRECTION_DOWN_LEFT                               ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIRECTION_UP_RIGHT                                ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIRECTION_UP_LEFT                                 ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIRECTION_DOWN && ctx == SPRITEGROUP_CONTEXT_R_D  ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIRECTION_DOWN && ctx == SPRITEGROUP_CONTEXT_L_D  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIRECTION_DOWN && ctx == SPRITEGROUP_CONTEXT_R_U  ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIRECTION_DOWN && ctx == SPRITEGROUP_CONTEXT_L_U  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIRECTION_UP && ctx == SPRITEGROUP_CONTEXT_R_D    ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIRECTION_UP && ctx == SPRITEGROUP_CONTEXT_L_D    ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIRECTION_UP && ctx == SPRITEGROUP_CONTEXT_R_U    ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIRECTION_UP && ctx == SPRITEGROUP_CONTEXT_L_U    ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIRECTION_RIGHT && ctx == SPRITEGROUP_CONTEXT_R_D ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIRECTION_RIGHT && ctx == SPRITEGROUP_CONTEXT_L_D ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIRECTION_RIGHT && ctx == SPRITEGROUP_CONTEXT_R_U ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIRECTION_RIGHT && ctx == SPRITEGROUP_CONTEXT_L_U ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIRECTION_LEFT && ctx == SPRITEGROUP_CONTEXT_R_D  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIRECTION_LEFT && ctx == SPRITEGROUP_CONTEXT_L_D  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIRECTION_LEFT && ctx == SPRITEGROUP_CONTEXT_R_U  ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIRECTION_LEFT && ctx == SPRITEGROUP_CONTEXT_L_U  ? SPRITEGROUP_CONTEXT_L_U
                                                                     : old_ctx;
    spritegroup_setcontexts(group, ctx);
}




void libgame_update_spritegroup_move(gamestate* const g, const entityid id, const int x, const int y) {
    if (!g) return;
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (!sg) return;
    sg->move_x += x;
    sg->move_y += y;
}




void libgame_handle_player_input_movement_key(gamestate* const g, const direction_t dir) {
    if (!g) return;
    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
    if (hero_id == -1) return;
    // update player direction
    libgame_lua_set_entity_int(L, hero_id, "direction", dir);
    libgame_lua_create_action(L, hero_id, ACTION_MOVE, libgame_get_x_from_dir(dir), libgame_get_y_from_dir(dir));
    libgame_update_spritegroup(g, hero_id, SPECIFIER_NONE, dir); // updates sg context
    // hack to make the buckler correctly update...this probably doesnt belong here lol
    const entityid shieldid = libgame_lua_get_entity_shield(L, hero_id);
    libgame_update_spritegroup(g, shieldid, SPECIFIER_SHIELD_BLOCK, dir);

    //libgame_entity_anim_set(g, hero_id, SPRITEGROUP_ANIM_HUMAN_WALK);

    g->player_input_received = true;
}




void libgame_handle_player_input_attack_key(gamestate* const g) {
    if (!g) return;
    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
    if (hero_id == -1) return;
    const direction_t dir = libgame_lua_get_entity_int(L, hero_id, "direction");
    if (!libgame_lua_create_action(L, hero_id, ACTION_ATTACK, libgame_get_x_from_dir(dir), libgame_get_y_from_dir(dir))) merror("attack action failed to create");
    //else
    //libgame_entity_anim_set(g, hero_id, SPRITEGROUP_ANIM_HUMAN_ATTACK);
    g->player_input_received = true;
}




void libgame_handle_player_input_block_key(gamestate* const g) {
    if (!g) return;
    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
    if (hero_id == -1) return;
    // trigger a "block" action
    const direction_t dir = libgame_lua_get_entity_int(L, hero_id, "direction");
    if (libgame_lua_create_action(L, hero_id, ACTION_BLOCK, libgame_get_x_from_dir(dir), libgame_get_y_from_dir(dir)))
        libgame_entity_anim_set(g, hero_id, SPRITEGROUP_ANIM_HUMAN_GUARD);
    else
        merror("block action failed to create");
    g->player_input_received = true;
}




void libgame_handle_player_input_pickup_key(gamestate* const g) {
    if (!g) return;
    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
    if (hero_id == -1) return;
    if (libgame_lua_create_action(L, hero_id, ACTION_PICKUP, libgame_lua_get_entity_int(L, hero_id, "x"), libgame_lua_get_entity_int(L, hero_id, "y")))
        msuccess("pickup action created");
    else
        merror("pickup action failed to create");
    g->player_input_received = true;
}



void libgame_handle_player_wait_key(gamestate* const g) {
    if (!g) return;
    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
    if (hero_id == -1) return;
    const direction_t dir = libgame_lua_get_entity_int(L, hero_id, "direction");
    if (libgame_lua_create_action(L, hero_id, ACTION_NONE, libgame_get_x_from_dir(dir), libgame_get_y_from_dir(dir)))
        libgame_entity_anim_set(g, hero_id, SPRITEGROUP_ANIM_HUMAN_IDLE);
    else
        merror("attack action failed to create");
    g->player_input_received = true;

    //const direction_t dir = libgame_lua_get_entity_int(L, hero_id, "direction");
    //        if (libgame_lua_create_action(L, hero_id, ACTION_NONE, libgame_get_x_from_dir(dir), libgame_get_y_from_dir(dir)))
    //            libgame_entity_anim_set(g, hero_id, SPRITEGROUP_ANIM_HUMAN_IDLE);
    //        else
    //            merror("attack action failed to create");
    //        g->player_input_received = true;
}



void libgame_handle_input_player(gamestate* const g) {
    if (g && g->controlmode == CONTROLMODE_PLAYER && g->player_input_received == false) {
        const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
        //const bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        // the real setup will involve managing the player's dungeon position
        if (IsKeyPressed(KEY_KP_6) || IsKeyPressed(KEY_RIGHT))
            libgame_handle_player_input_movement_key(g, DIRECTION_RIGHT);
        else if (IsKeyPressed(KEY_KP_4) || IsKeyPressed(KEY_LEFT))
            libgame_handle_player_input_movement_key(g, DIRECTION_LEFT);
        else if (IsKeyPressed(KEY_KP_2) || IsKeyPressed(KEY_DOWN))
            libgame_handle_player_input_movement_key(g, DIRECTION_DOWN);
        else if (IsKeyPressed(KEY_KP_8) || IsKeyPressed(KEY_UP))
            libgame_handle_player_input_movement_key(g, DIRECTION_UP);
        else if (IsKeyPressed(KEY_KP_1))
            libgame_handle_player_input_movement_key(g, DIRECTION_DOWN_LEFT);
        else if (IsKeyPressed(KEY_KP_3))
            libgame_handle_player_input_movement_key(g, DIRECTION_DOWN_RIGHT);
        else if (IsKeyPressed(KEY_KP_7))
            libgame_handle_player_input_movement_key(g, DIRECTION_UP_LEFT);
        else if (IsKeyPressed(KEY_KP_9))
            libgame_handle_player_input_movement_key(g, DIRECTION_UP_RIGHT);
        else if (IsKeyPressed(KEY_A))
            libgame_handle_player_input_attack_key(g);
        //g->player_input_received = true;
        else if (IsKeyPressed(KEY_W))
            libgame_handle_player_input_pickup_key(g);
        //g->player_input_received = true;
        else if (IsKeyPressed(KEY_B))
            libgame_handle_player_input_block_key(g);
        //g->player_input_received = true;
        else if (IsKeyPressed(KEY_SPACE))
            // randomize the dungeon tiles
            libgame_lua_randomize_dungeon_tiles(L, libgame_lua_get_entity_int(L, hero_id, "x") - 2, libgame_lua_get_entity_int(L, hero_id, "y") - 2, 4, 4);
        else if (IsKeyPressed(KEY_PERIOD))
            // Wait action
            libgame_handle_player_wait_key(g);
    }
}




void libgame_update_spritegroup_by_lastmove(gamestate* const g, const entityid entity_id) {
    if (!g) return;
    const int xdir = libgame_lua_get_entity_int(L, entity_id, "last_move_x"), ydir = libgame_lua_get_entity_int(L, entity_id, "last_move_y");
    //const direction_t dir = libgame_get_dir_from_xy(xdir, ydir);
    libgame_update_spritegroup(g, entity_id, SPECIFIER_NONE, libgame_get_dir_from_xy(xdir, ydir));
    libgame_update_spritegroup_move(g, entity_id, xdir * DEFAULT_TILE_SIZE, ydir * DEFAULT_TILE_SIZE);
}




void libgame_process_turn_action(gamestate* const g, const int i) {
    if (i == -1) {
        merror("libgame_process_turn_action: i is -1");
        return;
    }

    const action_t action_type = libgame_lua_get_nth_action_type(L, i);

    if (action_type == ACTION_NONE) {
        merror("libgame_process_turn_action: action type is ACTION_NONE");
        return;
    } else if (action_type >= ACTION_COUNT) {
        merror("libgame_process_turn_action: action type is out of bounds");
        return;
    }

    const entityid id = libgame_lua_get_nth_action_id(L, i);

    if (id == -1) {
        merror("libgame_process_turn_action: id is -1");
        return;
    }

    const int x = libgame_lua_get_nth_action_x(L, i), y = libgame_lua_get_nth_action_y(L, i);

    if (x < 0 || y < 0) {
        merror("libgame_process_turn_action: x or y is negative");
        return;
    }

    // process the action
    const int result_id = libgame_lua_process_action(L, i);
    // depending on the action result depends on how we update sprite animations
    if (result_id == -1) {
        merror("libgame_process_turn_action: result_id is -1");
        libgame_update_spritegroup(g, id, SPECIFIER_NONE, libgame_get_dir_from_xy(x, y));
        return;
    }

    if (action_type == ACTION_NONE) {
        libgame_update_spritegroup(g, id, SPECIFIER_NONE, DIRECTION_NONE);
    } else if (action_type == ACTION_MOVE) {
        libgame_update_spritegroup_by_lastmove(g, result_id);
    } else if (action_type == ACTION_ATTACK) {
        // get race of entity
        race_t race = libgame_lua_get_entity_int(L, id, "race");
        int spritegroup_anim_id = SPRITEGROUP_ANIM_HUMAN_ATTACK;
        if (race == RACE_HUMAN) {
            msuccess("set human attack animation");
            spritegroup_anim_id = SPRITEGROUP_ANIM_HUMAN_ATTACK;
        } else if (race == RACE_ORC) {
            msuccess("set orc attack animation");
            spritegroup_anim_id = SPRITEGROUP_ANIM_ORC_ATTACK;
        }
        if (!libgame_entity_anim_set(g, id, spritegroup_anim_id)) merror("failed to set attack animation");
    }
}




//void libgame_process_next_action(gamestate* const g) {
//    if (!g) return;
//    const int action_count = libgame_lua_get_action_count(L);
//    if (action_count <= 0) return;
//    int current_action = libgame_lua_get_gamestate_int(L, "CurrentAction");
//    libgame_process_turn_action(g, current_action);
//    libgame_incr_current_action(g);
//}



//void libgame_process_turn_actions(gamestate* const g) {
//    if (!g) return;
//    for (int i = 1; i <= libgame_lua_get_action_count(L); i++) {
//        libgame_lua_set_gamestate_int(L, "CurrentAction", i);
//        libgame_process_turn_action(g, i);
//    }
//    libgame_lua_set_gamestate_int(L, "CurrentAction", -1);
//    libgame_lua_clear_actions(L);
//}




const char* libgame_get_str_from_dir(const direction_t dir) {
    return dir == DIRECTION_RIGHT        ? "RIGHT"
           : dir == DIRECTION_LEFT       ? "LEFT"
           : dir == DIRECTION_UP         ? "UP"
           : dir == DIRECTION_DOWN       ? "DOWN"
           : dir == DIRECTION_UP_LEFT    ? "UP_LEFT"
           : dir == DIRECTION_UP_RIGHT   ? "UP_RIGHT"
           : dir == DIRECTION_DOWN_LEFT  ? "DOWN_LEFT"
           : dir == DIRECTION_DOWN_RIGHT ? "DOWN_RIGHT"
                                         : "NONE";
}




//void libgame_process_turn(gamestate* const g) {
//    if (g) libgame_process_turn_actions(g);
//libgame_lua_clear_actions(L);
//}




void libgame_handle_caminput(gamestate* const g) {
    if (g && g->controlmode == CONTROLMODE_CAMERA) {
        const bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        const float zoom_incr = 1.00f, cam_move_incr = 4.00f;
        if (IsKeyPressed(KEY_Z) && shift && g->cam2d.zoom >= 2.0f)
            g->cam2d.zoom -= zoom_incr;
        else if (IsKeyPressed(KEY_Z))
            g->cam2d.zoom += zoom_incr;
        if (IsKeyDown(KEY_UP))
            g->cam2d.offset.y += cam_move_incr;
        else if (IsKeyDown(KEY_DOWN))
            g->cam2d.offset.y -= cam_move_incr;
        else if (IsKeyDown(KEY_LEFT))
            g->cam2d.offset.x += cam_move_incr;
        else if (IsKeyDown(KEY_RIGHT))
            g->cam2d.offset.x -= cam_move_incr;
        else if (IsKeyDown(KEY_KP_5))
            g->cam2d.offset = (Vector2){0, 0};
        else if (IsKeyDown(KEY_KP_2))
            g->cam2d.offset.y += cam_move_incr;
        else if (IsKeyDown(KEY_KP_4))
            g->cam2d.offset.x -= cam_move_incr;
        else if (IsKeyDown(KEY_KP_6))
            g->cam2d.offset.x += cam_move_incr;
        else if (IsKeyDown(KEY_KP_8))
            g->cam2d.offset.y -= cam_move_incr;
        else if (IsKeyDown(KEY_KP_0))
            g->cam2d.zoom = 2.0f;
        else if (IsKeyPressed(KEY_R))
            g->cam2d.offset = (Vector2){0, 0};
        else if (IsKeyPressed(KEY_F))
            g->cam_lockon = !g->cam_lockon;
        else if (IsKeyDown(KEY_KP_1)) {
            g->cam2d.offset.x -= cam_move_incr;
            g->cam2d.offset.y += cam_move_incr;
        } else if (IsKeyDown(KEY_KP_3)) {
            g->cam2d.offset.x += cam_move_incr;
            g->cam2d.offset.y += cam_move_incr;
        } else if (IsKeyDown(KEY_KP_7)) {
            g->cam2d.offset.x -= cam_move_incr;
            g->cam2d.offset.y -= cam_move_incr;
        } else if (IsKeyDown(KEY_KP_9)) {
            g->cam2d.offset.x += cam_move_incr;
            g->cam2d.offset.y -= cam_move_incr;
        }
    }
}




bool libgame_windowshouldclose() { return WindowShouldClose(); }




void libgame_initwindow(gamestate* const g) {
    if (!g) return;
    const char* title = libgame_lua_get_gamestate_str(L, "WindowTitle");
    if (!title) title = "Gamestate.WindowTitle not set in init.lua";
    InitWindow(libgame_lua_get_gamestate_int(L, "WindowWidth"), libgame_lua_get_gamestate_int(L, "WindowHeight"), title);
    SetWindowMonitor(1);
    SetWindowPosition(libgame_lua_get_gamestate_int(L, "WindowPosX"), libgame_lua_get_gamestate_int(L, "WindowPosY"));
    SetTargetFPS(DEFAULT_TARGET_FPS);
    SetExitKey(KEY_Q);
    g->windowwidth = libgame_lua_get_gamestate_int(L, "WindowWidth"), g->windowheight = libgame_lua_get_gamestate_int(L, "WindowHeight");
}




void libgame_closewindow() { CloseWindow(); }




void libgame_update_debug_panel_buffer(gamestate* const g) {
    if (!g) return;
    const entityid id = libgame_lua_get_gamestate_int(L, "HeroId");
    if (id == -1) return;
    const int hx = libgame_lua_get_entity_int(L, id, "x"), hy = libgame_lua_get_entity_int(L, id, "y"), last_mv_x = libgame_lua_get_entity_int(L, id, "last_move_x"),
              last_mv_y = libgame_lua_get_entity_int(L, id, "last_move_y"), hero_was_damaged = libgame_lua_get_entity_int(L, id, "was_damaged"), dw = libgame_lua_get_dungeonfloor_row_count(L),
              dh = libgame_lua_get_dungeonfloor_col_count(L), action_count = libgame_lua_get_action_count(L), entity_count = libgame_lua_get_num_entities(L),
              dir = libgame_lua_get_entity_int(L, id, "direction");

    const int current_action = libgame_lua_get_gamestate_int(L, "CurrentAction");

    const int current_turn = libgame_lua_get_gamestate_int(L, "CurrentTurn");

    const char* dir_str = libgame_get_str_from_dir(dir);
    snprintf(g->debugpanel.buffer,
             1024,
             "Framecount:   %d\n"
             "%s\n"
             "%s\n"
             "Target size:  %d,%d\n"
             "Window size:  %d,%d\n"
             "Cam.target:   %.2f,%.2f\n"
             "Cam.offset:   %.2f,%.2f\n"
             "Cam.zoom:     %.2f\n"
             "Active scene: %d\n"
             "Control mode: %d\n"
             "Hero position: %d,%d\n"
             "Inventory count: %d\n"
             "Dungeon size: %dx%d\n"
             "Entity count: %d\n"
             "Last move: %d,%d\n"
             "Was damaged: %d\n"
             "Dir: %s\n"
             "CurrentAction: %d\n"
             "Action count: %d\n"
             "Current Turn: %d\n"
             "Lock: %d\n",
             g->framecount,
             g->timebeganbuf,
             g->currenttimebuf,
             g->targetwidth,
             g->targetheight,
             g->windowwidth,
             g->windowheight,
             g->cam2d.target.x,
             g->cam2d.target.y,
             g->cam2d.offset.x,
             g->cam2d.offset.y,
             g->cam2d.zoom,
             activescene,
             g->controlmode,
             hx,
             hy,
             libgame_lua_get_inventory_count(L, id),
             dh,
             dw,
             entity_count,
             last_mv_x,
             last_mv_y,
             hero_was_damaged,
             dir_str,
             current_action,
             action_count,
             current_turn,
             g->lock);
}




void libgame_update_smoothmove(gamestate* const g, const entityid id) {
    if (!g) return;
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (!group) return;
    //const float move_unit = 1.0f;
    // only do it 1 unit at a time
    const float move = 2.0f;
    group->dest.x += group->move_x > 0 ? move : group->move_x < 0 ? -move : 0;
    group->dest.y += group->move_y > 0 ? move : group->move_y < 0 ? -move : 0;
    group->move_x += group->move_x > 0 ? -move : group->move_x < 0 ? move : 0;
    group->move_y += group->move_y > 0 ? -move : group->move_y < 0 ? move : 0;
}




void libgame_do_camera_lock_on(gamestate* const g) {
    if (!g) return;
    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_get(g->spritegroups, hero_id);
    if (hero_group && g->cam_lockon) g->cam2d.target = (Vector2){hero_group->dest.x, hero_group->dest.y};
}




//void libgame_do_one_camera_rotation(gamestate* g) {
//    if (g->do_one_rotation) {
//        g->cam2d.rotation += 8.0f;
//        if (g->cam2d.rotation >= 360.0f) {
//            g->cam2d.rotation = 0.0f;
//            g->do_one_rotation = false;
//        }
//    }
//}




void libgame_handle_npc_turn_lua(gamestate* const g, const entityid id) {
    if (!g) return;
    //minfo("libgame_handle_npc_turn_lua begin");

    int diceroll = rand() % 8;
    const int xdir = diceroll == 0 || diceroll == 6 || diceroll == 7 ? -1 : diceroll == 2 || diceroll == 3 || diceroll == 4 ? 1 : 0;
    const int ydir = diceroll == 0 || diceroll == 1 || diceroll == 2 ? -1 : diceroll == 4 || diceroll == 5 || diceroll == 6 ? 1 : 0;

    if (!libgame_lua_create_action(L, id, ACTION_MOVE, xdir, ydir)) merror("could not create npc action: move");
    //const direction_t dir = libgame_lua_get_entity_int(L, id, "direction");
    //if (!libgame_lua_create_action(L, id, ACTION_ATTACK, libgame_get_x_from_dir(dir), libgame_get_y_from_dir(dir))) merror("attack action failed to create");
}



void libgame_handle_npcs_turn_lua(gamestate* const g) {
    if (!g) return;
    //for (int i = 0; i < libgame_lua_get_num_entities(L); i++) {
    for (int i = 1; i <= libgame_lua_get_num_entities(L); i++) {
        const entityid id = libgame_lua_get_nth_entity(L, i);
        const entitytype_t type = libgame_lua_get_entity_int(L, id, "type");
        if (type == ENTITY_NPC) libgame_handle_npc_turn_lua(g, id);
    }
    libgame_lua_incr_current_turn(L);
    //msuccess("libgame_handle_npcs_turn_lua end");
}




void libgame_reset_entity_anim(gamestate* const g, entityid id) {
    if (!g) return;
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (!sg) return;
    sprite* s = spritegroup_get(sg, sg->current);
    if (s && s->num_loops >= 1) {
        sg->prev_anim = sg->current;
        sg->current = sg->default_anim;
        s->num_loops = 0;
    }
}




void libgame_reset_entities_anim(gamestate* const g) {
    if (!g) return;
    for (int i = 0; i < libgame_lua_get_num_entities(L); i++) libgame_reset_entity_anim(g, i + 1);
}




void libgame_update_entity_damaged_anim(gamestate* const g, const int i) {
    if (!g) return;
    const entityid id = libgame_lua_get_nth_entity(L, i);
    const int was_damaged = libgame_lua_get_entity_int(L, id, "was_damaged");
    const race_t race = libgame_lua_get_entity_int(L, id, "race");
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    int index = -1;
    if (was_damaged && sg) {
        //int old_index = sg->current;
        if (race == RACE_HUMAN) {
            //msuccess("human was damaged");
            index = SPRITEGROUP_ANIM_HUMAN_DMG;
            //if (index == old_index) {
            if (index == sg->current) {
                sprite* s = spritegroup_get(sg, sg->current);
                if (s && s->num_loops >= 1) {
                    index = SPRITEGROUP_ANIM_HUMAN_IDLE;
                    libgame_lua_set_entity_int(L, id, "was_damaged", 0);
                    s->num_loops = 0;
                }
            }
            libgame_entity_anim_set(g, id, index);
        } else if (race == RACE_ORC) {
            index = SPRITEGROUP_ANIM_ORC_DMG;
            if (index == sg->current) {
                sprite* s = spritegroup_get(sg, sg->current);
                if (s && s->num_loops >= 1) {
                    index = SPRITEGROUP_ANIM_ORC_IDLE;
                    libgame_lua_set_entity_int(L, id, "was_damaged", 0);
                    s->num_loops = 0;
                }
            }
            libgame_entity_anim_set(g, id, index);
        }
    }
}




void libgame_update_entities_damaged_anim(gamestate* const g) {
    if (!g) return;
    for (int i = 0; i < libgame_lua_get_num_entities(L); i++) libgame_update_entity_damaged_anim(g, i + 1);
}




void libgame_update_gamestate(gamestate* g) {
    if (!g) return;
    //UpdateMusicStream(test_music);
    libgame_update_debug_panel_buffer(g);
    //setdebugpanelcenter(g);
    // need to clean this up a bit but works in general right now for what we want
    // needs extensibility to handle all animation types
    // in essence some animations we only want to loop once, then reset to a default or previous
    libgame_update_entities_damaged_anim(g);
    libgame_reset_entities_anim(g);
    libgame_update_smoothmove(g, libgame_lua_get_gamestate_int(L, "HeroId"));
    libgame_do_camera_lock_on(g);
    // at this point, we can take other NPC turns
    // lets iterate over our entities, find the NPCs, and make them move in a random direction
    // then, we will update their smooth moves
    // we will need to eventually disable player input during smooth moving
    // update smooth move for NPCs and other entities
    libgame_update_smoothmoves_for_entitytype(g, ENTITY_NPC);
    // i plan on using the lock and lock timer to control an ebb-and-flow
    // so that it doesnt appear like enemies move immediately as the player
    // does
    if (g->player_input_received) {
        libgame_handle_npcs_turn_lua(g);
        // this is where we want to process the turn
        // we want to do this in a lock-step fashion so that it is only called once every N frames or so
        g->processing_actions = true;
        g->player_input_received = false;
    }

    if (g->processing_actions && g->lock == 0) {
        libgame_process_turn_alt(g);
        g->lock = DEFAULT_LOCK;
    } else if (g->processing_actions && g->lock > 0) {
        g->lock--;
    }


    const int action_count = libgame_lua_get_action_count(L);
    if (action_count == 0) {
        g->processing_actions = false;
        g->player_input_received = false;
        g->lock = 0;
    }
}




void libgame_update_smoothmoves_for_entitytype(gamestate* const g, const entitytype_t type) {
    if (!g) return;
    //const int count = libgame_lua_get_num_entities(L);
    for (int i = 0; i < libgame_lua_get_num_entities(L); i++) {
        const entityid id = libgame_lua_get_nth_entity(L, i + 1);
        if (type == libgame_lua_get_entity_int(L, id, "type")) libgame_update_smoothmove(g, id);
    }
}




void libgame_drawframeend(gamestate* const g) {
    if (!g) return;
    EndDrawing();
    // update gamestate values at end of draw frame:
    // framecount, currenttime, currenttimetm, currenttimebuf
    g->framecount++;
    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&(g->currenttime));
    strftime(g->currenttimebuf, 64, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
}




void libgame_drawframe(gamestate* g) {
    if (!g) return;
    BeginDrawing();
    BeginTextureMode(target);
    switch (activescene) {
    //case SCENE_COMPANY:
    //libgame_draw_company_scene(g);
    //    break;
    case SCENE_TITLE:
        libgame_draw_title_scene(g);
        break;
    case SCENE_GAMEPLAY:
        libgame_draw_gameplayscene(g);
        break;
    default:
        break;
    }
    EndTextureMode();
    DrawTexturePro(target.texture, target_src, target_dest, target_origin, 0.0f, WHITE);
    libgame_draw_debug_panel(g);
    libgame_drawframeend(g);
}




void libgame_calc_debugpanel_size(gamestate* const g) {
    if (!g) return;
    const int sz = 14, sp = 1;
    const Vector2 m = MeasureTextEx(g->font, g->debugpanel.buffer, sz, sp);
    g->debugpanel.w = m.x, g->debugpanel.h = m.y;
}




inline void libgame_draw_debug_panel(gamestate* const g) {
    if (g && g->debugpanelon) {
        const int fontsize = 14, spacing = 1, xy = 10, wh = 20;
        const Color c = {0x33, 0x33, 0x33, 255}, c2 = WHITE;
        const Vector2 p = {g->debugpanel.x, g->debugpanel.y}, o = {0, 0};
        const int x0 = g->debugpanel.x - xy, y0 = g->debugpanel.y - xy, w0 = g->debugpanel.w + wh, h0 = g->debugpanel.h + wh;
        const Rectangle box = {x0, y0, w0, h0};
        DrawRectanglePro(box, o, 0.0f, c);
        DrawTextEx(g->font, g->debugpanel.buffer, p, fontsize, spacing, c2);
    }
}




void libgame_drawgrid(gamestate* const g) {
    if (!g) return;
    const int len = libgame_lua_get_dungeonfloor_row_count(L), wid = libgame_lua_get_dungeonfloor_col_count(L);
    for (int i = 0; i <= len; i++) DrawLine(i * DEFAULT_TILE_SIZE, 0, i * DEFAULT_TILE_SIZE, wid * DEFAULT_TILE_SIZE, GREEN);
    for (int i = 0; i <= wid; i++) DrawLine(0, i * DEFAULT_TILE_SIZE, len * DEFAULT_TILE_SIZE, i * DEFAULT_TILE_SIZE, GREEN);
}




void libgame_draw_dungeonfloor(gamestate* const g) {
    if (!g) return;
    const Rectangle tile_src = {0, 0, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE};
    Rectangle tile_dest = {0, 0, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE};
    //const int rows = libgame_lua_get_dungeonfloor_row_count(L), cols = libgame_lua_get_dungeonfloor_col_count(L);
    for (int i = 0; i < libgame_lua_get_dungeonfloor_row_count(L); i++)
        for (int j = 0; j < libgame_lua_get_dungeonfloor_col_count(L); j++) {
            const int type = libgame_lua_get_tiletype(L, j, i);
            const int key = get_txkey_for_tiletype(type);
            if (key == -1) continue;
            tile_dest.x = j * DEFAULT_TILE_SIZE, tile_dest.y = i * DEFAULT_TILE_SIZE;
            DrawTexturePro(g->txinfo[key].texture, tile_src, tile_dest, (Vector2){0, 0}, 0, WHITE);
        }
}




void libgame_draw_entity_shadow(gamestate* const g, const entityid id) {
    if (!g) return;
    const entitytype_t type = libgame_lua_get_entity_int(L, id, "type");
    if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
        spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
        // draw entity shadow, which should exist at current+1 if loaded correctly
        DrawTexturePro(*group->sprites[group->current + 1]->texture, group->sprites[group->current + 1]->src, group->dest, (Vector2){0, 0}, 0.0f, WHITE);
        // when time comes to handle blocking with a shield, we will also need to draw
        // the 'back' component of the shield first before drawing the entity
    }
}




void libgame_draw_entity_shield_back(gamestate* const g, const entityid id) {
    if (!g) return;
    const entityid shield_id = libgame_lua_get_entity_shield(L, id);
    const entitytype_t type = libgame_lua_get_entity_int(L, id, "type");
    if (shield_id != -1 && type == ENTITY_PLAYER) {
        spritegroup_t* group = hashtable_entityid_spritegroup_get_by_specifier(g->spritegroups, id, SPECIFIER_NONE);
        spritegroup_t* shield_group = hashtable_entityid_spritegroup_get_by_specifier(g->spritegroups, shield_id, SPECIFIER_SHIELD_BLOCK);
        if (group && shield_group && group->current == SPRITEGROUP_ANIM_HUMAN_GUARD) {
            shield_group->dest.x = group->dest.x;
            shield_group->dest.y = group->dest.y;
            //minfo("updating shield spritegroup...");
            DrawTexturePro(*shield_group->sprites[shield_group->current + 1]->texture, shield_group->sprites[shield_group->current + 1]->src, shield_group->dest, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }
}




void libgame_draw_entity_shield_front(gamestate* const g, const entityid id) {
    if (!g) return;
    const entityid shield_id = libgame_lua_get_entity_shield(L, id);
    const entitytype_t type = libgame_lua_get_entity_int(L, id, "type");
    if (shield_id != -1 && type == ENTITY_PLAYER) {
        spritegroup_t* group = hashtable_entityid_spritegroup_get_by_specifier(g->spritegroups, id, SPECIFIER_NONE);
        spritegroup_t* shield_group = hashtable_entityid_spritegroup_get_by_specifier(g->spritegroups, shield_id, SPECIFIER_SHIELD_BLOCK);
        if (group && shield_group && group->current == SPRITEGROUP_ANIM_HUMAN_GUARD) {
            // to demo the buckler animating on the hero, we will need to pass in the shield id
            // normally we would query the hero to see which shield is equipped
            // but for now we will introduce a global buckler_id as a test
            shield_group->dest.x = group->dest.x, shield_group->dest.y = group->dest.y;
            DrawTexturePro(*shield_group->sprites[shield_group->current]->texture, shield_group->sprites[shield_group->current]->src, shield_group->dest, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }
}




void libgame_draw_entity_incr_frame(gamestate* const g, const entityid id) {
    if (!g) return;
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    const entitytype_t type = libgame_lua_get_entity_int(L, id, "type");
    if (type == ENTITY_SHIELD) group = hashtable_entityid_spritegroup_get_by_specifier(g->spritegroups, id, SPECIFIER_SHIELD_ON_TILE);
    if (g->framecount % FRAMEINTERVAL == 0) {
        sprite_incrframe(group->sprites[group->current]);
        if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
            sprite_incrframe(group->sprites[group->current + 1]);
            if (type == ENTITY_PLAYER && group->current == SPRITEGROUP_ANIM_HUMAN_GUARD) {
                specifier_t spec = SPECIFIER_SHIELD_BLOCK;
                const entityid shieldid = libgame_lua_get_entity_shield(L, id);
                spritegroup_t* buckler_group = hashtable_entityid_spritegroup_get_by_specifier(g->spritegroups, shieldid, spec);
                if (!buckler_group) return;
                sprite_incrframe(buckler_group->sprites[buckler_group->current]);
                sprite_incrframe(buckler_group->sprites[buckler_group->current + 1]);
            }
        }
    }
}




void libgame_draw_entity(gamestate* const g, const entityid id) {
    if (!g) return;
    const entitytype_t type = libgame_lua_get_entity_int(L, id, "type");
    const specifier_t spec = type == ENTITY_SHIELD ? SPECIFIER_SHIELD_ON_TILE : SPECIFIER_NONE;
    spritegroup_t* group = hashtable_entityid_spritegroup_get_by_specifier(g->spritegroups, id, spec);
    if (!group) return;
    // draw entity shadow
    libgame_draw_entity_shadow(g, id);
    libgame_draw_entity_shield_back(g, id);
    // draw the main entity sprite
    DrawTexturePro(*group->sprites[group->current]->texture, group->sprites[group->current]->src, group->dest, (Vector2){0, 0}, 0.0f, WHITE);
    libgame_draw_entity_shield_front(g, id);
    libgame_draw_entity_incr_frame(g, id);
    if (g->debugpanelon) {
        const int x = group->dest.x, y = group->dest.y, w = group->dest.width, h = group->dest.height;
        // first draw the outer rectangle without the offset
        //Vector2 v[4] = {{x, y}, {x + w, y }, {x + w , y + h }, {x , y + h }};
        const Vector2 v[4] = {{x - group->off_x, y - group->off_y}, {x + group->off_x + w, y - group->off_y}, {x + w + group->off_x, y + h + group->off_y}, {x - group->off_x, y + h + group->off_y}};
        DrawLineV(v[0], v[1], (Color){0, 0, 255, 255});
        DrawLineV(v[1], v[2], (Color){0, 0, 255, 255});
        DrawLineV(v[2], v[3], (Color){0, 0, 255, 255});
        DrawLineV(v[3], v[0], (Color){0, 0, 255, 255});
        //DrawLineV(v[0], v[1], c);
        //DrawLineV(v[1], v[2], c);
        //DrawLineV(v[2], v[3], c);
        //DrawLineV(v[3], v[0], c);
        // now lets draw it with the offset
        //v[0] = (Vector2){x - group->off_x, y - group->off_y};
        //v[1] = (Vector2){x + group->off_x + w, y - group->off_y};
        //v[2] = (Vector2){x + w + group->off_x, y + h + group->off_y};
        //v[3] = (Vector2){x - group->off_x, y + h + group->off_y};
    }
}




void libgame_draw_entities_at_lua(gamestate* const g, const entitytype_t type, const int x, const int y) {
    if (!g) return;
    //minfo("libgame_draw_entities_at_lua");
    //const int num_entities = libgame_lua_get_num_entities_at(L, x, y);
    for (int k = 0; k < libgame_lua_get_num_entities_at(L, x, y); k++) {
        const entityid id = libgame_lua_get_nth_entity_at(L, k + 1, x, y);
        const entitytype_t type2 = libgame_lua_get_entity_int(L, id, "type");
        //if (type == type2)
        //    libgame_draw_entity(g, id);
        (type == type2) ? libgame_draw_entity(g, id) : 0;
    }
}




void libgame_draw_gameplayscene_entities(gamestate* const g) {
    if (!g) return;
    for (int i = 0; i < libgame_lua_get_dungeonfloor_row_count(L); i++)
        for (int j = 0; j < libgame_lua_get_dungeonfloor_col_count(L); j++) {
            //libgame_draw_items(g, ITEM_TORCH, i, j);
            //libgame_draw_items_that_are_not(g, ITEM_TORCH, i, j);
            //libgame_draw_entities_at(g, ENTITY_NPC, i, j);
            //libgame_draw_entities_at(g, ENTITY_PLAYER, i, j);
            libgame_draw_entities_at_lua(g, ENTITY_SHIELD, i, j);
            libgame_draw_entities_at_lua(g, ENTITY_NPC, i, j);
            libgame_draw_entities_at_lua(g, ENTITY_PLAYER, i, j);
        }
}




void libgame_draw_gameplayscene(gamestate* const g) {
    if (!g) return;
    BeginMode2D(g->cam2d);
    ClearBackground(BLACK);
    libgame_draw_dungeonfloor(g);
    if (g->gridon) libgame_drawgrid(g);
    // draw torches, items, npcs, player
    libgame_draw_gameplayscene_entities(g);
    // lighting basics
    //const int tilesize = DEFAULT_TILE_SIZE;
    //for (int i = 0; i < g->dungeonfloor->len; i++) {
    //    for (int j = 0; j < g->dungeonfloor->wid; j++) {
    //int lightlvl = 200;
    //if (libgame_itemtype_is_at(g, (Vector2){i, j}, ITEM_TORCH)) {
    //    lightlvl = 100;
    //}
    //DrawRectangle(i * tilesize, j * tilesize, tilesize, tilesize, (Color){0, 0, 0, lightlvl});
    //    }
    //}
    libgame_handle_fade(g);
    EndMode2D();
    // disabled for now
    //DrawRectangle(0, 0, 100, 100, BLACK);
    //DrawRectangleLines(10, 10, 80, 80, WHITE);
    //DrawTextEx(g->font, "message log", (Vector2){20, 20}, 12, 1, WHITE);
}




void libgame_draw_title_scene(gamestate* const g) {
    if (!g) return;
    const Color bgc = {0x66, 0x66, 0x66, 255}, fgc = WHITE, fgc2 = BLACK;
    char b[128 * 3];
    char *b2 = b + 128, *b3 = b + 128 * 2;
    snprintf(b, 128, "project");
    snprintf(b2, 128, "rpg");
    snprintf(b3, 128, "press space to continue");
    const Vector2 m = MeasureTextEx(g->font, b, 40, 2), m2 = MeasureTextEx(g->font, b2, 40, 2), m3 = MeasureTextEx(g->font, b3, 16, 1);
    const float tw2 = g->targetwidth / 2.0f, th2 = g->targetheight / 2.0f;
    const int offset = 100, x = tw2 - m.x / 2.0f - offset, y = th2 - m.y / 2.0f, x2 = tw2 - m2.x / 2.0f + offset, x3 = tw2 - m3.x / 2.0f, y3 = th2 + m3.y / 2.0f + 20;
    const Vector2 pos[3] = {{x, y}, {x2, y}, {x3, y3}};
    ClearBackground(bgc);
    DrawTextEx(g->font, b, pos[0], 40, 4, fgc);
    DrawTextEx(g->font, b2, pos[1], 40, 1, fgc2);
    // just below the 'project rpg' text
    DrawTextEx(g->font, b3, pos[2], 16, 1, fgc);
    libgame_handle_fade(g);
}




//void libgame_draw_company_scene(gamestate* g) {
//    const Color bgc = BLACK;
//    const Color fgc = {0x66, 0x66, 0x66, 255};
//    const int fontsize = 32;
//    const int spacing = 1;
//    const int interval = 120;
//    const int dur = 60;
//    char b[128 * 3];
//    bzero(b, 128 * 3);
//    char* b2 = b + 128;
//    char* b3 = b + 128 * 2;
//    snprintf(b, 128, COMPANYNAME);
//    snprintf(b2, 128, COMPANYFILL);
//    snprintf(b3, 128, "presents");
//    const Vector2 measure = MeasureTextEx(g->font, b, fontsize, spacing);
//    if (g->framecount % interval >= 0 && g->framecount % interval < dur) {
//        for (int i = 0; i < 10; i++) {
//            shufflestrinplace(b);
//            shufflestrinplace(b3);
//        }
//    }
//    for (int i = 0; i < 10; i++) {
//        shufflestrinplace(b2);
//    }
//    const Vector2 pos = {targetwidth / 2.0f - measure.x / 2.0f, targetheight / 2.0f - measure.y / 2.0f};
//    ClearBackground(bgc);
//    DrawTextEx(g->font, b, pos, fontsize, 1, fgc);
//    DrawTextEx(g->font, b2, pos, fontsize, 1, fgc);
//    const Vector2 measure3 = MeasureTextEx(g->font, b3, 20, 1);
//    const Vector2 pp = {targetwidth / 2.0f - measure3.x / 2.0f, targetheight / 2.0f + measure.y / 2.0f + 20};
//    DrawTextEx(g->font, b3, pp, 20, 1, fgc);
//    libgame_handle_fade(g);
//}




void libgame_load_texture_from_disk(gamestate* const g, const int index, const int contexts, const int frames, const bool dodither, const char* path) {
    if (!g) return;
    if (dodither) {
        Image img = LoadImage(path);
        ImageDither(&img, 4, 4, 4, 4);
        Texture2D t = LoadTextureFromImage(img);
        g->txinfo[index].texture = t;
        UnloadImage(img);
    } else
        g->txinfo[index].texture = LoadTexture(path);
    g->txinfo[index].num_frames = frames;
    g->txinfo[index].contexts = contexts;
    msuccess("libgame_loadtexture");
    //SetTextureFilter(textures[index], TEXTURE_FILTER_POINT);
    //SetTextureFilter(txinfo[index].texture, TEXTURE_FILTER_POINT);
}




void libgame_load_textures_from_disk(gamestate* const g) {
    if (!g) return;
    const char* texture_file_path = "textures.txt";
    FILE* fp = fopen(texture_file_path, "r");
    if (!fp) {
        merror("libgame_loadtextures_from_disk: could not open file");
        return;
    }
    char line[512], path[256];
    bzero(line, 512);
    bzero(path, 256);
    while (fgets(line, 512, fp)) {
        // if line begins with #, skip it
        if (line[0] == '#') continue;
        // if line is empty, skip it
        if (strlen(line) == 0) continue;
        // parse the line
        int index = -1, contexts = -1, frames = -1, dodither = false;
        sscanf(line, "%d %d %d %d %s", &index, &contexts, &frames, &dodither, path);
        libgame_load_texture_from_disk(g, index, contexts, frames, dodither, path);
        bzero(line, 512);
        bzero(path, 256);
    }
    fclose(fp);
}




void libgame_load_textures(gamestate* const g) {
    if (!g) return;
    libgame_load_textures_from_disk(g);
}



void libgame_unloadtexture(gamestate* const g, const int index) {
    if (!g) return;
    if (g->txinfo[index].texture.id > 0) UnloadTexture(g->txinfo[index].texture);
}




void libgame_unloadtextures(gamestate* const g) {
    if (!g) return;
    //minfo("unloading textures");
    // this can be done smarter, surely...
    for (int i = TX_DIRT_00; i <= TX_BUCKLER; i++) libgame_unloadtexture(g, i);
}




//void libgame_reloadtextures(gamestate* g) {
//    libgame_unloadtextures(g);
//    libgame_loadtextures(g);
//}




void libgame_init() {
    g = gamestateinitptr();
    libgame_initsharedsetup(g);
    msuccess("libgame_init");
}




void libgame_create_spritegroup_by_id(gamestate* const g, const entityid id) {
    if (!g || id < 0) return;
    // want to pre-select values to feed into create_spritegroup by
    // entity-type
    //    if player or NPC, by race
    //    if item, by itemtype and specs
    const entitytype_t entity_type = libgame_lua_get_entity_int(L, id, "type");
    if (entity_type == ENTITY_PLAYER || entity_type == ENTITY_NPC) {
        race_t race = libgame_lua_get_entity_int(L, id, "race");
        if (race == RACE_HUMAN) {
            libgame_create_spritegroup(g, id, TX_HUMAN_KEYS, TX_HUMAN_KEY_COUNT, -12, -12, SPECIFIER_NONE);
            libgame_set_default_anim_for_id(g, id, SPRITEGROUP_ANIM_HUMAN_IDLE);
        } else if (race == RACE_ORC) {
            libgame_create_spritegroup(g, id, TX_ORC_KEYS, TX_ORC_KEY_COUNT, -12, -12, SPECIFIER_NONE);
            libgame_set_default_anim_for_id(g, id, SPRITEGROUP_ANIM_ORC_IDLE);
        }
    } else if (entity_type == ENTITY_SHIELD) {
        // first we will create the spritegroup for the dungeon-item representation
        // then we will create the spritegroups for being equipped on the player
        // they will get attached in the hashtable at the same location via chaining
        // we will select the proper sprite during rendering by attaching filters to the spritegroups
        libgame_create_spritegroup(g, id, TX_BUCKLER_KEYS, TX_BUCKLER_KEY_COUNT, 0, 0, SPECIFIER_SHIELD_ON_TILE);

        libgame_create_spritegroup(g, id, TX_GUARD_BUCKLER_KEYS, TX_GUARD_BUCKLER_KEY_COUNT, -10, -14, SPECIFIER_SHIELD_BLOCK);
        libgame_set_default_anim_for_id(g, id, SPRITEGROUP_ANIM_GUARD_BUCKLER);

        libgame_create_spritegroup(g, id, TX_GUARD_BUCKLER_SUCCESS_KEYS, TX_GUARD_BUCKLER_SUCCESS_KEY_COUNT, -12, -12, SPECIFIER_SHIELD_BLOCK_SUCCESS);
        libgame_set_default_anim_for_id(g, id, SPRITEGROUP_ANIM_GUARD_BUCKLER);
    }
}




void libgame_set_default_anim_for_id(gamestate* const g, const entityid id, const int anim) {
    if (!g) return;
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (!group) return;
    group->default_anim = anim;
}




void libgame_create_spritegroup(gamestate* const g, const entityid id, int* keys, const int num_keys, const int offset_x, const int offset_y, const specifier_t spec) {
    if (!g) return;
    spritegroup_t* group = spritegroup_create(20);
    const int x = libgame_lua_get_entity_int(L, id, "x"), y = libgame_lua_get_entity_int(L, id, "y"), dw = libgame_lua_get_dungeonfloor_col_count(L), dh = libgame_lua_get_dungeonfloor_row_count(L);
    if (x < 0 || x > dw || y < 0 || y > dh) return;
    for (int i = 0; i < num_keys; i++) spritegroup_add(group, sprite_create(&g->txinfo[keys[i]].texture, g->txinfo[keys[i]].contexts, g->txinfo[keys[i]].num_frames));
    // this is effectively how we will update the
    // sprite position in relation to the entity's
    // dungeon position
    const float w = spritegroup_get(group, 0)->width, h = spritegroup_get(group, 0)->height, dst_x = x * DEFAULT_TILE_SIZE, dst_y = y * DEFAULT_TILE_SIZE;
    group->current = 0;
    group->dest = (Rectangle){dst_x + offset_x, dst_y + offset_y, w, h};
    group->off_x = offset_x, group->off_y = offset_y;
    spritegroup_set_specifier(group, spec);
    // add the spritegroup to the hashtable
    hashtable_entityid_spritegroup_insert(g->spritegroups, id, group);
}




void libgame_loadtargettexture(gamestate* const g) {
    if (!g) return;
    g->targetwidth = libgame_lua_get_gamestate_int(L, "TargetWidth"), g->targetheight = libgame_lua_get_gamestate_int(L, "TargetHeight");
    target = LoadRenderTexture(g->targetwidth, g->targetheight);
    target_src = (Rectangle){0, 0, g->targetwidth, -g->targetheight}, target_dest = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};
    //SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    // update the gamestate display values
    g->cam2d.offset.x = g->targetwidth / 2.0f, g->cam2d.offset.y = g->targetheight / 4.0f;
}




void libgame_loadfont(gamestate* const g) {
    if (g) g->font = LoadFontEx(DEFAULT_FONT_PATH, 60, 0, 255);
}




void libgame_init_datastructures(gamestate* g) {
    if (!g) return;
    g->spritegroups = hashtable_entityid_spritegroup_create(DEFAULT_HASHTABLE_ENTITYID_SPRITEGROUP_SIZE);
    libgame_lua_create_dungeonfloor(L, 16, 16, TILETYPE_DIRT_00);
}




const entityid libgame_create_hero_lua(gamestate* const g, const char* name, const int x, const int y) {
    if (!g) return -1;
    const entitytype_t entity_type = ENTITY_PLAYER;
    const entityid id = libgame_lua_create_entity(L, name, entity_type, x, y);
    if (id == -1) {
        merror("libgame_create_hero_lua: could not create hero entity, expect crashes");
        libgame_lua_set_gamestate_int(L, "HeroId", -1);
        return -1;
    }
    libgame_lua_set_gamestate_int(L, "HeroId", id);
    libgame_lua_set_entity_int(L, id, "race", RACE_HUMAN);
    libgame_lua_set_entity_int(L, id, "direction", DIRECTION_DOWN_RIGHT);
    libgame_create_spritegroup_by_id(g, id);
    libgame_update_spritegroup(g, id, SPECIFIER_NONE, DIRECTION_DOWN_RIGHT);
    return id;
}




const entityid libgame_create_buckler_lua(gamestate* const g, const char* name, const int x, const int y) {
    if (!g || !name || x < 0 || y < 0) return -1;
    const int dw = libgame_lua_get_dungeonfloor_col_count(L), dh = libgame_lua_get_dungeonfloor_row_count(L);
    if (x >= dw || y >= dh) return -1;
    const entityid id = libgame_lua_create_entity(L, name, ENTITY_SHIELD, x, y);
    if (id == -1) return -1;
    // set buckler race
    libgame_lua_set_entity_int(L, id, "race", RACE_NONE);
    libgame_create_spritegroup_by_id(g, id);
    libgame_update_spritegroup(g, id, SPECIFIER_SHIELD_ON_TILE, DIRECTION_NONE);
    libgame_update_spritegroup(g, id, SPECIFIER_SHIELD_BLOCK, DIRECTION_DOWN_RIGHT);
    return id;
}




const entityid libgame_create_orc_lua(gamestate* const g, const char* name, const int x, const int y) {
    const int dw = libgame_lua_get_dungeonfloor_col_count(L), dh = libgame_lua_get_dungeonfloor_row_count(L);
    if (!g || !name || x < 0 || y < 0 || x >= dw || y >= dh) return -1;
    const entityid id = libgame_lua_create_entity(L, name, ENTITY_NPC, x, y);
    if (id == -1) return -1;
    libgame_lua_set_entity_int(L, id, "race", RACE_ORC);
    libgame_create_spritegroup_by_id(g, id);
    return id;
}




//void libgame_init_sound() {
//InitAudioDevice();
//SetAudioStreamBufferSizeDefault(2048);
//test_music = LoadMusicStream("./evildojo.mp3");
//PlayMusicStream(test_music);
//}




void libgame_initsharedsetup(gamestate* const g) {
    if (!g) return;
    // init lua
    L = luaL_newstate();
    luaL_openlibs(L);
    const char* filename = "init.lua";
    if (luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0)) luaL_error(L, "cannot run %s: %s", filename, lua_tostring(L, -1));
    //#ifdef MOBILE
    // just testing on my own phone for now, pixel 6a
    //        const int tw = 720;
    //        const int th = 1400;
    //        const int scale = libgame_lua_get_int(L, "Scale");
    //        const int dw = tw * scale;
    //        const int dh = th * scale;
    //        libgame_lua_set_gamestate_int(L, "TargetWidth", tw);
    //        libgame_lua_set_gamestate_int(L, "TargetHeight", th);
    //        libgame_lua_set_gamestate_int(L, "WindowWidth", dw);
    //        libgame_lua_set_gamestate_int(L, "WindowHeight", dh);
    //#endif
    libgame_initwindow(g);
    //InitAudioDevice();
    //SetAudioStreamBufferSizeDefault(2048);
    //test_music = LoadMusicStream("./evildojo.mp3");
    //PlayMusicStream(test_music);
    SetRandomSeed(time(NULL));
    g->font = LoadFontEx(DEFAULT_FONT_PATH, 60, 0, 255);
    g->targetwidth = libgame_lua_get_gamestate_int(L, "TargetWidth"), g->targetheight = libgame_lua_get_gamestate_int(L, "TargetHeight");
    target = LoadRenderTexture(g->targetwidth, g->targetheight);
    target_src = (Rectangle){0, 0, g->targetwidth, -g->targetheight};
    target_dest = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
    // update the gamestate display values
    // can get this from init.lua....
    g->cam2d.offset.x = g->targetwidth / 2.0f, g->cam2d.offset.y = g->targetheight / 4.0f;
    libgame_load_textures(g);
    g->spritegroups = hashtable_entityid_spritegroup_create(DEFAULT_HASHTABLE_ENTITYID_SPRITEGROUP_SIZE);
    libgame_lua_create_dungeonfloor(L, 16, 16, TILETYPE_DIRT_00);
    if (libgame_create_hero_lua(g, "hero", 1, 1) == -1) merror("libgame_initsharedsetup: could not create hero entity");
    if (libgame_create_buckler_lua(g, "buckler", 2, 1) == -1) merror("libgame_initsharedsetup: could not create buckler entity");
    // these dont work right until the text buffer of the debugpanel is filled
    libgame_update_debug_panel_buffer(g);
    libgame_calc_debugpanel_size(g);
    setdebugpanelbottomleft(g);
}




void libgame_initwithstate(gamestate* const state) {
    if (state != NULL) {
        g = state;
        libgame_initsharedsetup(g);
    }
}




void libgame_closesavegamestate() { libgame_closeshared(g); }




void libgame_close(gamestate* g) {
    if (!g) return;
    libgame_closeshared(g);
    gamestatefree(g);
}




void libgame_closeshared(gamestate* const g) {
    if (!g) return;
    // dont need to free most of gamestate
    //minfo("libgame_closeshared");
    //UnloadMusicStream(test_music);
    //CloseAudioDevice();
    UnloadFont(g->font);
    libgame_unloadtextures(g);
    UnloadRenderTexture(target);
    CloseWindow();
    lua_close(L);
    msuccess("libgame_closeshared end");
}




gamestate* libgame_getgamestate() { return g; }




const bool libgame_external_check_reload() { return IsKeyPressed(KEY_R); }




void libgame_incr_current_action(gamestate* const g) {
    if (!g) return;

    int current_action = libgame_lua_get_gamestate_int(L, "CurrentAction");
    const int action_count = libgame_lua_get_action_count(L);

    if (current_action == -1) {
        current_action = 1;
    } else if (current_action >= action_count) {
        //current_action = -1;
        current_action = 1;
        libgame_lua_clear_actions(L);
    } else {
        current_action++;
    }
    libgame_lua_set_gamestate_int(L, "CurrentAction", current_action);
}


void libgame_incr_current_action_key(gamestate* const g) {
    if (!g) return;

    libgame_incr_current_action(g);

    //int current_action = libgame_lua_get_gamestate_int(L, "CurrentAction");
    //const int action_count = libgame_lua_get_action_count(L);
    //if (current_action == -1) {
    //    current_action = 1;
    //} else if (current_action > action_count) {
    //    current_action = -1;
    //} else {
    //    current_action++;
    //}
    //libgame_lua_set_gamestate_int(L, "CurrentAction", current_action);
}
