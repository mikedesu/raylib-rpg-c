#include "libgame.h"
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
#include "race.h"
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
Rectangle tile_src = {0, 0, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE};
const Vector2 zero_vec = {0, 0};
const Vector2 target_origin = {0, 0};

gamestate* g = NULL;
RenderTexture target;
Rectangle target_src = {0, 0, 0, 0};
Rectangle target_dest = {0, 0, 0, 0};
Rectangle tile_dest = {0, 0, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE};
int activescene = GAMEPLAYSCENE;
entityid next_entity_id = 0;


void libgame_draw_fade(const gamestate* const g) {
    if (g && g->fadealpha > 0) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, g->fadealpha});
    }
}

//void libgame_draw_fade(const gamestate* const g) {
//    if (!g) {
//        merror("libgame_draw_fade: gamestate is NULL");
//        return;
//    }
//    if (g->fadealpha > 0) {
//        DrawRectangle(0,
//                      0,
//                      GetScreenWidth(),
//                      GetScreenHeight(),
//                      (Color){0, 0, 0, g->fadealpha});
//    }
//}


void libgame_handle_fade(gamestate* const g) {
    if (g) {
        libgame_handle_fade_unsafe(g);
    } else {
        merror("libgame_handle_fade: gamestate is NULL");
    }
}


void libgame_handle_fade_unsafe(gamestate* const g) {
    const int speed = 4;
    // modify the fadealpha
    int a = g->fadealpha;
    fadestate_t fs = g->fadestate;
    g->fadealpha += fs == FADESTATEOUT && a < 255 ? speed : fs == FADESTATEIN && a > 0 ? -speed : 0;
    // have to update a
    a = g->fadealpha;
    // handle scene rotation based on fadealpha
    bool fmax = a >= 255;
    bool fmin = a <= 0;
    g->fadealpha = fmax ? 255 : fmin ? 0 : a;
    g->fadestate = fmax ? FADESTATEIN : fmin ? FADESTATENONE : fs;
    if (a >= 255) {
        activescene = activescene + 1 > 2 ? 0 : activescene + 1;
    }
    libgame_draw_fade(g);
}


const bool libgame_entity_set_anim(gamestate* const g, const entityid id, const int index) {
    bool retval = false;
    if (g) {
        retval = spritegroup_set_current(hashtable_entityid_spritegroup_get(g->spritegroups, id), index);
    } else {
        merror("libgame_entity_set_anim: gamestate is NULL");
    }
    return retval;
}


const bool libgame_entity_set_anim_unsafe(gamestate* const g, const entityid id, const int index) {
    minfo("libgame_entity_set_anim_unsafe");
    return spritegroup_set_current(hashtable_entityid_spritegroup_get(g->spritegroups, id), index);
}


const int libgame_get_x_from_dir(const direction_t dir) {
    const bool r = dir == DIRECTION_RIGHT;
    const bool dr = dir == DIRECTION_DOWN_RIGHT;
    const bool ur = dir == DIRECTION_UP_RIGHT;
    const bool l = dir == DIRECTION_LEFT;
    const bool dl = dir == DIRECTION_DOWN_LEFT;
    const bool ul = dir == DIRECTION_UP_LEFT;
    return r || dr || ur ? 1 : l || dl || ul ? -1 : 0;
}


const int libgame_get_y_from_dir(const direction_t dir) {
    const bool d = dir == DIRECTION_DOWN;
    const bool dr = dir == DIRECTION_DOWN_RIGHT;
    const bool dl = dir == DIRECTION_DOWN_LEFT;
    const bool u = dir == DIRECTION_UP;
    const bool ur = dir == DIRECTION_UP_RIGHT;
    const bool ul = dir == DIRECTION_UP_LEFT;
    return d || dr || dl ? 1 : u || ur || ul ? -1 : 0;
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


const direction_t libgame_get_dir_from_xy(const int x, const int y) {
    return x == 1 && y == 0     ? DIRECTION_RIGHT
           : x == -1 && y == 0  ? DIRECTION_LEFT
           : x == 0 && y == 1   ? DIRECTION_DOWN
           : x == 0 && y == -1  ? DIRECTION_UP
           : x == 1 && y == 1   ? DIRECTION_DOWN_RIGHT
           : x == -1 && y == 1  ? DIRECTION_DOWN_LEFT
           : x == 1 && y == -1  ? DIRECTION_UP_RIGHT
           : x == -1 && y == -1 ? DIRECTION_UP_LEFT
                                : DIRECTION_NONE;
}


void libgame_handle_move_unsafe(gamestate* const g, const entityid id, const int x, const int y) {
    entity_t* e = em_get(g->entitymap, id);
    if (e) {
        if (e->x < 0 || e->x >= g->dungeon_floor->width) {
            merror("libgame_handle_player_input_movement_key: e->x is out of "
                   "bounds");
        } else if (e->y < 0 || e->y >= g->dungeon_floor->height) {
            merror("libgame_handle_player_input_movement_key: e->y is out of "
                   "bounds");
        } else if (e->x + x < 0 || e->x + x >= g->dungeon_floor->width) {
            merror("libgame_handle_player_input_movement_key: e->x+x is "
                   "out of "
                   "bounds");
        } else if (e->y + y < 0 || e->y + y >= g->dungeon_floor->height) {
            merror("libgame_handle_player_input_movement_key: e->y+y is "
                   "out of "
                   "bounds");
        } else {
            // whether or not the move is successful, we will do these
            libgame_entity_set_anim_unsafe(g, id, SPRITEGROUP_ANIM_HUMAN_WALK);
            //libgame_entity_set_anim(g, id, SPRITEGROUP_ANIM_HUMAN_WALK);
            libgame_entity_update_context_unsafe(g, id, SPECIFIER_NONE, libgame_get_dir_from_xy(x, y));
            // we will need to examine if there is an entity at the
            // destination
            // first we have to check if the tile exists
            // if it is outside the dungeon bounds it will return NULL
            dungeon_tile_t* dest_tile = dungeon_floor_tile_at(g->dungeon_floor, e->x + x, e->y + y);
            if (dest_tile) {
                // we have to check the tile type
                // we cannot move into things like walls etc
                dungeon_tile_type_t dest_type = dest_tile->type;
                bool can_move_to_tile = false;
                can_move_to_tile = dest_type != DUNGEON_TILE_TYPE_STONE_WALL_00 &&
                                   dest_type != DUNGEON_TILE_TYPE_STONE_WALL_01 &&
                                   dest_type != DUNGEON_TILE_TYPE_STONE_WALL_02;
                if (can_move_to_tile) {
                    const size_t current_count = dungeon_tile_entity_count(dest_tile);
                    // this is very basic to start with
                    // eventually we will decide to move based on
                    // the type of entities at the destination
                    if (current_count == 0) {
                        // at this point, we've confirmed the move and are
                        // ready to move the entity
                        // first, we have to remove the entity from the tile
                        // temporarily
                        dungeon_floor_remove_at(g->dungeon_floor, id, e->x, e->y);
                        // next, we
                        // actually update the real entity's x/y
                        e->x = e->x + x;
                        e->y = e->y + y;
                        // we re-add the entity at its new location
                        dungeon_floor_add_at(g->dungeon_floor, id, e->x, e->y);
                        spritegroup_t* sg = hashtable_entityid_spritegroup_get(g->spritegroups, id);
                        // we SHOULD have a spritegroup but still need to check
                        if (sg) {
                            sg->move.x = x * DEFAULT_TILE_SIZE;
                            sg->move.y = y * DEFAULT_TILE_SIZE;
                        } else {
                            merror("libgame_handle_player_input_movement_key: "
                                   "spritegroup is NULL");
                        }
                    } else {
                        minfo("entity at location");
                    }
                }
            } else {
                merror("libgame_handle_player_input_movement_key: dest_tile "
                       "is NULL");
            }
        }
    } else {
        merror("libgame_handle_player_input_movement_key: e is NULL");
    }
}


void libgame_handle_move(gamestate* const g, const entityid id, const int x, const int y) {
    if (g) {
        libgame_handle_move_unsafe(g, id, x, y);
    } else {
        merror("libgame_handle_move: gamestate is NULL");
    }
}


void libgame_handle_input(gamestate* const g) {
    if (g) {
        //if (IsKeyPressed(KEY_SPACE) || GetTouchPointCount() > 0) {
        //    minfo("key space pressed");
        //    if (g->fadestate == FADESTATENONE) {
        //        g->fadestate = FADESTATEOUT;
        //    }
        //}
        //if (IsKeyPressed(KEY_E)) {
        //    libgame_test_enemy_placement(g);
        //}
        // flip between player and camera control
        if (IsKeyPressed(KEY_F3)) {
            if (g->controlmode == CONTROLMODE_PLAYER) {
                g->controlmode = CONTROLMODE_CAMERA;
            } else if (g->controlmode == CONTROLMODE_CAMERA) {
                g->controlmode = CONTROLMODE_PLAYER;
            }
        }
        if (IsKeyPressed(KEY_F2)) {
            g->debugpanelon = !g->debugpanelon;
        }
        if (IsKeyPressed(KEY_G)) {
            g->gridon = !g->gridon;
        }
        if (IsKeyPressed(KEY_W)) {
            g->is3d = !g->is3d;
        }
        if (g->controlmode == CONTROLMODE_PLAYER) {
            libgame_handle_input_player_unsafe(g);
        } else if (g->controlmode == CONTROLMODE_CAMERA) {
            libgame_handle_caminput_unsafe(g);
        }
    } else {
        merror("libgame_handleinput: gamestate is NULL");
    }
}


void libgame_handle_input_player_unsafe(gamestate* const g) {
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_KP_4)) {
        libgame_handle_move_unsafe(g, g->hero_id, -1, 0);
        g->player_input_received = true;
    } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_KP_6)) {
        libgame_handle_move_unsafe(g, g->hero_id, 1, 0);
        g->player_input_received = true;
    } else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_KP_8)) {
        libgame_handle_move_unsafe(g, g->hero_id, 0, -1);
        g->player_input_received = true;
    } else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_KP_2)) {
        libgame_handle_move_unsafe(g, g->hero_id, 0, 1);
        g->player_input_received = true;
    } else if (IsKeyPressed(KEY_KP_3)) {
        libgame_handle_move_unsafe(g, g->hero_id, 1, 1);
        g->player_input_received = true;
    } else if (IsKeyPressed(KEY_KP_1)) {
        libgame_handle_move_unsafe(g, g->hero_id, -1, 1);
        g->player_input_received = true;
    } else if (IsKeyPressed(KEY_KP_7)) {
        libgame_handle_move_unsafe(g, g->hero_id, -1, -1);
        g->player_input_received = true;
    } else if (IsKeyPressed(KEY_KP_9)) {
        libgame_handle_move_unsafe(g, g->hero_id, 1, -1);
        g->player_input_received = true;
    } else if (IsKeyPressed(KEY_PERIOD)) {
        g->player_input_received = true;
    } else if (IsKeyPressed(KEY_A)) {
        libgame_handle_entity_attack_unsafe(g, g->hero_id);
        g->player_input_received = true;
    }
}


void libgame_handle_entity_attack_unsafe(gamestate* const g, const entityid id) {
    if (g) {
        // ok time to work on the logic of attacking
        // we need to get the direction of the entity
        // we need to get the entity

        entity_t* e = em_get(g->entitymap, id);

        if (e) {
            // here, we need to handle the logic of attacking, and then set the animation appropriately
            // we need to know the direction the entity is currently facing
            // this will need to be set when moving etc

            // lets first get e's direction to see which dungeon tile we need to check
            const direction_t dir = e->direction;
            const int x = libgame_get_x_from_dir(dir);
            const int y = libgame_get_y_from_dir(dir);
            minfoint("libgame_handle_entity_attack: x", x);
            minfoint("libgame_handle_entity_attack: y", y);
            // we need to get the tile in front of the entity
            dungeon_tile_t* dest_tile = dungeon_floor_tile_at(g->dungeon_floor, e->x + x, e->y + y);
            // we need to check if there is an entity at the destination
            if (dest_tile) {
                const size_t current_count = dungeon_tile_entity_count(dest_tile);
                if (current_count > 0) {
                    // we need to get the entities at the destination
                    entityid* ids = dest_tile->entities;
                    if (ids) {
                        for (int i = 0; i < current_count; i++) {
                            entityid current_id = ids[i];
                            entity_t* current_entity = em_get(g->entitymap, current_id);
                            if (current_entity) {
                                // check the entity type
                                // if it is an NPC, then it can receive an attack
                                // eventually we will handle attacking items etc

                                minfoint("libgame_handle_entity_attack: current_entity->id", current_entity->id);

                                if (current_entity->type == ENTITY_NPC) {
                                    // we will need to handle the attack
                                    // we will need to get

                                    minfo("libgame_handle_entity_attack: attack reaches NPC");

                                    int anim = SPRITEGROUP_ANIM_HUMAN_DMG;

                                    if (current_entity->race == RACE_HUMAN) {
                                        minfoint("race is", e->race);
                                        anim = SPRITEGROUP_ANIM_HUMAN_DMG;
                                    } else if (current_entity->race == RACE_ORC) {
                                        minfoint("race is", e->race);

                                        anim = SPRITEGROUP_ANIM_ORC_DMG;
                                    }

                                    minfoint("libgame_handle_entity_attack: setting anim to", anim);

                                    // set the damage animation for the entity that is attacked
                                    libgame_entity_set_anim_unsafe(g, current_id, anim);
                                }
                            }
                        }
                    } else {
                        merror("CRITICAL: libgame_handle_entity_attack: ids is NULL but current_count > 0");
                    }
                } else {
                    minfo("libgame_handle_entity_attack: no entities at destination");
                }
            } else {
                minfo("libgame_handle_entity_attack: dest_tile is NULL");
            }


            // the below code handles the setting of the attack animation
            // assuming the attack is successful against another entity, we will also want to
            // set the damage animation for the entity that is attacked
            // we need to get the spritegroup
            int anim = SPRITEGROUP_ANIM_HUMAN_ATTACK;
            race_t race = e->race;
            if (race == RACE_HUMAN) {
                anim = SPRITEGROUP_ANIM_HUMAN_ATTACK;
            } else if (race == RACE_ORC) {
                anim = SPRITEGROUP_ANIM_ORC_ATTACK;
            }

            libgame_entity_set_anim_unsafe(g, id, anim);
        } else {
            merror("libgame_handle_entity_attack: entity is NULL");
        }
    } else {
        merror("libgame_handle_entity_attack: gamestate is NULL");
    }
}


const int libgame_entity_update_context_unsafe(gamestate* const g,
                                               const entityid id,
                                               const specifier_t spec,
                                               const direction_t dir) {
    int retval = 0;
    spritegroup_t* group = hashtable_entityid_spritegroup_get_by_specifier(g->spritegroups, id, spec);
    if (group) {
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
        retval = 0;
    } else {
        merror("libgame_entity_update_context: group is NULL");
        retval = -1;
    }
    return retval;
}


const int
libgame_entity_update_context(gamestate* const g, const entityid id, const specifier_t spec, const direction_t dir) {
    int retval = 0;
    if (g) {
        retval = libgame_entity_update_context_unsafe(g, id, spec, dir);
    } else {
        merror("libgame_entity_update_context: gamestate is NULL");
        retval = -1;
    }
    return retval;
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


void libgame_handle_caminput_zoom(gamestate* const g) {
    const bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    const float zoom_incr = 1.00f;
    if (IsKeyPressed(KEY_Z) && shift && g->cam2d.zoom >= 2.0f) {
        g->cam2d.zoom -= zoom_incr;
    } else if (IsKeyPressed(KEY_Z)) {
        g->cam2d.zoom += zoom_incr;
    }
}

void libgame_handle_caminput(gamestate* const g) {
    if (g) {
        libgame_handle_caminput_unsafe(g);
    } else {
        merror("libgame_handle_caminput: gamestate is NULL");
    }
}

void libgame_handle_caminput_unsafe(gamestate* const g) {
    //if (g) {
    const float move = 4.00f;
    libgame_handle_caminput_zoom(g);
    if (IsKeyDown(KEY_UP)) {
        g->cam2d.offset.y += move;
    } else if (IsKeyDown(KEY_DOWN)) {
        g->cam2d.offset.y -= move;
    } else if (IsKeyDown(KEY_LEFT)) {
        g->cam2d.offset.x += move;
    } else if (IsKeyDown(KEY_RIGHT)) {
        g->cam2d.offset.x -= move;
    } else if (IsKeyDown(KEY_KP_5)) {
        g->cam2d.offset = zero_vec;
    } else if (IsKeyDown(KEY_KP_2)) {
        g->cam2d.offset.y += move;
    } else if (IsKeyDown(KEY_KP_4)) {
        g->cam2d.offset.x -= move;
    } else if (IsKeyDown(KEY_KP_6)) {
        g->cam2d.offset.x += move;
    } else if (IsKeyDown(KEY_KP_8)) {
        g->cam2d.offset.y -= move;
    } else if (IsKeyDown(KEY_KP_0)) {
        // reset zoom
        g->cam2d.zoom = 2.0f;
    } else if (IsKeyPressed(KEY_R)) {
        g->cam2d.offset = zero_vec;
    } else if (IsKeyPressed(KEY_F)) {
        g->cam_lockon = !g->cam_lockon;
    } else if (IsKeyDown(KEY_KP_1)) {
        g->cam2d.offset.x -= move;
        g->cam2d.offset.y += move;
    } else if (IsKeyDown(KEY_KP_3)) {
        g->cam2d.offset.x += move;
        g->cam2d.offset.y += move;
    } else if (IsKeyDown(KEY_KP_7)) {
        g->cam2d.offset.x -= move;
        g->cam2d.offset.y -= move;
    } else if (IsKeyDown(KEY_KP_9)) {
        g->cam2d.offset.x += move;
        g->cam2d.offset.y -= move;
    }
    //}
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
    //const int window_width = 1080;
    //const int window_height = 1920 / 2;
    const int x = 0;
    //const int x = 1080;
    const int y = 0;
    InitWindow(window_width, window_height, title);
    SetWindowMonitor(1);
    //const int x = 1920 * 2 - 800;
    SetWindowPosition(x, y);
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
    entity* hero = em_get(g->entitymap, id);
    int entity_count = 0;
    char* hero_name = "none";
    int hx = -1;
    int hy = -1;
    if (hero) {
        entity_count = em_count(g->entitymap);
        hero_name = hero->name;
        hx = hero->x;
        hy = hero->y;
    }
    const int camera_zoom = (int)g->cam2d.zoom;
    const int dw = g->dungeon_floor->width;
    const int dh = g->dungeon_floor->height;
    const bool is3d = g->is3d;
    const int cam2dx = (int)g->cam2d.target.x;
    const int cam2dy = (int)g->cam2d.target.y;
    const int cam2dox = (int)g->cam2d.offset.x;
    const int cam2doy = (int)g->cam2d.offset.y;
    const float cam3dx = g->cam3d.position.x;
    const float cam3dy = g->cam3d.position.y;
    const float cam3dz = g->cam3d.position.z;
    const float cam3dtx = g->cam3d.target.x;
    const float cam3dty = g->cam3d.target.y;
    const float cam3dtz = g->cam3d.target.z;
    const controlmode_t controlmode = g->controlmode;

    snprintf(g->debugpanel.buffer,
             1024,
             "@evildojo666\n"
             "is3d: %d\n"
             "Frame Count: %d\n"
             "Camera2D Position Offset & Zoom: %03d,%03d %03d,%03d %03d\n"
             "Camera3D Position: %03.2f,%03.2f,%03.2f\n"
             "Camera3D Target:   %03.2f,%03.2f,%03.2f\n"
             "Dungeon Size: %dx%d\n"
             "Hero.name: %s\n"
             "Hero.pos: %d, %d\n"
             "EntityCount: %d\n"
             "ControlMode: %d\n",
             is3d,
             g->framecount,
             cam2dx,
             cam2dy,
             cam2dox,
             cam2doy,
             camera_zoom,
             cam3dx,
             cam3dy,
             cam3dz,
             cam3dtx,
             cam3dty,
             cam3dtz,
             dw,
             dh,
             hero_name,
             hx,
             hy,
             entity_count,
             controlmode);
}


void libgame_update_smoothmove(gamestate* const g, const entityid id) {
    if (g) {
        spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
        if (group) {
            const float move = 1.0f;
            const float x = group->move.x;
            const float y = group->move.y;
            group->dest.x += x > 0 ? move : x < 0 ? -move : 0;
            group->dest.y += y > 0 ? move : y < 0 ? -move : 0;
            group->move.x += x > 0 ? -move : x < 0 ? move : 0;
            group->move.y += y > 0 ? -move : y < 0 ? move : 0;
        } else {
            merror("libgame_update_smoothmove: group is NULL");
        }
    }
}


void libgame_do_camera_lock_on(gamestate* const g) {
    if (g) {
        spritegroup_t* grp = hashtable_entityid_spritegroup_get(g->spritegroups, g->hero_id);
        if (grp) {
            if (g->cam_lockon) {
                g->cam2d.target = (Vector2){grp->dest.x, grp->dest.y};
            }
        } else {
            merror("libgame_do_camera_lock_on: grp is NULL");
        }
    }
}


void libgame_reset_entity_anim(gamestate* const g, entityid id) {
    if (g) {
        spritegroup_t* sg = hashtable_entityid_spritegroup_get(g->spritegroups, id);
        if (sg) {
            sprite* s = spritegroup_get(sg, sg->current);
            if (s && s->num_loops >= 1) {
                //sg->prev_anim = sg->current;
                sg->current = sg->default_anim;
                s->num_loops = 0;
            } else if (!s) {
                merror("libgame_reset_entity_anim: s is NULL");
            }
        }
    }
}


void libgame_reset_entities_anim(gamestate* const g) {
    if (g) {
        for (int i = 0; i < em_count(g->entitymap); i++) {
            libgame_reset_entity_anim(g, i);
        }
    } else if (!g) {
        merror("libgame_reset_entities_anim: gamestate is NULL");
    }
}


void libgame_update_gamestate(gamestate* g) {
    //minfo("libgame_update_gamestate begin");
    if (g) {
        //UpdateMusicStream(test_music);
        libgame_update_debug_panel_buffer(g);
        //setdebugpanelcenter(g);
        libgame_reset_entities_anim(g);
        libgame_do_camera_lock_on(g);
        for (int i = 0; i < em_count(g->entitymap); i++) {
            entity* e = em_get(g->entitymap, i);
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
    } else if (!g) {
        merror("libgame_update_gamestate: gamestate is NULL");
    }

    //minfo("libgame_update_gamestate end");
}


void libgame_drawframeend_unsafe(gamestate* const g) {
    //if (!g) {
    //    merror("libgame_drawframeend: gamestate is NULL");
    //    return;
    //}

    //if (g) {
    EndDrawing();
    // update gamestate values at end of draw frame:
    // framecount, currenttime, currenttimetm, currenttimebuf
    g->framecount++;
    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&(g->currenttime));
    strftime(g->currenttimebuf, 64, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
    //}
}


void libgame_drawframe(gamestate* g) {
    //if (!g) {
    //    merror("libgame_drawframe: gamestate is NULL");
    //    return;
    //}
    if (g) {
        BeginDrawing();
        BeginTextureMode(target);
        switch (activescene) {
        //case SCENE_COMPANY:
        //    libgame_draw_company_scene(g);
        //    break;
        case SCENE_TITLE:
            libgame_draw_title_scene_unsafe(g);
            break;
        case SCENE_GAMEPLAY:
            libgame_draw_gameplayscene_unsafe(g);
            break;
        default:
            break;
        }
        EndTextureMode();
        DrawTexturePro(target.texture, target_src, target_dest, target_origin, 0.0f, WHITE);
        libgame_draw_debug_panel_unsafe(g);
        libgame_drawframeend_unsafe(g);
    }
}


void libgame_calc_debugpanel_size_unsafe(gamestate* const g) {
    const int fontsize = 14, spacing = 1;
    const Vector2 m = MeasureTextEx(g->font, g->debugpanel.buffer, fontsize, spacing);
    g->debugpanel.w = m.x;
    g->debugpanel.h = m.y;
}


inline void libgame_draw_debug_panel_unsafe(gamestate* const g) {
    if (g->debugpanelon) {
        const int fontsize = 14, spacing = 1, xy = 10, wh = 20;
        //const Color c = {0x33, 0x33, 0x33, 255};
        const Color c = {0, 0, 0, 0};
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
}


void libgame_set_tile_params_unsafe(gamestate* const g, dungeon_tile_type_t type, int i, int j) {
    const int size = DEFAULT_TILE_SIZE;
    tile_dest.x = j * DEFAULT_TILE_SIZE;
    tile_dest.y = i * DEFAULT_TILE_SIZE;
    if (type == DUNGEON_TILE_TYPE_STONE_WALL_00) {
        tile_src = (Rectangle){0, 0, size, 16};
        tile_dest = (Rectangle){tile_dest.x, tile_dest.y - 8, size, 16};
    } else if (type == DUNGEON_TILE_TYPE_STONE_WALL_01) {
        tile_src = (Rectangle){0, 0, size, 12};
        tile_dest = (Rectangle){tile_dest.x, tile_dest.y - 4, size, 12};
    } else if (type == DUNGEON_TILE_TYPE_STONE_WALL_02) {
        tile_src = (Rectangle){0, 0, size, size};
        tile_dest = (Rectangle){tile_dest.x, tile_dest.y, size, size};

    } else if (type == DUNGEON_TILE_TYPE_FLOOR_DIRT) {
        tile_src = (Rectangle){0, 0, size, size};
        tile_dest = (Rectangle){tile_dest.x, tile_dest.y, size, size};
    }
}


void libgame_set_tile_params(gamestate* const g, dungeon_tile_type_t type, int i, int j) {
    if (g) {
        libgame_set_tile_params_unsafe(g, type, i, j);
    }
}


void libgame_draw_dungeon_floor_tiles_unsafe_floors(gamestate* const g) {
    for (int i = 0; i < g->dungeon_floor->height; i++) {
        for (int j = 0; j < g->dungeon_floor->width; j++) {
            const dungeon_tile_type_t type = g->dungeon_floor->tiles[i][j].type;
            switch (type) {
            case DUNGEON_TILE_TYPE_FLOOR_DIRT: {
                const int key = get_txkey_for_tiletype(type);
                if (key != -1) {
                    libgame_set_tile_params_unsafe(g, type, i, j);
                    DrawTexturePro(g->txinfo[key].texture, tile_src, tile_dest, zero_vec, 0, WHITE);
                } else {
                    merror("libgame_draw_dungeon_floor_tiles_unsafe: key is -1");
                }
            } break;
            default: {
                //merror("libgame_draw_dungeon_floor_tiles_unsafe: default case");
            } break;
            }
        }
    }
}


void libgame_draw_dungeon_floor_tiles_unsafe_walls(gamestate* const g) {
    for (int i = 0; i < g->dungeon_floor->height; i++) {
        for (int j = 0; j < g->dungeon_floor->width; j++) {
            const dungeon_tile_type_t type = g->dungeon_floor->tiles[i][j].type;
            switch (type) {
            case DUNGEON_TILE_TYPE_STONE_WALL_00:
            case DUNGEON_TILE_TYPE_STONE_WALL_01:
            case DUNGEON_TILE_TYPE_STONE_WALL_02: {
                const int key = get_txkey_for_tiletype(type);
                if (key != -1) {
                    libgame_set_tile_params_unsafe(g, type, i, j);
                    DrawTexturePro(g->txinfo[key].texture, tile_src, tile_dest, zero_vec, 0, WHITE);
                } else {
                    merror("libgame_draw_dungeon_floor_tiles_unsafe: key is -1");
                }
            } break;
            default:
                break;
            }
        }
    }
}


void libgame_draw_dungeon_floor_tiles_3d_unsafe(gamestate* const g) {
    const float w = 1.0f;
    const float h = 1.0f;
    const float d = 1.0f;
    const float x = -10.0f;
    const float y = 0.0f;
    const float z = 10.0f;
    //DrawCube((Vector3){10, 0, 0}, w, h, d, RED);
    Color color = BROWN;
    for (int i = 0; i < g->dungeon_floor->height; i++) {
        for (int j = 0; j < g->dungeon_floor->width; j++) {
            dungeon_tile_type_t tiletype = g->dungeon_floor->tiles[i][j].type;
            if (tiletype == DUNGEON_TILE_TYPE_STONE_WALL_00) {
                color = DARKGRAY;
                DrawCube((Vector3){j, 0.5, i}, w, h * 2, d, color);
            } else if (tiletype == DUNGEON_TILE_TYPE_STONE_WALL_01) {
                color = GRAY;
                DrawCube((Vector3){j, 0.5, i}, w, h * 2, d, color);
            } else if (tiletype == DUNGEON_TILE_TYPE_STONE_WALL_02) {
                color = LIGHTGRAY;
                DrawCube((Vector3){j, 0.5, i}, w, h * 2, d, color);

            } else if (tiletype == DUNGEON_TILE_TYPE_FLOOR_DIRT) {
                color = BROWN;
                DrawPlane((Vector3){j, 0, i}, (Vector2){w, d}, color);
            }
        }
    }
}


void libgame_draw_dungeon_floor_entities(gamestate* const g) {
    if (g) {
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
}


void libgame_draw_dungeon_floor_entities_unsafe(gamestate* const g) {
    for (int i = 0; i < g->dungeon_floor->height; i++) {
        for (int j = 0; j < g->dungeon_floor->width; j++) {
            dungeon_tile_t* tile = &g->dungeon_floor->tiles[i][j];
            for (int k = 0; k < tile->entity_count; k++) {
                libgame_draw_entity_shadow(g, tile->entities[k]);
                libgame_draw_entity(g, tile->entities[k]);
            }
        }
    }
}


void libgame_draw_dungeon_floor_entities_3d_unsafe(gamestate* const g) {
    for (int i = 0; i < g->dungeon_floor->height; i++) {
        for (int j = 0; j < g->dungeon_floor->width; j++) {
            dungeon_tile_t* tile = &g->dungeon_floor->tiles[i][j];
            for (int k = 0; k < tile->entity_count; k++) {
                const entity_t* e = em_get(g->entitymap, tile->entities[k]);
                if (e) {
                    Color c = GREEN;
                    if (e->type == ENTITY_NPC) {
                        c = RED;
                    } else if (e->type == ENTITY_PLAYER) {
                        c = GREEN;
                    }
                    DrawCube((Vector3){j, 0.5, i}, 1.0f, 1.0f, 1.0f, c);
                }
            }
        }
    }
}


void libgame_draw_dungeon_floor(gamestate* const g) {
    if (g) {
        libgame_draw_dungeon_floor_unsafe(g);
        //if (g->dungeon_floor->height == -1 || g->dungeon_floor->width == -1) {
        //    merror("libgame_draw_dungeonfloor: row_count or col_count is -1");
        //} else {
        //    libgame_draw_dungeon_floor_tiles_unsafe_floors(g);
        //    libgame_draw_dungeon_floor_entities_unsafe(g);
        //    libgame_draw_dungeon_floor_tiles_unsafe_walls(g);
        //}
    } else {
        merror("libgame_draw_dungeon_floor: gamestate is NULL");
    }
}


void libgame_draw_dungeon_floor_unsafe(gamestate* const g) {
    if (g->dungeon_floor->height == -1 || g->dungeon_floor->width == -1) {
        merror("libgame_draw_dungeonfloor: row_count or col_count is -1");
    } else {
        libgame_draw_dungeon_floor_tiles_unsafe_floors(g);
        libgame_draw_dungeon_floor_entities_unsafe(g);
        libgame_draw_dungeon_floor_tiles_unsafe_walls(g);
    }
}


void libgame_draw_dungeon_floor_3d(gamestate* const g) {
    if (g) {
        if (g->dungeon_floor->height == -1 || g->dungeon_floor->width == -1) {
            merror("libgame_draw_dungeonfloor: row_count or col_count is -1");
        } else {
            libgame_draw_dungeon_floor_tiles_3d_unsafe(g);
            libgame_draw_dungeon_floor_entities_3d_unsafe(g);
        }
    } else {
        merror("libgame_draw_dungeon_floor: gamestate is NULL");
    }
}


void libgame_draw_dungeon_floor_3d_unsafe(gamestate* const g) {
    if (g->dungeon_floor->height == -1 || g->dungeon_floor->width == -1) {
        merror("libgame_draw_dungeonfloor: row_count or col_count is -1");
    } else {
        libgame_draw_dungeon_floor_tiles_3d_unsafe(g);
        libgame_draw_dungeon_floor_entities_3d_unsafe(g);
    }
}


void libgame_draw_entity_shadow(gamestate* const g, const entityid id) {
    if (g) {
        if (id == -1) {
            merror("libgame_draw_entity_shadow: id is -1");
        } else {
            spritegroup_t* grp = hashtable_entityid_spritegroup_get(g->spritegroups, id);
            if (grp) {
                // draw entity shadow, which should exist at current+1
                // if loaded correctly
                // we need to check for the pointer at current+1 to be non-NULL
                const sprite* const s = grp->sprites[grp->current + 1];
                if (s) {
                    DrawTexturePro(*s->texture, s->src, grp->dest, zero_vec, 0.0f, WHITE);
                } else {
                    merror("libgame_draw_entity_shadow: s is NULL");
                }
            } else {
                merror("libgame_draw_entity_shadow: grp is NULL");
            }
        }
    } else {
        merror("libgame_draw_entity_shadow: gamestate is NULL");
    }
}


void libgame_draw_entity(gamestate* const g, const entityid id) {
    if (g) {
        const specifier_t spec = SPECIFIER_NONE;
        spritegroup_t* group = hashtable_entityid_spritegroup_get_by_specifier(g->spritegroups, id, spec);
        if (group) {
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
            if (g->debugpanelon) {
                const int x = group->dest.x;
                const int y = group->dest.y;
                const int w = group->dest.width;
                const int h = group->dest.height;
                // first draw the outer rectangle without the offset
                Vector2 v[4] = {{x, y}, {x + w, y}, {x + w, y + h}, {x, y + h}};
                //DrawLineV(v[0], v[1], (Color){0, 0, 255, 255});
                //DrawLineV(v[1], v[2], (Color){0, 0, 255, 255});
                //DrawLineV(v[2], v[3], (Color){0, 0, 255, 255});
                //DrawLineV(v[3], v[0], (Color){0, 0, 255, 255});
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
        } else {
            merror("libgame_draw_entity: group is NULL");
        }
    } else {
        merror("libgame_draw_entity: gamestate is NULL");
    }
}


void libgame_draw_gameplayscene(gamestate* const g) {
    if (g) {
        libgame_draw_gameplayscene_unsafe(g);

    } else {
        merror("libgame_draw_gameplayscene: gamestate is NULL");
    }
}


void libgame_draw_gameplayscene_unsafe(gamestate* const g) {
    if (!g->is3d) {
        BeginMode2D(g->cam2d);
        ClearBackground(BLACK);
        libgame_draw_dungeon_floor_unsafe(g);
        libgame_handle_fade_unsafe(g);
        EndMode2D();
    } else {
        BeginMode3D(g->cam3d);
        ClearBackground(BLACK);
        libgame_draw_dungeon_floor_3d_unsafe(g);
        libgame_handle_fade_unsafe(g);
        EndMode3D();
    }
}


void libgame_draw_title_scene_unsafe(gamestate* const g) {
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
    if (g) {
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
    } else {
        merror("libgame_draw_company_scene: gamestate is NULL");
    }
}


void libgame_load_texture_from_disk(
    gamestate* const g, const int index, const int contexts, const int frames, const bool dodither, const char* path) {
    if (g) {
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
    } else {
        merror("libgame_loadtexture: gamestate is null");
    }
}


void libgame_load_textures_from_disk(gamestate* const g) {
    if (g) {
        const char* texture_file_path = "textures.txt";
        FILE* fp = fopen(texture_file_path, "r");
        if (fp) {
            char line[512];
            char path[256];
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
        } else {
            merror("libgame_loadtextures_from_disk: could not open file");
        }
    } else {
        merror("libgame_loadtextures_from_disk: gamestate is null");
    }
}


void libgame_load_textures(gamestate* const g) {
    if (g) {
        libgame_load_textures_from_disk(g);
    } else {
        merror("libgame_loadtextures: gamestate is null");
    }
}


void libgame_unloadtexture(gamestate* const g, const int index) {
    if (g) {
        if (g->txinfo[index].texture.id > 0) {
            UnloadTexture(g->txinfo[index].texture);
        }
    } else {
        merror("libgame_unloadtexture: gamestate is null");
    }
}


void libgame_unloadtextures(gamestate* const g) {
    //minfo("unloading textures");
    // this can be done smarter, surely...
    if (g) {
        for (int i = TX_DIRT_00; i <= TX_BUCKLER; i++) {
            libgame_unloadtexture(g, i);
        }
    } else {
        merror("libgame_unloadtextures: gamestate is null");
    }
}


void libgame_init() {
    g = gamestateinitptr();
    libgame_initsharedsetup(g);
    msuccess("libgame_init");
}


void libgame_set_default_anim_for_id(gamestate* const g, const entityid id, const int anim) {
    if (g) {
        spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
        if (group) {
            group->default_anim = anim;
        } else {
            merror("libgame_set_default_anim_for_id: group is NULL");
        }
    }
}


void libgame_create_spritegroup(gamestate* const g,
                                const entityid id,
                                int* keys,
                                const int num_keys,
                                const int offset_x,
                                const int offset_y,
                                const specifier_t spec) {
    if (g) {
        spritegroup_t* group = spritegroup_create(SPRITEGROUP_DEFAULT_SIZE);
        if (group) {
            entity* e = em_get(g->entitymap, id);
            if (e) {
                const int dw = g->dungeon_floor->width;
                const int dh = g->dungeon_floor->height;
                if (e->x >= 0) {
                    if (e->x < dw) {
                        if (e->y >= 0) {
                            if (e->y < dh) {
                                for (int i = 0; i < num_keys; i++) {
                                    spritegroup_add(group,
                                                    sprite_create(&g->txinfo[keys[i]].texture,
                                                                  g->txinfo[keys[i]].contexts,
                                                                  g->txinfo[keys[i]].num_frames));
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
                                // update the entity's spritegroup context
                                libgame_entity_update_context_unsafe(g, id, spec, e->direction);
                            } else {
                                merror("libgame_create_spritegroup: y is greater than or equal to dh");
                            }
                        } else {
                            merror("libgame_create_spritegroup: y is less than 0");
                        }
                    } else {
                        merror("libgame_create_spritegroup: x is greater than or equal to dw");
                    }
                } else {
                    merror("libgame_create_spritegroup: x is less than 0");
                }
            } else {
                merror("libgame_create_spritegroup: entity is NULL");
            }
        } else {
            merror("libgame_create_spritegroup: group is NULL");
        }
    } else {
        merror("libgame_create_spritegroup: gamestate is NULL");
    }
}


void libgame_loadtargettexture(gamestate* const g) {
    if (g) {
        target = LoadRenderTexture(g->targetwidth, g->targetheight);
        target_src = (Rectangle){0, 0, g->targetwidth, -g->targetheight},
        target_dest = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};
        //SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
        SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
        // update the gamestate display values
        g->cam2d.offset.x = g->targetwidth / 2.0f; //+ c_offset_x;
        g->cam2d.offset.y = g->targetheight / 4.0f; //+ c_offset_y;
        UpdateCamera(&(g->cam3d), CAMERA_FREE);
        g->cam3d.position = (Vector3){0.0f, 10.0f, 10.0f};
        g->cam3d.target = (Vector3){0.0f, 0.0f, 0.0f};
        g->cam3d.up = (Vector3){0.0f, 1.0f, 0.0f};
        g->cam3d.fovy = 45.0f;
        g->cam3d.projection = CAMERA_PERSPECTIVE;
    } else {
        merror("libgame_loadtargettexture: gamestate is NULL");
    }
}


void libgame_loadfont(gamestate* const g) {
    if (g) {
        g->font = LoadFontEx(DEFAULT_FONT_PATH, 60, 0, 255);
    } else {
        merror("libgame_loadfont: gamestate is NULL");
    }
}


void libgame_init_entityids(gamestate* const g) {
    if (g) {
        g->entityids = (entityid*)malloc(sizeof(entityid) * EM_MAX_SLOTS);
        if (g->entityids) {
            memset(g->entityids, -1, sizeof(entityid) * EM_MAX_SLOTS);
            g->index_entityids = 0;
            g->max_entityids = EM_MAX_SLOTS;
        } else {
            merror("libgame_initsharedsetup: could not allocate entityids");
        }
    } else {
        merror("libgame_init_entityids: gamestate is NULL");
    }
}


void libgame_initsharedsetup(gamestate* const g) {
    if (g) {
        const int hero_x = 4;
        const int hero_y = 4;
        const int orc_x = 5;
        const int orc_y = 5;
        const int c_offset_x = -100;
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
        g->cam2d.offset.x = g->targetwidth / 2.0f + c_offset_x;
        g->cam2d.offset.y = g->targetheight / 4.0f;
        libgame_load_textures(g);
        libgame_init_datastructures(g);
        libgame_create_entity(g, hero_x, hero_y, ENTITY_PLAYER, RACE_HUMAN, "hero");
        // testing
        libgame_create_npc_orc(g, orc_x, orc_y);
        //libgame_create_entity(g, orc_x, orc_y, ENTITY_NPC, RACE_ORC, "orc-0");
        // these dont work right until the text buffer of the debugpanel is filled
        libgame_update_debug_panel_buffer(g);
        libgame_calc_debugpanel_size_unsafe(g);
        //setdebugpanelbottomleft(g);
        setdebugpaneltopleft(g);
    } else {
        merror("libgame_initsharedsetup: gamestate is NULL");
    }
}


void libgame_create_npc_orc(gamestate* const g, const int x, const int y) {
    if (g) {
        libgame_create_entity_unsafe(g, x, y, ENTITY_NPC, RACE_ORC, "orc-0");
    } else {
        merror("libgame_create_npc_orc: gamestate is NULL");
    }
}


void libgame_create_npc_orc_unsafe(gamestate* const g, const int x, const int y) {
    libgame_create_entity_unsafe(g, x, y, ENTITY_NPC, RACE_ORC, "orc-0");
}


void libgame_init_spritegroups(gamestate* const g) {
    if (g) {
        g->spritegroups = hashtable_entityid_spritegroup_create(DEFAULT_HASHTABLE_ENTITYID_SPRITEGROUP_SIZE);
        msuccess("libgame_init_spritegroups");
    } else {
        merror("libgame_init_datastructures: gamestate NULL");
    }
}


void libgame_init_datastructures(gamestate* const g) {
    if (g) {
        libgame_init_spritegroups(g);
        // init the entitymap
        g->entitymap = em_new();
        // init the entities array
        libgame_init_entityids(g);
        g->dungeon_floor = dungeon_floor_create(20, 20);
        dungeon_floor_init(g->dungeon_floor);
    } else {
        merror("libgame_init_datastructures: gamestate is NULL");
    }
}


const bool libgame_create_entity_checks_unsafe(gamestate* const g, const int x, const int y) {
    bool retval = false;
    if (x < 0 || x >= g->dungeon_floor->width) {
        merror("libgame_create_hero: x is out of bounds");
        //retval = false;
    } else if (y < 0 || y >= g->dungeon_floor->height) {
        merror("libgame_create_hero: y is out of bounds");
        //retval = false;
    } else if (g->entitymap == NULL) {
        merror("libgame_create_hero: entitymap is NULL");
        //retval = false;
    } else if (g->dungeon_floor == NULL) {
        merror("libgame_create_hero: dungeon_floor is NULL");
        //retval = false;
    } else if (g->spritegroups == NULL) {
        merror("libgame_create_hero: spritegroups is NULL");
        //retval = false;
    } else if (g->index_entityids >= g->max_entityids) {
        merror("libgame_create_hero: index_entityids is greater than or "
               "equal to max_entityids");
        //retval = false;
    } else {
        // we have to check the tile type of the tile at x y
        // in order to see if an entity can exist on that tile
        // ex: we cant create entities on top of walls etc
        dungeon_tile_t* tile = &g->dungeon_floor->tiles[y][x];
        if (tile->type == DUNGEON_TILE_TYPE_STONE_WALL_00 || tile->type == DUNGEON_TILE_TYPE_STONE_WALL_01 ||
            tile->type == DUNGEON_TILE_TYPE_STONE_WALL_02) {
            merror("libgame_create_hero: entity cannot be created on a wall");
            //retval = false;
        } else {
            retval = true;
        }
    }
    return retval;
}


// written by gpt-3.5-turbo on 3/1/2025
void libgame_create_entity(
    gamestate* const g, const int x, const int y, entitytype_t type, race_t race, const char* name) {
    minfo("libgame_create_entity");
    if (g) {
        libgame_create_entity_unsafe(g, x, y, type, race, name);
    } else {
        merror("libgame_create_hero: gamestate is NULL");
    }
}


void libgame_create_entity_unsafe(
    gamestate* const g, const int x, const int y, entitytype_t type, race_t race, const char* name) {
    minfo("libgame_create_entity_unsafe");
    if (!libgame_create_entity_checks_unsafe(g, x, y)) {
        merror("One or more checks failed in libgame_create_hero");
    } else {
        entity* e = entity_new_at(next_entity_id++, type, x, y);
        if (!e) {
            merror("libgame_create_hero: could not create hero entity");
        } else {
            entity_set_name(e, name);
            entity_set_race(e, race);
            em_add(g->entitymap, e);
            if (type == ENTITY_PLAYER) {
                g->hero_id = e->id;
            }
            gamestate_add_entityid(g, e->id);
            dungeon_floor_add_at(g->dungeon_floor, e->id, x, y);
            // based on the NPC's race
            libgame_set_race_specific_parameters(e);
        }
    }
}


// written by gpt-3.5-turbo on 3/1/2025
void libgame_set_race_specific_parameters(entity* e) {
    // set race-specific parameters
    if (e) {
        int* keys = NULL;
        int num_keys = 0;
        int default_anim = 0;
        switch (e->race) {
        case RACE_HUMAN:
            keys = TX_HUMAN_KEYS;
            num_keys = TX_HUMAN_KEY_COUNT;
            default_anim = SPRITEGROUP_ANIM_HUMAN_IDLE;
            break;
        case RACE_ORC:
            keys = TX_ORC_KEYS;
            num_keys = TX_ORC_KEY_COUNT;
            default_anim = SPRITEGROUP_ANIM_ORC_IDLE;
            break;
        // add more cases for additional races as needed
        default:
            merror("Unknown race encountered!");
            return;
        }
        const int off_x = -12;
        const int off_y = -12;
        libgame_create_spritegroup(g, e->id, keys, num_keys, off_x, off_y, SPECIFIER_NONE);
        libgame_set_default_anim_for_id(g, e->id, default_anim);
    } else {
        merror("libgame_set_race_specific_parameters: entity is NULL");
    }
}


void libgame_initwithstate(gamestate* const state) {
    if (state == NULL) {
        merror("libgame_initwithstate: gamestate is NULL");
    } else {
        g = state;
        libgame_initsharedsetup(g);
    }
}


void libgame_closesavegamestate() {
    libgame_closeshared(g);
}


void libgame_close(gamestate* g) {
    if (g) {
        libgame_closeshared(g);
        gamestatefree(g);
    } else {
        merror("libgame_close: gamestate is NULL");
    }
}


void libgame_closeshared(gamestate* const g) {
    if (g) {
        // dont need to free most of gamestate
        minfo("libgame_closeshared");
        //UnloadMusicStream(test_music);
        //CloseAudioDevice();
        UnloadFont(g->font);
        libgame_unloadtextures(g);
        UnloadRenderTexture(target);
        CloseWindow();
        msuccess("libgame_closeshared end");
    } else {
        merror("libgame_closeshared: gamestate is NULL");
    }
}


gamestate* libgame_getgamestate() {
    return g;
}


const bool libgame_external_check_reload() {
    return IsKeyPressed(KEY_R);
}
