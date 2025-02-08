#include "libgame.h"
//#include "actionresults.h"
#include "controlmode.h"
#include "direction.h"
#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "dungeon_tile_type.h"
#include "em.h"
#include "entity.h"
#include "entitytype.h"
#include "fadestate.h"
#include "gamestate.h"
#include "get_txkey_for_tiletype.h"
#include "hashtable_entityid_spritegroup.h"
#include "libgame_defines.h"
#include "mprint.h"
#include "scene.h"
#include "setdebugpanel.h"
#include "specifier.h"
#include "sprite.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"
#include "tx_keys.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


//------------------------------------------------------------------
// libgame global variables
//------------------------------------------------------------------
gamestate* g = NULL;

RenderTexture target;

Rectangle target_src = {0, 0, 0, 0};
Rectangle target_dest = {0, 0, 0, 0};

const Rectangle tile_src = {0, 0, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE};
Rectangle tile_dest = {0, 0, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE};

const Vector2 zero_vec = {0, 0};
const Vector2 target_origin = {0, 0};

int activescene = GAMEPLAYSCENE;

entityid next_entity_id = 0;

em_t* entitymap = NULL;

entityid* entityids = NULL;

void libgame_draw_fade(const gamestate* const g) {
    if (!g) {
        merror("libgame_draw_fade: gamestate is NULL");
        return;
    }
    if (g->fadealpha > 0) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, g->fadealpha});
    }
}



void libgame_handle_fade(gamestate* const g) {
    if (!g) {
        merror("libgame_handle_fade: gamestate is NULL");
        return;
    }
    const int fadespeed = 4;
    // modify the fadealpha
    g->fadealpha += g->fadestate == FADESTATEOUT && g->fadealpha < 255 ? fadespeed
                    : g->fadestate == FADESTATEIN && g->fadealpha > 0  ? -fadespeed
                                                                       : 0;
    // handle scene rotation based on fadealpha
    if (g->fadealpha >= 255) {
        g->fadealpha = 255, g->fadestate = FADESTATEIN;
        activescene = activescene + 1 > 2 ? 0 : activescene + 1;
    }
    // halt fade if fadealpha is 0
    if (g->fadealpha <= 0) {
        g->fadealpha = 0, g->fadestate = FADESTATENONE;
    }
    libgame_draw_fade(g);
}




const bool libgame_entity_set_anim(gamestate* const g, const entityid id, const int index) {
    if (!g) {
        merror("libgame_entity_set_anim: gamestate is NULL");
        return false;
    }
#ifdef DEBUG
    char buf[1024];
    bzero(buf, 1024);
    snprintf(buf, 1024, "libgame_entity_anim_set: id: %d, index: %d", id, index);
    minfo(buf);
#endif
    return spritegroup_set_current(hashtable_entityid_spritegroup_get(g->spritegroups, id), index);
}




//void libgame_test_enemy_placement(gamestate* const g) {
//    if (!g) {
//        merror("test_enemy_placement: gamestate is NULL");
//        return;
//    }
//const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
//if (hero_id == -1) {
//    merror("test_enemy_placement: hero_id is -1");
//    return;
//}
//const int x = libgame_get_hero_x(g);
//const int y = libgame_get_hero_y(g);
//    int x, y;
//    libgame_get_hero_xy(g, &x, &y);
//    x++;
//    if (libgame_lua_tile_is_occupied_by_npc(L, x, y)) {
//        mwarning("test_enemy_placement: tile is occupied by npc");
//    } else if (libgame_create_orc(g, "orc", x, y) == -1) {
//        mwarning("test_enemy_placement: failed to create orc");
//    }
//}


const int libgame_get_x_from_dir(const direction_t dir) {
    return dir == DIRECTION_RIGHT || dir == DIRECTION_DOWN_RIGHT || dir == DIRECTION_UP_RIGHT ? 1
           : dir == DIRECTION_LEFT || dir == DIRECTION_DOWN_LEFT || dir == DIRECTION_UP_LEFT  ? -1
                                                                                              : 0;
}



const int libgame_get_y_from_dir(const direction_t dir) {
    return dir == DIRECTION_DOWN || dir == DIRECTION_DOWN_LEFT || dir == DIRECTION_DOWN_RIGHT ? 1
           : dir == DIRECTION_UP || dir == DIRECTION_UP_LEFT || dir == DIRECTION_UP_RIGHT     ? -1
                                                                                              : 0;
}




//void libgame_get_xy_from_dir(int* x, int* y, const direction_t dir) {
//    if (!x) {
//        merror("libgame_get_xy_from_dir: x is NULL");
//        return;
//    }
//    if (!y) {
//        merror("libgame_get_xy_from_dir: y is NULL");
//        return;
//    }
//    *x = dir == DIRECTION_DOWN || dir == DIRECTION_DOWN_LEFT || dir == DIRECTION_DOWN_RIGHT ? 1
//         : dir == DIRECTION_UP || dir == DIRECTION_UP_LEFT || dir == DIRECTION_UP_RIGHT     ? -1
//                                                                                            : 0;
//    *y = dir == DIRECTION_DOWN || dir == DIRECTION_DOWN_LEFT || dir == DIRECTION_DOWN_RIGHT ? 1
//         : dir == DIRECTION_UP || dir == DIRECTION_UP_LEFT || dir == DIRECTION_UP_RIGHT     ? -1
//                                                                                            : 0;
//}




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




void libgame_handle_move(gamestate* const g, const entityid id, const int x, const int y) {
    //void libgame_handle_move_left(gamestate* const g, const entityid id) {
    entity_t* e = em_get(entitymap, id);
    if (e == NULL) {
        merror("libgame_handle_player_input_movement_key: e is NULL");
        return;
    }

    if (e->x < 0 || e->x >= g->dungeon_floor->width) {
        merror("libgame_handle_player_input_movement_key: e->x is out of bounds");
        return;
    }

    if (e->y < 0 || e->y >= g->dungeon_floor->height) {
        merror("libgame_handle_player_input_movement_key: e->y is out of bounds");
        return;
    }

    if (e->x + x < 0 || e->x + x >= g->dungeon_floor->width) {
        merror("libgame_handle_player_input_movement_key: e->x+x is out of bounds");
        return;
    }

    if (e->y + y < 0 || e->y + y >= g->dungeon_floor->height) {
        merror("libgame_handle_player_input_movement_key: e->y+y is out of bounds");
        return;
    }

    // whether or not the move is successful, we will do these
    libgame_entity_set_anim(g, id, SPRITEGROUP_ANIM_HUMAN_WALK);
    libgame_entity_update_context(g, id, SPECIFIER_NONE, libgame_get_dir_from_xy(x, y));
    dungeon_floor_remove_at(g->dungeon_floor, id, e->x, e->y);
    dungeon_floor_add_at(g->dungeon_floor, id, e->x + x, e->y + y);

    //if (!dungeon_floor_remove_at(g->dungeon_floor, id, e->x, e->y)) {
    //    merror("libgame_handle_player_input_movement_key: dungeon_floor_remove_at failed");
    //    return;
    //}

    //if (!dungeon_floor_add_at(g->dungeon_floor, id, e->x + x, e->y + y)) {
    //    merror("libgame_handle_player_input_movement_key: dungeon_floor_add_at failed");
    //    return;
    //}

    spritegroup_t* sg = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    sg->move.x = x * DEFAULT_TILE_SIZE;
    sg->move.y = y * DEFAULT_TILE_SIZE;
    e->x = e->x + x;
    e->y = e->y + y;
}



void libgame_handle_input(gamestate* const g) {
    if (!g) {
        merror("libgame_handleinput: gamestate is NULL");
        return;
    }
    //if (IsKeyPressed(KEY_SPACE) || GetTouchPointCount() > 0) {
    //    minfo("key space pressed");
    //    if (g->fadestate == FADESTATENONE) {
    //        g->fadestate = FADESTATEOUT;
    //    }
    //}
    if (IsKeyPressed(KEY_E)) {
        libgame_test_enemy_placement(g);
    }
    if (IsKeyPressed(KEY_C)) {
        g->controlmode = g->controlmode == CONTROLMODE_CAMERA ? CONTROLMODE_PLAYER : CONTROLMODE_CAMERA;
    }
    if (IsKeyPressed(KEY_D)) {
        g->debugpanelon = !g->debugpanelon;
    }
    if (IsKeyPressed(KEY_G)) {
        g->gridon = !g->gridon;
    }
    if (g->controlmode == CONTROLMODE_PLAYER) {
        if (IsKeyPressed(KEY_LEFT)) {
            minfo("KEY_LEFT");
            libgame_handle_move(g, g->hero_id, -1, 0);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_RIGHT)) {
            minfo("KEY_RIGHT");
            libgame_handle_move(g, g->hero_id, 1, 0);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_UP)) {
            minfo("KEY_UP");
            libgame_handle_move(g, g->hero_id, 0, -1);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_DOWN)) {
            minfo("KEY_DOWN");
            libgame_handle_move(g, g->hero_id, 0, 1);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_A)) {
            minfo("KEY_A");
            //libgame_entity_set_anim(g, hero_id, SPRITEGROUP_ANIM_HUMAN_ATTACK);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_PERIOD)) {
            minfo("KEY_PERIOD");
            g->player_input_received = true;
        }
    } else if (g->controlmode == CONTROLMODE_CAMERA) {
        libgame_handle_caminput(g);
    }
}




void libgame_update_spritegroup_dest(gamestate* const g, const entityid id) {
    spritegroup_t* sg = NULL;
    entity_t* e = NULL;
    if (g) {
        sg = hashtable_entityid_spritegroup_get(g->spritegroups, id);
        e = em_get(entitymap, id);
        if (sg && e) {
            sg->dest.x = e->x * DEFAULT_TILE_SIZE + sg->off_x;
            sg->dest.y = e->y * DEFAULT_TILE_SIZE + sg->off_y;
        } else if (!sg) {
            merror("libgame_update_spritegroup_dest: spritegroup is NULL");
        } else if (!e) {
            merror("libgame_update_spritegroup_dest: entity is NULL");
        }
    } else if (!g) {
        merror("libgame_update_spritegroup_dest: gamestate is NULL");
    }
}




//void libgame_update_all_entity_sg_dests(gamestate* const g) {
//    if (!g) {
//        merror("libgame_update_all_entity_sg_dests: gamestate is NULL");
//        return;
//    }
//const int entity_count = libgame_lua_get_num_entities(L);
//for (int i = 1; i <= entity_count; i++) {
//    const entityid id = libgame_lua_get_nth_entity(L, i);
//    libgame_update_spritegroup_dest(g, id);
//}
//}




const int
libgame_entity_update_context(gamestate* const g, const entityid id, const specifier_t spec, const direction_t dir) {
    if (!g) {
        merror("libgame_entity_update_context: gamestate is NULL");
        return -1;
    }
    spritegroup_t* group = hashtable_entityid_spritegroup_get_by_specifier(g->spritegroups, id, spec);
    if (!group) {
        merror("libgame_entity_update_context: group is NULL");
        fprintf(stderr, "id: %d, spec: %d\n", id, spec);
        return -1;
    }
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
    return 0;
}




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




void libgame_handle_caminput(gamestate* const g) {
    if (!g) {
        return;
    }

    const bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    const float zoom_incr = 1.00f;
    const float cam_move_incr = 4.00f;
    if (IsKeyPressed(KEY_Z) && shift && g->cam2d.zoom >= 2.0f) {
        g->cam2d.zoom -= zoom_incr;
    } else if (IsKeyPressed(KEY_Z)) {
        g->cam2d.zoom += zoom_incr;
    }
    if (IsKeyDown(KEY_UP)) {
        g->cam2d.offset.y += cam_move_incr;
    } else if (IsKeyDown(KEY_DOWN)) {
        g->cam2d.offset.y -= cam_move_incr;
    } else if (IsKeyDown(KEY_LEFT)) {
        g->cam2d.offset.x += cam_move_incr;
    } else if (IsKeyDown(KEY_RIGHT)) {
        g->cam2d.offset.x -= cam_move_incr;
    } else if (IsKeyDown(KEY_KP_5)) {
        g->cam2d.offset = (Vector2){0, 0};
    } else if (IsKeyDown(KEY_KP_2)) {
        g->cam2d.offset.y += cam_move_incr;
    } else if (IsKeyDown(KEY_KP_4)) {
        g->cam2d.offset.x -= cam_move_incr;
    } else if (IsKeyDown(KEY_KP_6)) {
        g->cam2d.offset.x += cam_move_incr;
    } else if (IsKeyDown(KEY_KP_8)) {
        g->cam2d.offset.y -= cam_move_incr;
    } else if (IsKeyDown(KEY_KP_0)) {
        g->cam2d.zoom = 2.0f;
    } else if (IsKeyPressed(KEY_R)) {
        g->cam2d.offset = (Vector2){0, 0};
    } else if (IsKeyPressed(KEY_F)) {
        g->cam_lockon = !g->cam_lockon;
    } else if (IsKeyDown(KEY_KP_1)) {
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




bool libgame_windowshouldclose() {
    return WindowShouldClose();
}




void libgame_initwindow(gamestate* const g) {
    if (!g) {
        merror("libgame_initwindow: gamestate is NULL");
        return;
    }
    const char* title = "evildojo666";
    const int window_width = 800;
    const int window_height = 480;
    InitWindow(window_width, window_height, title);
    SetWindowMonitor(1);
    SetWindowPosition(1920 * 2 - 800, 0);
    SetTargetFPS(DEFAULT_TARGET_FPS);
    SetExitKey(KEY_ESCAPE);
    //g->windowwidth = libgame_lua_get_gamestate_int(L, "WindowWidth"),
    g->windowwidth = window_width;
    g->windowheight = window_height;
}



void libgame_closewindow() {
    CloseWindow();
}




void libgame_update_debug_panel_buffer(gamestate* const g) {
    if (!g) {
        merror("libgame_update_debug_panel_buffer: gamestate is NULL");
        return;
    }
    entityid id = g->hero_id;
    entity* hero = em_get(entitymap, id);
    const int entity_count = em_count(entitymap);

    const int camera_zoom = (int)g->cam2d.zoom;

    const int dw = g->dungeon_floor->width;
    const int dh = g->dungeon_floor->height;

    snprintf(g->debugpanel.buffer,
             1024,
             "@evildojo666\n"
             "Frame Count: %d\n"
             "Camera Zoom: %d\n"
             "Dungeon Size: %dx%d\n"
             "Hero.pos: %d, %d\n"
             "EntityCount: %d\n",
             g->framecount,
             camera_zoom,
             dw,
             dh,
             hero->x,
             hero->y,
             entity_count);
}




void libgame_update_smoothmove(gamestate* const g, const entityid id) {
    if (!g) {
        merror("libgame_update_smoothmove: gamestate is NULL");
        return;
    }
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (!group) {
        merror("libgame_update_smoothmove: group is NULL");
        return;
    }
    const float move = 1.0f;
    group->dest.x += group->move.x > 0 ? move : group->move.x < 0 ? -move : 0;
    group->dest.y += group->move.y > 0 ? move : group->move.y < 0 ? -move : 0;
    group->move.x += group->move.x > 0 ? -move : group->move.x < 0 ? move : 0;
    group->move.y += group->move.y > 0 ? -move : group->move.y < 0 ? move : 0;
}




void libgame_do_camera_lock_on(gamestate* const g) {
    if (!g) {
        merror("libgame_do_camera_lock_on: gamestate is NULL");
        return;
    }
    const entityid hero_id = g->hero_id;
    //const entityid hero_id = -1;
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_get(g->spritegroups, hero_id);
    if (hero_group && g->cam_lockon) {
        g->cam2d.target = (Vector2){hero_group->dest.x, hero_group->dest.y};
    }
}




void libgame_reset_entity_anim(gamestate* const g, entityid id) {
    if (!g) {
        merror("libgame_reset_entity_anim: gamestate is NULL");
        return;
    }
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (!sg) {
        merror("libgame_reset_entity_anim: spritegroup is NULL");
        return;
    }
    sprite* s = spritegroup_get(sg, sg->current);
    if (s && s->num_loops >= 1) {
        //sg->prev_anim = sg->current;
        sg->current = sg->default_anim;
        s->num_loops = 0;
    }
}




void libgame_reset_entities_anim(gamestate* const g) {
    if (!g) {
        merror("libgame_reset_entities_anim: gamestate is NULL");
        return;
    }

    for (int i = 0; i < em_count(entitymap); i++) {
        libgame_reset_entity_anim(g, i);
    }
}




void libgame_update_gamestate(gamestate* g) {
    //minfo("libgame_update_gamestate begin");
    if (!g) {
        merror("libgame_update_gamestate: gamestate is NULL");
        return;
    }
    //UpdateMusicStream(test_music);
    libgame_update_debug_panel_buffer(g);
    //setdebugpanelcenter(g);
    libgame_reset_entities_anim(g);
    libgame_do_camera_lock_on(g);
    for (int i = 0; i < em_count(entitymap); i++) {
        entity* e = em_get(entitymap, i);
        if (!e) {
            merror("libgame_update_gamestate: entity is NULL");
            continue;
        }
        libgame_update_smoothmove(g, e->id);
    }
    // at this point, we can take other NPC turns
    // lets iterate over our entities, find the NPCs, and make them move in a random direction
    // then, we will update their smooth moves
    // we will need to eventually disable player input during smooth moving
    // update smooth move for NPCs and other entities
    //libgame_update_smoothmoves_for_entitytype(g, ENTITY_NPC);
    //minfo("test");
    if (g->player_input_received) {
        g->player_input_received = false;
    }
    //minfo("libgame_update_gamestate end");
}




const bool libgame_handle_sprite_update(gamestate* const g,
                                        const entityid actor_id,
                                        const actionresults_t actionresults_type,
                                        const int xdir,
                                        const int ydir) {
    // given the actor_id, grab the spritegroup and set the animation
    minfo("libgame_handle_sprite_update begin");
    printf("actor_id: %d\n", actor_id);
    printf("actionresults_type: %d\n", actionresults_type);
    printf("xdir = %d\n", xdir);
    printf("ydir = %d\n", ydir);
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(g->spritegroups, actor_id);
    if (!sg) {
        merror("libgame_handle_sprite_update: spritegroup is NULL");
        return false;
    }
    //race_t actor_race = libgame_lua_get_entity_int(L, actor_id, "race");
    //int anim = -1;
    //if (actionresults_type == AR_MOVE_SUCCESS) {
    //    anim = actor_race == RACE_HUMAN ? SPRITEGROUP_ANIM_HUMAN_WALK
    //           : actor_race == RACE_ORC ? SPRITEGROUP_ANIM_ORC_WALK
    //                                    : -1;
    //    if (anim == -1) {
    //        merror("Race not set properly");
    //        fprintf(stderr, "Race: %d\n", actor_race);
    //    }
    //    libgame_entity_set_anim(g, actor_id, anim);
    //libgame_entity_anim_enqueue(g, actor_id, anim);
    //libgame_update_spritegroup_move(
    //    g, actor_id, xdir * DEFAULT_TILE_SIZE, ydir * DEFAULT_TILE_SIZE);
    //    libgame_entity_update_context(g, actor_id, SPECIFIER_NONE, libgame_get_dir_from_xy(xdir, ydir));
    //    return true;
    //} else if (actionresults_type == AR_ATTACK_SUCCESS) {
    //    anim = actor_race == RACE_HUMAN ? SPRITEGROUP_ANIM_HUMAN_ATTACK
    //           : actor_race == RACE_ORC ? SPRITEGROUP_ANIM_ORC_ATTACK
    //                                    : -1;
    //    if (anim == -1) {
    //        merror("Race not set properly");
    //        fprintf(stderr, "Race: %d\n", actor_race);
    //    }
    //    libgame_entity_set_anim(g, actor_id, anim);
    //libgame_entity_anim_enqueue(g, actor_id, anim);
    //    libgame_entity_update_context(g, actor_id, SPECIFIER_NONE, libgame_get_dir_from_xy(xdir, ydir));
    //    return true;
    //} else if (actionresults_type == AR_WAS_DAMAGED) {
    //    anim = actor_race == RACE_HUMAN ? SPRITEGROUP_ANIM_HUMAN_DMG
    //           : actor_race == RACE_ORC ? SPRITEGROUP_ANIM_ORC_DMG
    //                                    : -1;
    //    if (anim == -1) {
    //        merror("Race not set properly");
    //        fprintf(stderr, "Race: %d\n", actor_race);
    //    }
    //    libgame_entity_set_anim(g, actor_id, anim);
    //libgame_entity_anim_enqueue(g, actor_id, anim);
    //    libgame_entity_update_context(g, actor_id, SPECIFIER_NONE, libgame_get_dir_from_xy(xdir, ydir));
    //    return true;
    //}
    return false;
}




void libgame_drawframeend(gamestate* const g) {
    if (!g) {
        merror("libgame_drawframeend: gamestate is NULL");
        return;
    }
    EndDrawing();
    // update gamestate values at end of draw frame:
    // framecount, currenttime, currenttimetm, currenttimebuf
    g->framecount++;
    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&(g->currenttime));
    strftime(g->currenttimebuf, 64, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
}




void libgame_drawframe(gamestate* g) {
    if (!g) {
        merror("libgame_drawframe: gamestate is NULL");
        return;
    }
    BeginDrawing();
    BeginTextureMode(target);
    switch (activescene) {
    //case SCENE_COMPANY:
    //    libgame_draw_company_scene(g);
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
    // draw a line from the hero to the last mouse click
    //const int hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
    //const int x = libgame_lua_get_entity_int(L, hero_id, "x");
    //const int y = libgame_lua_get_entity_int(L, hero_id, "y");
    //const Vector2 hero_vec = {x, y};
    //DrawLine(x, y, GetMouseX(), GetMouseY(), RED);
    EndTextureMode();
    DrawTexturePro(target.texture, target_src, target_dest, target_origin, 0.0f, WHITE);
    //if (g->debugpanelon) {
    libgame_draw_debug_panel(g);
    libgame_drawframeend(g);
}




void libgame_calc_debugpanel_size(gamestate* const g) {
    if (!g) {
        merror("libgame_calc_debugpanel_size: gamestate is NULL");
        return;
    }
    const int fontsize = 14;
    const int spacing = 1;
    const Vector2 m = MeasureTextEx(g->font, g->debugpanel.buffer, fontsize, spacing);
    g->debugpanel.w = m.x;
    g->debugpanel.h = m.y;
}




inline void libgame_draw_debug_panel(gamestate* const g) {
    if (!g) {
        merror("libgame_draw_debug_panel: gamestate is NULL");
        return;
    }
    if (!g->debugpanelon) {
        //merror("libgame_draw_debug_panel: debugpanelon is false");
        return;
    }
    const int fontsize = 14;
    const int spacing = 1;
    const int xy = 10;
    const int wh = 20;
    const Color c = {0x33, 0x33, 0x33, 255};
    const Color c2 = WHITE;
    const Vector2 p = {g->debugpanel.x, g->debugpanel.y};
    const Vector2 o = {0, 0};
    const int x0 = g->debugpanel.x - xy;
    const int y0 = g->debugpanel.y - xy;
    const int w0 = g->debugpanel.w + wh;
    const int h0 = g->debugpanel.h + wh;
    const Rectangle box = {x0, y0, w0, h0};
    DrawRectanglePro(box, o, 0.0f, c);
    DrawTextEx(g->font, g->debugpanel.buffer, p, fontsize, spacing, c2);
}




void libgame_drawgrid(gamestate* const g) {
    if (!g) {
        merror("libgame_drawgrid: gamestate is NULL");
        return;
    }
    //const int len = libgame_lua_get_dungeonfloor_row_count(L);
    //const int wid = libgame_lua_get_dungeonfloor_col_count(L);
    //for (int i = 0; i <= len; i++) {
    //    DrawLine(i * DEFAULT_TILE_SIZE, 0, i * DEFAULT_TILE_SIZE, wid * DEFAULT_TILE_SIZE, GREEN);
    //}
    //for (int i = 0; i <= wid; i++) {
    //    DrawLine(0, i * DEFAULT_TILE_SIZE, len * DEFAULT_TILE_SIZE, i * DEFAULT_TILE_SIZE, GREEN);
    //}
}




void libgame_draw_dungeon_floor_tiles(gamestate* const g) {
    for (int i = 0; i < g->dungeon_floor->height; i++) {
        for (int j = 0; j < g->dungeon_floor->width; j++) {
            const dungeon_tile_type_t type = g->dungeon_floor->tiles[i][j].type;
            const int key = get_txkey_for_tiletype(type);
            if (key != -1) {
                tile_dest.x = j * DEFAULT_TILE_SIZE;
                tile_dest.y = i * DEFAULT_TILE_SIZE;
                DrawTexturePro(g->txinfo[key].texture, tile_src, tile_dest, zero_vec, 0, WHITE);
            }
        }
    }
}




void libgame_draw_dungeon_floor_entities(gamestate* const g) {
    for (int i = 0; i < g->dungeon_floor->height; i++) {
        for (int j = 0; j < g->dungeon_floor->width; j++) {
            dungeon_tile_t* tile = &g->dungeon_floor->tiles[i][j];
            if (tile && tile->entity_count > 0) {
                //minfo("tile entity loop");
                for (int k = 0; k < tile->entity_count; k++) {
                    libgame_draw_entity_shadow(g, tile->entities[k]);
                    libgame_draw_entity(g, tile->entities[k]);
                }
            }
        }
    }
}




void libgame_draw_dungeon_floor(gamestate* const g) {
    if (!g) {
        merror("libgame_draw_dungeon_floor: gamestate is NULL");
        return;
    }
    if (g->dungeon_floor->height == -1 || g->dungeon_floor->width == -1) {
        merror("libgame_draw_dungeonfloor: row_count or col_count is -1");
        return;
    }

    // draw dungeon floor tiles
    libgame_draw_dungeon_floor_tiles(g);

    // draw entities per tile
    // next, we want to draw the entities on the tiles
    // at first we will do it generically then we will guarantee ordering
    libgame_draw_dungeon_floor_entities(g);
}




void libgame_draw_entity_shadow(gamestate* const g, const entityid id) {
    if (!g) {
        merror("libgame_draw_entity_shadow: gamestate is NULL");
        return;
    }
    if (id == -1) {
        merror("libgame_draw_entity_shadow: id is -1");
        return;
    }
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (!group) {
        merror("libgame_draw_entity_shadow: group is NULL");
        return;
    }
    // draw entity shadow, which should exist at current+1 if loaded correctly
    DrawTexturePro(*group->sprites[group->current + 1]->texture,
                   group->sprites[group->current + 1]->src,
                   group->dest,
                   (Vector2){0, 0},
                   0.0f,
                   WHITE);
}




void libgame_draw_entity(gamestate* const g, const entityid id) {
    if (!g) {
        merror("libgame_draw_entity: gamestate is NULL");
        return;
    }
    const specifier_t spec = SPECIFIER_NONE;
    spritegroup_t* group = hashtable_entityid_spritegroup_get_by_specifier(g->spritegroups, id, spec);
    if (!group) {
        merror("libgame_draw_entity: group is NULL");
        return;
    }
    // draw entity shadow
    //libgame_draw_entity_shadow(g, id);
    // draw the main entity sprite
    DrawTexturePro(*group->sprites[group->current]->texture,
                   group->sprites[group->current]->src,
                   group->dest,
                   (Vector2){0, 0},
                   0.0f,
                   WHITE);

    if (g->framecount % FRAMEINTERVAL == 0) {
        sprite_incrframe(group->sprites[group->current]);
        sprite_incrframe(group->sprites[group->current + 1]);
    }

    //libgame_draw_entity_incr_frame(g, id);

    if (g->debugpanelon) {
        const int x = group->dest.x;
        const int y = group->dest.y;
        const int w = group->dest.width;
        const int h = group->dest.height;
        // first draw the outer rectangle without the offset
        Vector2 v[4] = {{x, y}, {x + w, y}, {x + w, y + h}, {x, y + h}};
        DrawLineV(v[0], v[1], (Color){0, 0, 255, 255});
        DrawLineV(v[1], v[2], (Color){0, 0, 255, 255});
        DrawLineV(v[2], v[3], (Color){0, 0, 255, 255});
        DrawLineV(v[3], v[0], (Color){0, 0, 255, 255});
        // now lets draw it with the offset
        v[0].x = x - group->off_x;
        v[0].y = y - group->off_y;
        v[1].x = x + group->off_x + w;
        v[1].y = y - group->off_y;
        v[2].x = x + w + group->off_x;
        v[2].y = y + h + group->off_y;
        v[3].x = x - group->off_x;
        v[3].y = y + h + group->off_y;
        DrawLineV(v[0], v[1], (Color){0, 255, 0, 255});
        DrawLineV(v[1], v[2], (Color){0, 255, 0, 255});
        DrawLineV(v[2], v[3], (Color){0, 255, 0, 255});
        DrawLineV(v[3], v[0], (Color){0, 255, 0, 255});
    }
}




void libgame_draw_gameplayscene(gamestate* const g) {
    if (!g) {
        merror("libgame_draw_gameplayscene: gamestate is NULL");
        return;
    }
    BeginMode2D(g->cam2d);
    ClearBackground(BLACK);
    libgame_draw_dungeon_floor(g);
    libgame_handle_fade(g);
    EndMode2D();
}




void libgame_draw_title_scene(gamestate* const g) {
    if (!g) {
        merror("libgame_draw_title_scene: gamestate is NULL");
        return;
    }
    const Color bgc = {0x66, 0x66, 0x66, 255}, fgc = WHITE, fgc2 = BLACK;
    char b1[128 * 3];
    char* b2 = b1 + 128;
    char* b3 = b1 + 128 * 2;
    snprintf(b1, 128, "project");
    snprintf(b2, 128, "rpg");
    snprintf(b3, 128, "press space to continue");
    const Vector2 m = MeasureTextEx(g->font, b1, 40, 2);
    const Vector2 m2 = MeasureTextEx(g->font, b2, 40, 2);
    const Vector2 m3 = MeasureTextEx(g->font, b3, 16, 1);
    const float tw2 = g->targetwidth / 2.0f;
    const float th2 = g->targetheight / 2.0f;
    const int offset = 100;
    const int x1 = tw2 - m.x / 2.0f - offset;
    const int y1 = th2 - m.y / 2.0f;
    const int x2 = tw2 - m2.x / 2.0f + offset;
    const int x3 = tw2 - m3.x / 2.0f;
    const int y3 = th2 + m3.y / 2.0f + 20;
    const Vector2 pos[3] = {{x1, y1}, {x2, y1}, {x3, y3}};
    ClearBackground(bgc);
    DrawTextEx(g->font, b1, pos[0], 40, 4, fgc);
    DrawTextEx(g->font, b2, pos[1], 40, 1, fgc2);
    // just below the 'project rpg' text
    DrawTextEx(g->font, b3, pos[2], 16, 1, fgc);
    libgame_handle_fade(g);
}




void libgame_draw_company_scene(gamestate* const g) {
    if (!g) {
        merror("libgame_draw_company_scene: gamestate is NULL");
        return;
    }
    const Color bgc = BLACK;
    const Color fgc = {0x66, 0x66, 0x66, 255};
    const int fontsize = 32;
    const int spacing = 1;
    char b[128 * 3];
    bzero(b, 128 * 3);
    char* b2 = b + 128;
    char* b3 = b + 128 * 2;
    snprintf(b, 128, COMPANYNAME);
    snprintf(b2, 128, COMPANYFILL);
    snprintf(b3, 128, "presents");
    const Vector2 measure = MeasureTextEx(g->font, b, fontsize, spacing);
    const Vector2 pos = {g->targetwidth / 2.0f - measure.x / 2.0f, g->targetheight / 2.0f - measure.y / 2.0f};
    ClearBackground(bgc);
    DrawTextEx(g->font, b, pos, fontsize, 1, fgc);
    DrawTextEx(g->font, b2, pos, fontsize, 1, fgc);
    const Vector2 measure3 = MeasureTextEx(g->font, b3, 20, 1);
    const Vector2 pp = {g->targetwidth / 2.0f - measure3.x / 2.0f, g->targetheight / 2.0f + measure.y / 2.0f + 20};
    DrawTextEx(g->font, b3, pp, 20, 1, fgc);
    libgame_handle_fade(g);
}




void libgame_load_texture_from_disk(
    gamestate* const g, const int index, const int contexts, const int frames, const bool dodither, const char* path) {
    if (!g) {
        merror("libgame_loadtexture: gamestate is null");
        return;
    }
    if (dodither) {
        Image img = LoadImage(path);
        ImageDither(&img, 4, 4, 4, 4);
        Texture2D t = LoadTextureFromImage(img);
        g->txinfo[index].texture = t;
        UnloadImage(img);
    } else {
        g->txinfo[index].texture = LoadTexture(path);
    }
    g->txinfo[index].num_frames = frames;
    g->txinfo[index].contexts = contexts;
    msuccess("libgame_loadtexture");
    //SetTextureFilter(textures[index], TEXTURE_FILTER_POINT);
    //SetTextureFilter(txinfo[index].texture, TEXTURE_FILTER_POINT);
}




void libgame_load_textures_from_disk(gamestate* const g) {
    if (!g) {
        merror("libgame_loadtextures_from_disk: gamestate is null");
        return;
    }
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
    if (!g) {
        merror("libgame_loadtextures: gamestate is null");
        return;
    }
    libgame_load_textures_from_disk(g);
}



void libgame_unloadtexture(gamestate* const g, const int index) {
    if (!g) {
        merror("libgame_unloadtexture: gamestate is null");
        return;
    }
    if (g->txinfo[index].texture.id > 0) {
        UnloadTexture(g->txinfo[index].texture);
    }
}




void libgame_unloadtextures(gamestate* const g) {
    if (!g) {
        merror("libgame_unloadtextures: gamestate is null");
        return;
    }
    //minfo("unloading textures");
    // this can be done smarter, surely...
    for (int i = TX_DIRT_00; i <= TX_BUCKLER; i++) {
        libgame_unloadtexture(g, i);
    }
}




void libgame_init() {
    g = gamestateinitptr();
    libgame_initsharedsetup(g);
    msuccess("libgame_init");
}




const int libgame_create_spritegroup_by_id(gamestate* const g, const entityid id, const direction_t dir) {
    if (!g) {
        merror("libgame_create_spritegroup_by_id: gamestate");
        return -1;
    } else if (id < 0) {
        merror("libgame_create_spritegroup_by_id: id");
        return -1;
    }
    entity* e = em_get(entitymap, id);
    if (!e) {
        merror("libgame_create_spritegroup_by_id: entity is NULL");
        return -1;
    }
    const entitytype_t type = e->type;
    if (type == ENTITY_PLAYER) {
        libgame_create_spritegroup(g, id, TX_HUMAN_KEYS, TX_HUMAN_KEY_COUNT, -12, -12, SPECIFIER_NONE);
        libgame_set_default_anim_for_id(g, id, SPRITEGROUP_ANIM_HUMAN_IDLE);
    }
    return 0;
}




void libgame_set_default_anim_for_id(gamestate* const g, const entityid id, const int anim) {
    if (!g) {
        merror("libgame_set_default_anim_for_id: gamestate is NULL");
        return;
    }
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (!group) {
        merror("libgame_set_default_anim_for_id: group is NULL");
        return;
    }
    group->default_anim = anim;
}




void libgame_create_spritegroup(gamestate* const g,
                                const entityid id,
                                int* keys,
                                const int num_keys,
                                const int offset_x,
                                const int offset_y,
                                const specifier_t spec) {
    if (!g) {
        merror("libgame_create_spritegroup: gamestate is NULL");
        return;
    }
    spritegroup_t* group = spritegroup_create(SPRITEGROUP_DEFAULT_SIZE);
    if (!group) {
        merror("libgame_create_spritegroup: could not create group");
        return;
    }
    entity* e = em_get(entitymap, id);
    if (!e) {
        merror("libgame_create_spritegroup: entity is NULL");
        return;
    }
    const int dw = g->dungeon_floor->width;
    const int dh = g->dungeon_floor->height;
    if (e->x < 0) {
        merror("libgame_create_spritegroup: x is less than 0");
        return;
    }
    if (e->x >= dw) {
        merror("libgame_create_spritegroup: x is greater than or equal to dw");
        return;
    }
    if (e->y < 0) {
        merror("libgame_create_spritegroup: y is less than 0");
        return;
    }
    if (e->y >= dh) {
        merror("libgame_create_spritegroup: y is greater than or equal to dh");
        return;
    }
    for (int i = 0; i < num_keys; i++) {
        spritegroup_add(
            group,
            sprite_create(&g->txinfo[keys[i]].texture, g->txinfo[keys[i]].contexts, g->txinfo[keys[i]].num_frames));
    }
    // this is effectively how we will update the
    // sprite position in relation to the entity's
    // dungeon position
    const float w = spritegroup_get(group, 0)->width;
    const float h = spritegroup_get(group, 0)->height;
    const float dst_x = e->x * DEFAULT_TILE_SIZE;
    const float dst_y = e->y * DEFAULT_TILE_SIZE;
    group->current = 0;
    group->dest = (Rectangle){dst_x + offset_x, dst_y + offset_y, w, h};
    group->off_x = offset_x;
    group->off_y = offset_y;
    spritegroup_set_specifier(group, spec);
    // add the spritegroup to the hashtable
    hashtable_entityid_spritegroup_insert(g->spritegroups, id, group);
}




void libgame_loadtargettexture(gamestate* const g) {
    if (!g) {
        merror("libgame_loadtargettexture: gamestate is NULL");
        return;
    }
    target = LoadRenderTexture(g->targetwidth, g->targetheight);
    target_src = (Rectangle){0, 0, g->targetwidth, -g->targetheight},
    target_dest = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};
    //SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    // update the gamestate display values
    g->cam2d.offset.x = g->targetwidth / 2.0f; //+ c_offset_x;
    g->cam2d.offset.y = g->targetheight / 4.0f; //+ c_offset_y;
}




void libgame_loadfont(gamestate* const g) {
    if (!g) {
        merror("libgame_loadfont: gamestate is NULL");
        return;
    }
    g->font = LoadFontEx(DEFAULT_FONT_PATH, 60, 0, 255);
}




const entityid libgame_create_entity(gamestate* const g, const char* name) {
    entityid id = -1;
    if (!g) {
        merror("libgame_create_entity: gamestate is NULL");
        return -1;
    } else if (!name) {
        merror("libgame_create_entity: name is NULL");
        return -1;
    } else if (strlen(name) == 0) {
        merror("libgame_create_entity: name is empty");
        return -1;
    }
    return id;
}




const entityid libgame_create_orc(gamestate* const g, const char* name, const int x, const int y) {
    if (!g) {
        merror("libgame_create_orc: gamestate is NULL");
        return -1;
    }
    if (!name) {
        merror("libgame_create_orc: name is NULL");
        return -1;
    }
    if (x < 0) {
        merror("libgame_create_orc: x is less than 0");
        return -1;
    }
    if (y < 0) {
        merror("libgame_create_orc: y is less than 0");
        return -1;
    }
    return -1;
}




void libgame_init_entityids(gamestate* const g) {
    entityids = (entityid*)malloc(sizeof(entityid) * EM_MAX_SLOTS);
    if (!entityids) {
        merror("libgame_initsharedsetup: could not allocate entityids");
        return;
    }
    memset(entityids, -1, sizeof(entityid) * EM_MAX_SLOTS);
}




void libgame_initsharedsetup(gamestate* const g) {
    if (!g) {
        merror("libgame_initsharedsetup: gamestate is NULL");
        return;
    }
    libgame_initwindow(g);
    //InitAudioDevice();
    //SetAudioStreamBufferSizeDefault(2048);
    //test_music = LoadMusicStream("./evildojo.mp3");
    //PlayMusicStream(test_music);
    SetRandomSeed(time(NULL));
    g->font = LoadFontEx(DEFAULT_FONT_PATH, 60, 0, 255);
    g->targetwidth = 1280;
    g->targetheight = 720;
    target = LoadRenderTexture(g->targetwidth, g->targetheight);
    target_src = (Rectangle){0, 0, g->targetwidth, -g->targetheight};
    target_dest = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    // update the gamestate display values
    // can get this from init.lua....
    const int c_offset_x = -100;
    g->cam2d.offset.x = g->targetwidth / 2.0f + c_offset_x;
    g->cam2d.offset.y = g->targetheight / 4.0f;
    libgame_load_textures(g);
    g->spritegroups = hashtable_entityid_spritegroup_create(DEFAULT_HASHTABLE_ENTITYID_SPRITEGROUP_SIZE);

    // init the entitymap
    entitymap = em_new();
    // init the entities array
    libgame_init_entityids(g);
    g->dungeon_floor = dungeon_floor_create(20, 20);
    dungeon_floor_init(g->dungeon_floor);


    // ----------
    // begin create hero
    // ----------
    entity* hero = entity_new_at(next_entity_id++, ENTITY_PLAYER, 2, 2, "hero");
    em_add(entitymap, hero);
    g->hero_id = hero->id;
    // whenever we create a new entity and want it on the visible dungeon floor
    // we have to add it to the tile at its x,y position
    const bool res = dungeon_floor_add_at(g->dungeon_floor, hero->id, hero->x, hero->y);
    if (!res) {
        merror("libgame_initsharedsetup: could not add hero to dungeon floor");
    }
    // we ALSO have to create the spritegroup for the entity
    libgame_create_spritegroup(g, hero->id, TX_HUMAN_KEYS, TX_HUMAN_KEY_COUNT, -12, -12, SPECIFIER_NONE);
    libgame_set_default_anim_for_id(g, hero->id, SPRITEGROUP_ANIM_HUMAN_IDLE);
    // ----------
    // end create hero
    // ----------

    // these dont work right until the text buffer of the debugpanel is filled
    libgame_update_debug_panel_buffer(g);
    libgame_calc_debugpanel_size(g);
    //setdebugpanelbottomleft(g);
    setdebugpaneltopleft(g);
}




void libgame_initwithstate(gamestate* const state) {
    if (state == NULL) {
        merror("libgame_initwithstate: gamestate is NULL");
        return;
    }
    g = state;
    libgame_initsharedsetup(g);
}




void libgame_closesavegamestate() {
    libgame_closeshared(g);
}




void libgame_close(gamestate* g) {
    if (!g) {
        merror("libgame_close: gamestate is NULL");
        return;
    }
    libgame_closeshared(g);

    gamestatefree(g);

    // free the entityids array
    free(entityids);
    // free the entitymap
    em_free(entitymap);
    // free the dungeon floor
    //dungeon_floor_free(&dungeon_floor);
}




void libgame_closeshared(gamestate* const g) {
    if (!g) {
        merror("libgame_closeshared: gamestate is NULL");
        return;
    }
    // dont need to free most of gamestate
    minfo("libgame_closeshared");
    //UnloadMusicStream(test_music);
    //CloseAudioDevice();
    UnloadFont(g->font);
    libgame_unloadtextures(g);
    UnloadRenderTexture(target);
    CloseWindow();
    msuccess("libgame_closeshared end");
}




gamestate* libgame_getgamestate() {
    return g;
}




const bool libgame_external_check_reload() {
    return IsKeyPressed(KEY_R);
}
