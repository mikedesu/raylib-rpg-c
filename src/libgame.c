#include "actions.h"
#include "controlmode.h"
#include "direction.h"
#include "dungeonfloor.h"
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
#include "utils.h"
#include "vectorentityid.h"
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
Rectangle target_src = (Rectangle){0, 0, 0, 0};
Rectangle target_dest = (Rectangle){0, 0, 0, 0};
Vector2 target_origin = (Vector2){0, 0};

int activescene = GAMEPLAYSCENE;
//int activescene = COMPANYSCENE;
int targetwidth = -1;
int targetheight = -1;
int windowwidth = -1;
int windowheight = -1;

entityid buckler_id = -1;
//Music test_music = {0};



void libgame_draw_fade(gamestate* g) {
    if (g->fadealpha > 0) {
        Color c = {0, 0, 0, g->fadealpha};
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), c);
    }
}




void libgame_handle_fade(gamestate* g) {
    const int fadespeed = 4;
    // modify the fadealpha
    if (g->fadestate == FADESTATEOUT && g->fadealpha < 255) {
        g->fadealpha += fadespeed;
    } else if (g->fadestate == FADESTATEIN && g->fadealpha > 0) {
        g->fadealpha -= fadespeed;
    }
    // handle scene rotation based on fadealpha
    if (g->fadealpha >= 255) {
        g->fadealpha = 255;
        g->fadestate = FADESTATEIN;
        activescene++;
        if (activescene > 2) {
            activescene = 0;
        }
    }
    // halt fade if fadealpha is 0
    if (g->fadealpha <= 0) {
        g->fadealpha = 0;
        g->fadestate = FADESTATENONE;
    }
    libgame_draw_fade(g);
}




void libgame_entity_anim_incr(gamestate* g, entityid id) {
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (group) {
        spritegroup_incr(group);
    }
}




void libgame_entity_anim_set(gamestate* g, entityid id, int index) {
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (group) {
        //char buf[128];
        //snprintf(buf, 128, "entity_anim_set: id: %d, index: %d", id, index);
        //msuccess(buf);
        //s->num_loops = 0;
        spritegroup_set_current(group, index);
    }
}




void libgame_test_enemy_placement(gamestate* g) {
    minfo("test_enemy_placement begin");
    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
    if (hero_id != -1) {
        const int x = libgame_lua_get_entity_int(L, hero_id, "x") + 1;
        const int y = libgame_lua_get_entity_int(L, hero_id, "y");
        if (!libgame_lua_tile_is_occupied_by_npc(L, x, y)) {
            entityid id = libgame_create_orc_lua(g, "orc", x, y);
            if (id == -1) {
                merror("test_enemy_placement: failed to create orc");
            } else {
                char buf[128];
                snprintf(buf, 128, "test_enemy_placement: orc created, id: %d", id);
                msuccess(buf);
            }
        } else {
            merror("test_enemy_placement: tile is occupied by npc");
        }
    } else {
        merror("test_enemy_placement: hero_id is -1");
    }
}




const int libgame_get_x_from_dir(direction_t dir) {
    return dir == DIRECTION_RIGHT || dir == DIRECTION_DOWN_RIGHT || dir == DIRECTION_UP_RIGHT ? 1 : dir == DIRECTION_LEFT || dir == DIRECTION_DOWN_LEFT || dir == DIRECTION_UP_LEFT ? -1 : 0;
}




const int libgame_get_y_from_dir(direction_t dir) {
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



//void libgame_update_anim_indices(gamestate* g) {
//    minfo("updating anim indices...");
//    // update the animation index for all NPCs and player
//    const int count = libgame_lua_get_num_entities(L);
//    for (int i = 0; i < count; i++) {
//        const entityid id = libgame_lua_get_nth_entity(L, i + 1);
//        const race_t race = libgame_lua_get_entity_int(L, id, "race");
//        char buf[128];
//        snprintf(buf, 128, "updating anim index: id: %d  race: %d\n", id, race);
//        minfo(buf);
//        if (id != -1) {
//            spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
//            if (group) {
//                const int index = race == RACE_HUMAN ? SPRITEGROUP_ANIM_HUMAN_WALK : race == RACE_ORC ? SPRITEGROUP_ANIM_ORC_WALK : -1;
//                spritegroup_set_current(group, index);
//            }
//        }
//    }
//}




void libgame_handleinput(gamestate* g) {
    //minfo("handleinput: starting...");
    //if (IsKeyPressed(KEY_SPACE) || GetTouchPointCount() > 0) {
    //    //    minfo("key space pressed");
    //    if (g->fadestate == FADESTATENONE) {
    //        g->fadestate = FADESTATEOUT;
    //    }
    //}

    // lets cycle the animation for the hero
    //libgame_entity_anim_incr(g, g->hero_id);
    // technically we dont want to be able to attack until we have picked up a weapon...
    //libgame_handle_player_attack(g);
    //g->player_input_received = true;

    if (IsKeyPressed(KEY_E)) {
        libgame_test_enemy_placement(g);
        //    g->player_input_received = true;
    }

    //if (IsKeyPressed(KEY_P)) {
    //    minfo("clearing was damaged...");
    //    libgame_lua_clear_was_damaged(L);
    //    libgame_update_anim_indices(g);
    //}

    // lets place a torch where the player is standing
    //if (IsKeyPressed(KEY_T)) {
    //    entity_t* hero = hashtable_entityid_entity_get(g->entities, g->hero_id);
    //    if (hero) {
    //        // check to see if there are any items at that location
    //        tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, hero->x, hero->y);
    //        if (t) {
    //            for (int i = 0; i < vectorentityid_capacity(&t->entityids); i++) {
    //                entityid id = vectorentityid_get(&t->entityids, i);
    //                entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
    //                if (entity->type == ENTITY_ITEM && entity->itemtype == ITEM_TORCH) {
    //                    minfo("torch already exists at this location");
    //                } else {
    //                    //libgame_createitembytype(g, ITEM_TORCH, hero->x, hero->y);
    //                }
    //            }
    //        }
    //    }
    //    g->player_input_received = true;
    //}
    libgame_handle_modeswitch(g);
    libgame_handle_debugpanel_switch(g);
    libgame_handle_grid_switch(g);
    libgame_handle_input_player(g);
    libgame_handle_caminput(g);
}




void libgame_handle_modeswitch(gamestate* g) {
    if (IsKeyPressed(KEY_C)) {
        g->controlmode = g->controlmode == CONTROLMODE_CAMERA ? CONTROLMODE_PLAYER : CONTROLMODE_CAMERA;
    }
}




void libgame_handle_debugpanel_switch(gamestate* g) {
    if (IsKeyPressed(KEY_D)) {
        minfo("debug panel switch");
        g->debugpanelon = !g->debugpanelon;
    }
}




void libgame_handle_grid_switch(gamestate* g) {
    if (IsKeyPressed(KEY_G)) {
        minfo("grid switch");
        g->gridon = !g->gridon;
    }
}




void libgame_update_spritegroup(gamestate* g, entityid id, direction_t dir) {
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (group) {
        int old_ctx = group->sprites[group->current]->currentcontext;
        int ctx = old_ctx;
        switch (dir) {
        case DIRECTION_RIGHT:
            ctx = ctx == SPRITEGROUP_CONTEXT_R_D   ? SPRITEGROUP_CONTEXT_R_D
                  : ctx == SPRITEGROUP_CONTEXT_L_D ? SPRITEGROUP_CONTEXT_R_D
                  : ctx == SPRITEGROUP_CONTEXT_R_U ? SPRITEGROUP_CONTEXT_R_U
                                                   : SPRITEGROUP_CONTEXT_R_U;
            break;
        case DIRECTION_LEFT:
            ctx = ctx == SPRITEGROUP_CONTEXT_R_D   ? SPRITEGROUP_CONTEXT_L_D
                  : ctx == SPRITEGROUP_CONTEXT_L_D ? SPRITEGROUP_CONTEXT_L_D
                  : ctx == SPRITEGROUP_CONTEXT_R_U ? SPRITEGROUP_CONTEXT_L_U
                                                   : SPRITEGROUP_CONTEXT_L_U;
            break;
        case DIRECTION_UP:
            ctx = ctx == SPRITEGROUP_CONTEXT_R_D   ? SPRITEGROUP_CONTEXT_R_U
                  : ctx == SPRITEGROUP_CONTEXT_L_D ? SPRITEGROUP_CONTEXT_L_U
                  : ctx == SPRITEGROUP_CONTEXT_R_U ? SPRITEGROUP_CONTEXT_R_U
                                                   : SPRITEGROUP_CONTEXT_L_U;
            break;
        case DIRECTION_DOWN:
            ctx = ctx == SPRITEGROUP_CONTEXT_R_D   ? SPRITEGROUP_CONTEXT_R_D
                  : ctx == SPRITEGROUP_CONTEXT_L_D ? SPRITEGROUP_CONTEXT_L_D
                  : ctx == SPRITEGROUP_CONTEXT_R_U ? SPRITEGROUP_CONTEXT_R_D
                                                   : SPRITEGROUP_CONTEXT_L_D;
            break;
        case DIRECTION_UP_LEFT:
            ctx = SPRITEGROUP_CONTEXT_L_U;
            break;
        case DIRECTION_UP_RIGHT:
            ctx = SPRITEGROUP_CONTEXT_R_U;
            break;
        case DIRECTION_DOWN_LEFT:
            ctx = SPRITEGROUP_CONTEXT_L_D;
            break;
        case DIRECTION_DOWN_RIGHT:
            ctx = SPRITEGROUP_CONTEXT_R_D;
            break;
        case DIRECTION_NONE:
            ctx = old_ctx;
            break;

        default:
            break;
        }
        spritegroup_setcontexts(group, ctx);
    }
}




//void libgame_update_spritegroup_current(gamestate* g, entityid id, int index) {
//const race_t race = libgame_lua_get_entity_int(L, id, "race");
//    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
//    if (group) {
//const int index = race == RACE_HUMAN ? SPRITEGROUP_ANIM_HUMAN_WALK : race == RACE_ORC ? SPRITEGROUP_ANIM_ORC_WALK : -1;
//        spritegroup_set_current(group, index);
//    }
//}




void libgame_update_spritegroup_move(gamestate* g, const entityid id, const int x, const int y) {
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (sg) {
        sg->move_x += x;
        sg->move_y += y;
    }
}




void libgame_handleplayerinput_move(gamestate* g, const int xdir, const int ydir) {
    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
    if (hero_id != -1) {
        libgame_lua_create_action(L, hero_id, ACTION_MOVE, xdir, ydir);
    } else {
        merror("handleplayerinput_move: hero_id is -1");
    }
}




void libgame_handle_player_input_movement_key(gamestate* g, const direction_t dir) {
    if (g) {
        const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
        if (hero_id != -1) {
            const int xdir = libgame_get_x_from_dir(dir);
            const int ydir = libgame_get_y_from_dir(dir);
            // update player direction
            libgame_lua_set_entity_int(L, hero_id, "direction", dir);
            libgame_handleplayerinput_move(g, xdir, ydir);
            libgame_update_spritegroup(g, hero_id, dir); // updates sg context
            libgame_entity_anim_set(g, hero_id, SPRITEGROUP_ANIM_HUMAN_WALK);
            g->player_input_received = true;
        } else {
            merror("handleplayerinput_key_right: hero_id is -1");
        }
    }
}




//const entityid libgame_entity_pickup_item(gamestate* g, const entityid id) {
//    entityid retval = -1;
//    entity_t* e = hashtable_entityid_entity_get(g->entities, id);
//    tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, e->x, e->y);
//    if (e && t) {
//        minfo("entity and tile pointers acquired, entering loop...");
//        for (int i = 0; i < vectorentityid_capacity(&t->entityids); i++) {
//            entityid id2 = vectorentityid_get(&t->entityids, i);
//            entity_t* entity = hashtable_entityid_entity_get(g->entities, id2);
//            if (entity && entity->type == ENTITY_ITEM) {
//                minfo("item found on tile, removing...");
//                // we are going to remove the item from the tile
//                // and add it to the player's inventory
//                vectorentityid_remove_value(&t->entityids, id2);
//                // add the item to the player's inventory
//                vectorentityid_add(&e->inventory, id2);
//                retval = id2;
//                break;
//            } else {
//                merror("no item found on tile");
//            }
//        }
//    }
//    return retval;
//}



//#ifdef MOBILE
//void libgame_handle_input_player_mobile(gamestate* g) {
//    if (g->controlmode == CONTROLMODE_PLAYER) {
//        int current_gesture = GetGestureDetected();
//        switch (current_gesture) {
//        case GESTURE_TAP:
//            spritegroup_t* grp = hashtable_entityid_spritegroup_get(g->spritegroups, libgame_lua_get_gamestate_int(L, "HeroId"));
//            if (grp) {
//                spritegroup_incr(grp);
//                g->player_input_received = true;
//            }
//            break;
//        case GESTURE_HOLD:
//            libgame_test_enemy_placement(g);
//            g->player_input_received = true;
//            break;
//        case GESTURE_PINCH_OUT:
//            g->cam2d.zoom += 1.0f;
//            break;
//        case GESTURE_PINCH_IN:
//            g->cam2d.zoom -= 1.0f;
//            if (g->cam2d.zoom < 1.0f) {
//                g->cam2d.zoom = 1.0f;
//            }
//            break;
//        case GESTURE_SWIPE_UP:
//            libgame_handle_player_input_movement_key(g, DIRECTION_UP);
//            g->player_input_received = true;
//
//            break;
//        case GESTURE_SWIPE_DOWN:
//            libgame_handle_player_input_movement_key(g, DIRECTION_DOWN);
//            g->player_input_received = true;
//            break;
//        case GESTURE_SWIPE_LEFT:
//            libgame_handle_player_input_movement_key(g, DIRECTION_LEFT);
//            g->player_input_received = true;
//            break;
//        case GESTURE_SWIPE_RIGHT:
//            libgame_handle_player_input_movement_key(g, DIRECTION_RIGHT);
//            g->player_input_received = true;
//            break;
//        default:
//            break;
//        }
//    }
//}
//#endif



void libgame_handle_input_player(gamestate* g) {
    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
    //minfo("handle_playerinput: starting...");
    if (g->controlmode == CONTROLMODE_PLAYER && g->player_input_received == false) {
        //const bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        // this is just a test
        // the real setup will involve managing the player's dungeon position
        // and then translating that into a destination on screen
        // eventually we will create a mapping for custom controls
        // that way we can centralize handling of the controls
        //if (g->player_input_received == false) {
        if (IsKeyPressed(KEY_KP_6) || IsKeyPressed(KEY_RIGHT)) {
            //libgame_handleplayerinput_key_right(g);
            libgame_handle_player_input_movement_key(g, DIRECTION_RIGHT);
        } else if (IsKeyPressed(KEY_KP_4) || IsKeyPressed(KEY_LEFT)) {
            //libgame_handleplayerinput_key_left(g);
            libgame_handle_player_input_movement_key(g, DIRECTION_LEFT);
        } else if (IsKeyPressed(KEY_KP_2) || IsKeyPressed(KEY_DOWN)) {
            //libgame_handleplayerinput_key_down(g);
            libgame_handle_player_input_movement_key(g, DIRECTION_DOWN);
        } else if (IsKeyPressed(KEY_KP_8) || IsKeyPressed(KEY_UP)) {
            //libgame_handleplayerinput_key_up(g);
            libgame_handle_player_input_movement_key(g, DIRECTION_UP);
        } else if (IsKeyPressed(KEY_KP_1)) {
            //libgame_handleplayerinput_key_down_left(g);
            libgame_handle_player_input_movement_key(g, DIRECTION_DOWN_LEFT);
        } else if (IsKeyPressed(KEY_KP_3)) {
            //libgame_handleplayerinput_key_down_right(g);
            libgame_handle_player_input_movement_key(g, DIRECTION_DOWN_RIGHT);
        } else if (IsKeyPressed(KEY_KP_7)) {
            //libgame_handleplayerinput_key_up_left(g);
            libgame_handle_player_input_movement_key(g, DIRECTION_UP_LEFT);
        } else if (IsKeyPressed(KEY_KP_9)) {
            //libgame_handleplayerinput_key_up_right(g);
            libgame_handle_player_input_movement_key(g, DIRECTION_UP_RIGHT);
        }



        else if (IsKeyPressed(KEY_A)) {
            if (hero_id != -1) {
                // create an attack action
                const direction_t dir = libgame_lua_get_entity_int(L, hero_id, "direction");
                const int xdir = libgame_get_x_from_dir(dir);
                const int ydir = libgame_get_y_from_dir(dir);
                bool res = libgame_lua_create_action(L, hero_id, ACTION_ATTACK, xdir, ydir);
                if (res) {
                    msuccess("attack action created");
                    libgame_entity_anim_set(g, hero_id, SPRITEGROUP_ANIM_HUMAN_ATTACK);


                    //spritegroup_t* grp = hashtable_entityid_spritegroup_get(g->spritegroups, hero_id);
                    //if (grp) {
                    //    spritegroup_set_current(grp, SPRITEGROUP_ANIM_HUMAN_ATTACK);
                    //}
                } else {
                    merror("attack action failed to create");
                }
                g->player_input_received = true;
                //g->is_locked = true;
                //g->lock_timer = 0;
            }
        }


        else if (IsKeyPressed(KEY_W)) {
            // test item pickup
            const int x = libgame_lua_get_entity_int(L, hero_id, "x");
            const int y = libgame_lua_get_entity_int(L, hero_id, "y");
            bool res = libgame_lua_create_action(L, hero_id, ACTION_PICKUP, x, y);

            if (res) {
                msuccess("pickup action created");
                //libgame_entity_anim_set(g, hero_id, SPRITEGROUP_ANIM_HUMAN_PICKUP);
            } else {
                merror("pickup action failed to create");
            }

            g->player_input_received = true;
        }



        else if (IsKeyPressed(KEY_B)) {

            if (hero_id != -1) {
                // this only does the block animation
                // we need this to trigger a "block" action

                const direction_t dir = libgame_lua_get_entity_int(L, hero_id, "direction");
                const int xdir = libgame_get_x_from_dir(dir);
                const int ydir = libgame_get_y_from_dir(dir);

                bool res = libgame_lua_create_action(L, hero_id, ACTION_BLOCK, xdir, ydir);

                if (res) {
                    msuccess("block action created");
                    //libgame_entity_anim_set(g, hero_id, SPRITEGROUP_ANIM_HUMAN_PICKUP);
                } else {
                    merror("block action failed to create");
                }

                g->player_input_received = true;


                //libgame_entity_anim_set(g, hero_id, SPRITEGROUP_ANIM_HUMAN_GUARD);
                //g->player_input_received = true;
            }
        }



        //else if (IsKeyPressed(KEY_COMMA)) {
        //minfo("Comma key pressed");
        // look at the tile the player is on
        //libgame_entity_pickup_item(g, g->hero_id);
        //g->player_input_received = true;
        //}

        //if (IsKeyPressed(KEY_P)) {
        //    minfo("Key pressed: P");
        //    libgame_lua_mytest(L);
        //    libgame_lua_reserialization_test(L);
        //}

        else if (IsKeyPressed(KEY_SPACE)) {
            //    // randomize the dungeon tiles
            const int w = 4;
            const int h = 4;
            //const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
            //int hx = libgame_lua_get_entity_int(L, g->hero_id, "x") - w / 2;
            //int hy = libgame_lua_get_entity_int(L, g->hero_id, "y") - h / 2;
            const int hx = libgame_lua_get_entity_int(L, hero_id, "x") - w / 2;
            const int hy = libgame_lua_get_entity_int(L, hero_id, "y") - h / 2;
            libgame_lua_randomize_dungeon_tiles(L, hx, hy, w, h);
        }

        else if (IsKeyPressed(KEY_PERIOD)) {

            const direction_t dir = libgame_lua_get_entity_int(L, hero_id, "direction");
            const int xdir = libgame_get_x_from_dir(dir);
            const int ydir = libgame_get_y_from_dir(dir);
            bool res = libgame_lua_create_action(L, hero_id, ACTION_NONE, xdir, ydir);
            if (res) {
                msuccess("attack action created");
                libgame_entity_anim_set(g, hero_id, SPRITEGROUP_ANIM_HUMAN_IDLE);

                //spritegroup_t* grp = hashtable_entityid_spritegroup_get(g->spritegroups, hero_id);
                //if (grp) {
                //    spritegroup_set_current(grp, SPRITEGROUP_ANIM_HUMAN_ATTACK);
                //}
            } else {
                merror("attack action failed to create");
            }
            g->player_input_received = true;
        }
        //libgame_handle_npcs_turn_lua(g);
        //libgame_process_turn(g);
        //int action_count = libgame_lua_get_action_count(L);
        //if (action_count > 0) {
        //    for (int i = 0; i < action_count; i++) {
        //        int entity_id = libgame_lua_process_action(L, i + 1);
        //        if (entity_id != -1) {
        //            const int xdir = libgame_lua_get_entity_int(L, entity_id, "last_move_x");
        //            const int ydir = libgame_lua_get_entity_int(L, entity_id, "last_move_y");
        //            direction_t dir = DIRECTION_RIGHT;
        //            if (xdir == -1 && ydir == -1) {
        //                dir = DIRECTION_UP_LEFT;
        //            } else if (xdir == 0 && ydir == -1) {
        //                dir = DIRECTION_UP;
        //            } else if (xdir == 1 && ydir == -1) {
        //                dir = DIRECTION_UP_RIGHT;
        //            } else if (xdir == -1 && ydir == 0) {
        //                dir = DIRECTION_LEFT;
        //            } else if (xdir == 1 && ydir == 0) {
        //                dir = DIRECTION_RIGHT;
        //            } else if (xdir == -1 && ydir == 1) {
        //                dir = DIRECTION_DOWN_LEFT;
        //            } else if (xdir == 0 && ydir == 1) {
        //                dir = DIRECTION_DOWN;
        //            } else if (xdir == 1 && ydir == 1) {
        //                dir = DIRECTION_DOWN_RIGHT;
        //            }
        //            libgame_update_spritegroup(g, entity_id, dir);
        //            libgame_update_spritegroup_move(g, entity_id, xdir * DEFAULT_TILE_SIZE, ydir * DEFAULT_TILE_SIZE);
        //        }
        //    }
        //    libgame_lua_clear_actions(L);
        //}

        //libgame_lua_process_actions(L);
        //libgame_entity_look(g, g->hero_id);
        //spritegroup_t* hero_group = hashtable_entityid_spritegroup_get(g->spritegroups, g->hero_id);
        //if (hero_group) {
        //    hero_group->current = 0; //standing/idle
        //}

        //bool result = libgame_lua_entity_move_random_dir(L, g->hero_id);
        //if (result) {
        //    const int xdir = libgame_lua_get_entity_int(L, g->hero_id, "last_move_x");
        //    const int ydir = libgame_lua_get_entity_int(L, g->hero_id, "last_move_y");
        //    libgame_handleplayerinput_move(g, xdir, ydir);
        //}
        //    g->player_input_received = true;
        //}


#ifdef MOBILE
        libgame_handle_input_player_mobile(g);
#endif
    }
}



void libgame_update_spritegroup_by_lastmove(gamestate* g, entityid entity_id) {
    const int xdir = libgame_lua_get_entity_int(L, entity_id, "last_move_x");
    const int ydir = libgame_lua_get_entity_int(L, entity_id, "last_move_y");
    const direction_t dir = libgame_get_dir_from_xy(xdir, ydir);
    libgame_update_spritegroup(g, entity_id, dir);
    libgame_update_spritegroup_move(g, entity_id, xdir * DEFAULT_TILE_SIZE, ydir * DEFAULT_TILE_SIZE);
}




void libgame_process_turn_actions(gamestate* g) {
    const int action_count = libgame_lua_get_action_count(L);
    if (action_count > 0) {
        for (int i = 0; i < action_count; i++) {
            const action_t action_type = libgame_lua_get_nth_action_type(L, i + 1);
            const entityid entity_id = libgame_lua_get_nth_action_id(L, i + 1);
            const int x = libgame_lua_get_nth_action_x(L, i + 1);
            const int y = libgame_lua_get_nth_action_y(L, i + 1);
            const int result_id = libgame_lua_process_action(L, i + 1);
            if (result_id != -1) {
                if (action_type == ACTION_NONE) {
                    libgame_update_spritegroup(g, entity_id, DIRECTION_NONE);

                } else if (action_type == ACTION_MOVE) {
                    // get entity last move
                    libgame_update_spritegroup_by_lastmove(g, result_id);
                }
            } else {
                //const int xdir = libgame_lua_get_entity_int(L, entity_id, "last_move_x");
                //const int ydir = libgame_lua_get_entity_int(L, entity_id, "last_move_y");
                const int xdir = x;
                const int ydir = y;
                const direction_t dir = libgame_get_dir_from_xy(xdir, ydir);
                libgame_update_spritegroup(g, entity_id, dir);
            }
        }
    }
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




void libgame_process_turn(gamestate* g) {
    libgame_process_turn_actions(g);
    libgame_lua_clear_actions(L);
}




void libgame_handle_caminput(gamestate* g) {
    if (g->controlmode == CONTROLMODE_CAMERA) {
        const bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        const float zoom_incr = 1.00f;
        const float cam_move_incr = 4;

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
        } else if (IsKeyDown(KEY_KP_1)) {
            g->cam2d.offset.x -= cam_move_incr;
            g->cam2d.offset.y += cam_move_incr;
        } else if (IsKeyDown(KEY_KP_2)) {
            g->cam2d.offset.y += cam_move_incr;
        } else if (IsKeyDown(KEY_KP_3)) {
            g->cam2d.offset.x += cam_move_incr;
            g->cam2d.offset.y += cam_move_incr;
        } else if (IsKeyDown(KEY_KP_4)) {
            g->cam2d.offset.x -= cam_move_incr;
        } else if (IsKeyDown(KEY_KP_6)) {
            g->cam2d.offset.x += cam_move_incr;
        } else if (IsKeyDown(KEY_KP_7)) {
            g->cam2d.offset.x -= cam_move_incr;
            g->cam2d.offset.y -= cam_move_incr;
        } else if (IsKeyDown(KEY_KP_8)) {
            g->cam2d.offset.y -= cam_move_incr;
        } else if (IsKeyDown(KEY_KP_9)) {
            g->cam2d.offset.x += cam_move_incr;
            g->cam2d.offset.y -= cam_move_incr;
        } else if (IsKeyDown(KEY_KP_0)) {
            g->cam2d.zoom = 2.0f;
        } else if (IsKeyPressed(KEY_R)) {
            g->cam2d.offset = (Vector2){0, 0};
        } else if (IsKeyPressed(KEY_F)) {
            g->cam_lockon = !g->cam_lockon;
        }
    }
}




bool libgame_windowshouldclose() {
    return WindowShouldClose();
}




void libgame_initwindow(gamestate* g) {
    const char* title = libgame_lua_get_gamestate_str(L, "WindowTitle");
    if (!title) {
        title = "Gamestate.WindowTitle not set in init.lua";
    }
    windowwidth = libgame_lua_get_gamestate_int(L, "WindowWidth");
    windowheight = libgame_lua_get_gamestate_int(L, "WindowHeight");
    InitWindow(windowwidth, windowheight, title);
    //InitWindow(width, height, title);
    while (!IsWindowReady())
        ;
    // this is hard-coded for now so we can auto-position the window
    // for easier config during streaming
    //SetWindowMonitor(0);
    SetWindowMonitor(1);
    const int x = libgame_lua_get_gamestate_int(L, "WindowPosX");
    const int y = libgame_lua_get_gamestate_int(L, "WindowPosY");
    //const int x = DEFAULT_WINDOW_POS_X;
    //const int y = DEFAULT_WINDOW_POS_Y;
    SetWindowPosition(x, y);
    SetTargetFPS(DEFAULT_TARGET_FPS);
    SetExitKey(KEY_Q);
    g->display.windowwidth = windowwidth;
    g->display.windowheight = windowheight;
    //minfo("end of libgame_initwindow");
}




void libgame_closewindow() {
    CloseWindow();
}




void libgame_update_debug_panel_buffer(gamestate* g) {
    int hx = -1;
    int hy = -1;
    int last_mv_x = -1;
    int last_mv_y = -1;
    int hero_was_damaged = -1;
    const entityid id = libgame_lua_get_gamestate_int(L, "HeroId");
    if (id != -1) {
        hx = libgame_lua_get_entity_int(L, id, "x");
        hy = libgame_lua_get_entity_int(L, id, "y");
        last_mv_x = libgame_lua_get_entity_int(L, id, "last_move_x");
        last_mv_x = libgame_lua_get_entity_int(L, id, "last_move_y");
        hero_was_damaged = libgame_lua_get_entity_int(L, id, "was_damaged");
    }
    const int dw = libgame_lua_get_dungeonfloor_row_count(L);
    const int dh = libgame_lua_get_dungeonfloor_col_count(L);
    const int action_count = libgame_lua_get_action_count(L);
    const int entity_count = libgame_lua_get_num_entities(L);
    const int dir = libgame_lua_get_entity_int(L, id, "direction");
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
             "Action count: %d\n"
             "Entity count: %d\n"
             "Last move: %d,%d\n"
             "Was damaged: %d\n"
             "Dir: %s\n",

             g->framecount,
             g->timebeganbuf,
             g->currenttimebuf,
             //g->is_locked,
             //g->lock_timer,

             targetwidth,
             targetheight,
             windowwidth,
             windowheight,
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
             //vectorentityid_capacity(&hero->inventory),
             dh,
             dw,
             action_count,
             entity_count,
             last_mv_x,
             last_mv_y,
             hero_was_damaged,
             dir_str

    );
}




void libgame_update_smoothmove(gamestate* g, const entityid id) {
    if (g) {
        spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
        //const int tilesize = DEFAULT_TILE_SIZE;
        if (group) {
            float move_unit = 1.0f;
            // only do it 1 unit at a time
            if (group->move_x > 0) {
                group->dest.x += move_unit;
                group->move_x -= move_unit;
            } else if (group->move_x < 0) {
                group->dest.x -= move_unit;
                group->move_x += move_unit;
            }

            if (group->move_y > 0) {
                group->dest.y += move_unit;
                group->move_y -= move_unit;
            } else if (group->move_y < 0) {
                group->dest.y -= move_unit;
                group->move_y += move_unit;
            }
            // makes sure the entity is at the destination
            //if (group->move_x == 0.0f && group->move_y == 0.0f) {
            //    const int x = libgame_lua_get_entity_int(L, id, "x");
            //    const int y = libgame_lua_get_entity_int(L, id, "y");
            //    group->dest.x = x * tilesize + group->off_x;
            //    group->dest.y = y * tilesize + group->off_y;
            //}
        }
    }
}




void libgame_do_camera_lock_on(gamestate* g) {
    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_get(g->spritegroups, hero_id);
    if (hero_group && g->cam_lockon) {
        g->cam2d.target = (Vector2){hero_group->dest.x, hero_group->dest.y};
    }
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




void libgame_handle_npc_turn_lua(gamestate* g, const entityid id) {
    minfo("libgame_handle_npc_turn_lua begin");
    const int xdir = rand() % 3 - 1;
    const int ydir = rand() % 3 - 1;
    //const int xdir = 0;
    //const int ydir = 0;
    const bool result = libgame_lua_create_action(L, id, 2, xdir, ydir);
    //const bool result = libgame_lua_create_action(L, id, 1, xdir, ydir);
    if (result) {
        msuccess("libgame_handle_npc_turn_lua end");
    } else {
        merror("libgame_handle_npc_turn_lua end");
    }
}



void libgame_handle_npcs_turn_lua(gamestate* g) {
    //minfo("libgame_handle_npcs_turn_lua begin");
    //char buf[128];
    //snprintf(buf, 128, "num entities: %d", libgame_lua_get_num_entities(L));
    //minfo(buf);
    for (int i = 0; i < libgame_lua_get_num_entities(L); i++) {
        const entityid id = libgame_lua_get_nth_entity(L, i + 1);
        const entitytype_t type = libgame_lua_get_entity_int(L, id, "type");
        //char buf2[128];
        //snprintf(buf2, 128, "i: %d, entity id: %d, type: %d", i, id, type);
        //minfo(buf2);
        if (type == ENTITY_NPC) {
            libgame_handle_npc_turn_lua(g, id);
        }
    }
    msuccess("libgame_handle_npcs_turn_lua end");
}




//void libgame_reset_spritegroup_on_loop(gamestate* g, const entityid id) {
//    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
//    if (group) {
//        const int current = group->current;
//        if (current == SPRITEGROUP_ANIM_ORC_DMG) {
//            // get the current sprite from the group and check its loop count
//            sprite* sprite = spritegroup_get(group, current);
//            if (sprite) {
//                if (sprite->num_loops > 0) {
//                    // reset the spritegroup to the default animation
//                    spritegroup_set_current(group, group->default_anim);
//                }
//            } else {
//                merror("could not get sprite");
//            }
//        }
//        spritegroup_set_current(group, group->default_anim);
//    }
//}



void libgame_reset_entity_anim(gamestate* g, entityid id) {
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (sg) {
        int old_index = sg->current;
        sprite* s = spritegroup_get(sg, old_index);
        const int loop_count = 1;
        if (s && s->num_loops >= loop_count) {
            sg->prev_anim = sg->current;
            sg->current = sg->default_anim;
            s->num_loops = 0;
        }
    }
}



void libgame_reset_entities_anim(gamestate* g) {

    const int count = libgame_lua_get_num_entities(L);
    for (int i = 0; i < count; i++) {
        //minfo("resetting animation...");
        libgame_reset_entity_anim(g, i);
    }
}




void libgame_update_entity_damaged_anim(gamestate* g, const int i) {
    const entityid id = libgame_lua_get_nth_entity(L, i + 1);
    const int was_damaged = libgame_lua_get_entity_int(L, id, "was_damaged");
    const race_t race = libgame_lua_get_entity_int(L, id, "race");
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    int index = -1;
    if (was_damaged && sg) {
        int old_index = sg->current;
        if (race == RACE_HUMAN) {
            index = SPRITEGROUP_ANIM_HUMAN_SPINDIE;
            if (index == old_index) {
                sprite* s = spritegroup_get(sg, old_index);
                const int loop_count = 1;
                if (s && s->num_loops >= loop_count) {
                    index = SPRITEGROUP_ANIM_HUMAN_IDLE;
                    libgame_lua_set_entity_int(L, id, "was_damaged", 0);
                    s->num_loops = 0;
                }
            }
            libgame_entity_anim_set(g, id, index);
        } else if (race == RACE_ORC) {
            index = SPRITEGROUP_ANIM_ORC_DMG;
            if (index == old_index) {
                sprite* s = spritegroup_get(sg, old_index);
                const int loop_count = 1;
                if (s && s->num_loops >= loop_count) {
                    index = SPRITEGROUP_ANIM_ORC_IDLE;
                    libgame_lua_set_entity_int(L, id, "was_damaged", 0);
                    s->num_loops = 0;
                }
            }
            libgame_entity_anim_set(g, id, index);
        }
    }
}




void libgame_update_entities_damaged_anim(gamestate* g) {
    const int count = libgame_lua_get_num_entities(L);
    for (int i = 0; i < count; i++) {
        libgame_update_entity_damaged_anim(g, i);
    }
}




void libgame_update_gamestate(gamestate* g) {
    //UpdateMusicStream(test_music);
    //minfo("begin libgame_updategamestate");
    //minfo("libgame_updategamestate: update debug panel buffer");
    libgame_update_debug_panel_buffer(g);
    //setdebugpanelcenter(g);
    //minfo("libgame_updategamestate: update smooth move");


    // need to clean this up a bit but works in general right now for what we want
    // needs extensibility to handle all animation types
    // in essence some animations we only want to loop once, then reset to a default or previous
    libgame_update_entities_damaged_anim(g);

    libgame_reset_entities_anim(g);


    libgame_update_smoothmove(g, libgame_lua_get_gamestate_int(L, "HeroId"));
    //minfo("libgame_updategamestate: do camera lockon");
    libgame_do_camera_lock_on(g);
    // at this point, we can take other NPC turns
    // lets iterate over our entities, find the NPCs, and make them move in a random direction
    // then, we will update their smooth moves
    // we will need to eventually disable player input during smooth moving
    //if (g->player_input_received) {
    //    libgame_handle_npcs_turn_lua(g);
    //    libgame_process_turn(g);
    //    g->player_input_received = false;
    //}
    //minfo("libgame_updategamestate: update smooth moves for NPCs");
    // update smooth move for NPCs and other entities
    libgame_update_smoothmoves_for_entitytype(g, ENTITY_NPC);
    //msuccess("end libgame_updategamestate");


    // i plan on using the lock and lock timer to control an ebb-and-flow
    // so that it doesnt appear like enemies move immediately as the player
    // does
    //if (g->is_locked && g->lock_timer > 0) {
    //    g->lock_timer--;
    //} else if (g->is_locked && g->lock_timer <= 0) {
    //    g->is_locked = false;
    //    g->lock_timer = 0;

    if (g->player_input_received) {
        libgame_process_turn(g);
        g->player_input_received = false;

        //libgame_handle_npcs_turn_lua(g);
        //libgame_process_turn(g);

        //g->is_locked = true;
        //g->lock_timer = 60;
    } else {

        // while this works, we would prefer to "step" thru each entity's turn
        // presently, this makes it look like all NPCs move at once
        // but, in combination with the lock_timer, i believe we can introduce
        // an index into the gamestate to keep track of which entity acted last,
        // and once we've finished iterating asyncronously, we can assign then
        // receive player input again and repeat the cycle
        //for (int i = 0; i < libgame_lua_get_num_entities(L); i++) {
        //    const entityid id = libgame_lua_get_nth_entity(L, i + 1);
        //    const entitytype_t type = libgame_lua_get_entity_int(L, id, "type");
        //    if (type == ENTITY_NPC) {
        //        libgame_handle_npc_turn_lua(g, id);
        //    }
        //}
        //libgame_process_turn(g);
        //}
    }
}




void libgame_update_smoothmoves_for_entitytype(gamestate* g, const entitytype_t type) {
    const int count = libgame_lua_get_num_entities(L);
    for (int i = 0; i < count; i++) {
        //entityid id = vectorentityid_get(&g->entityids, i);
        const entityid id = libgame_lua_get_nth_entity(L, i + 1);
        const entitytype_t type2 = libgame_lua_get_entity_int(L, id, "type");
        if (type == type2) {
            libgame_update_smoothmove(g, id);
        }
        //entity_t* e = hashtable_entityid_entity_get(g->entities, id);
        //if (e && e->type == type) {
        //    libgame_update_smoothmove(g, id);
        //}
    }
}




void libgame_drawframeend(gamestate* g) {
    EndDrawing();
    g->framecount++;



    gamestate_update_current_time(g);
}




void libgame_drawframe(gamestate* g) {
    //minfo("begin libgamedrawframe");
    BeginDrawing();
    BeginTextureMode(target);
    switch (activescene) {
    case SCENE_COMPANY:
        libgame_draw_company_scene(g);
        break;
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
    //msuccess("libgame_drawframe end");
}




void libgame_calc_debugpanel_size(gamestate* g) {
    const int sz = 14;
    const int sp = 1;
    const Vector2 m = MeasureTextEx(g->font, g->debugpanel.buffer, sz, sp);
    g->debugpanel.w = m.x;
    g->debugpanel.h = m.y;
}




inline void libgame_draw_debug_panel(gamestate* g) {
    if (g && g->debugpanelon) {
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
}




void libgame_drawgrid(gamestate* g) {
    const Color c = GREEN;
    const int w = DEFAULT_TILE_SIZE;
    const int h = DEFAULT_TILE_SIZE;
    const int len = g->dungeonfloor->len;
    const int wid = g->dungeonfloor->wid;
    for (int i = 0; i <= len; i++) {
        DrawLine(i * w, 0, i * w, wid * h, c);
    }
    for (int i = 0; i <= wid; i++) {
        DrawLine(0, i * h, len * w, i * h, c);
    }
}




void libgame_draw_dungeonfloor(gamestate* g) {
    const Rectangle tile_src = {0, 0, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE};
    const int rows = libgame_lua_get_dungeonfloor_row_count(L);
    const int cols = libgame_lua_get_dungeonfloor_col_count(L);
    Rectangle tile_dest = {0, 0, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE};
    //char buf[128];
    //snprintf(buf, 128, "libgame_draw_dungeonfloor: rows: %d, cols: %d", rows, cols);
    //minfo(buf);
    const Color c = WHITE;
    const float rotation = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int type = libgame_lua_get_tiletype(L, j, i);
            int key = get_txkey_for_tiletype(type);
            if (key != -1) {
                tile_dest.x = j * DEFAULT_TILE_SIZE;
                tile_dest.y = i * DEFAULT_TILE_SIZE;
                DrawTexturePro(g->txinfo[key].texture, tile_src, tile_dest, (Vector2){0, 0}, rotation, c);
                //char buf[128];
                //snprintf(buf, 128, "libgame_draw_dungeonfloor: tiletype: %d, key: %d at x: %d, y: %d", type, key, j, i);
                //msuccess(buf);
            } else {
                char buf[128];
                snprintf(buf, 128, "libgame_draw_dungeonfloor: key not found for tile type: %d at x: %d, y: %d", type, 0, 0);
                merror(buf);
            }
        }
    }
}




void libgame_draw_entity(gamestate* g, entityid id) {
    if (g) {
        spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
        const entitytype_t type = libgame_lua_get_entity_int(L, id, "type");
        if (group) {
            const Color c = WHITE;
            const int current = group->current;

            if (type == ENTITY_SHIELD) {
                // select the proper spritegroup
                specifier_t spec = SPECIFIER_SHIELD_ON_TILE;
                spritegroup_t* group2 = hashtable_entityid_spritegroup_get_by_specifier(g->spritegroups, buckler_id, spec);
                // this could be NULL so check if
                if (!group2) {
                    //char buf[128];
                    //snprintf(buf, 128, "Failed to select spritegroup by specifier: %s", specifier_get_str(spec));
                    //merror(buf);
                } else {
                    group = group2;
                }
            }

            if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
                // draw entity shadow, which should exist at current+1 if loaded correctly
                DrawTexturePro(*group->sprites[current + 1]->texture, group->sprites[current + 1]->src, group->dest, (Vector2){0, 0}, 0.0f, c);

                // when time comes to handle blocking with a shield, we will also need to draw
                // the 'back' component of the shield first before drawing the entity
            }

            // draw the main entity sprite
            DrawTexturePro(*group->sprites[current]->texture, group->sprites[current]->src, group->dest, (Vector2){0, 0}, 0.0f, c);

            // if player or NPC and is blocking, also draw the front component of the shield
            if (type == ENTITY_PLAYER) {
                if (current == SPRITEGROUP_ANIM_HUMAN_GUARD) {
                    // to demo the buckler animating on the hero, we will need to pass in the shield id
                    // normally we would query the hero to see which shield is equipped
                    // but for now we will introduce a global buckler_id as a test

                    specifier_t spec = SPECIFIER_SHIELD_BLOCK;
                    spritegroup_t* buckler_group = hashtable_entityid_spritegroup_get_by_specifier(g->spritegroups, buckler_id, spec);
                    if (!buckler_group) {
                        merror("Failed to get buckler group");
                    } else {

                        buckler_group->dest.x = group->dest.x;
                        buckler_group->dest.y = group->dest.y;

                        DrawTexturePro(*buckler_group->sprites[buckler_group->current]->texture, buckler_group->sprites[buckler_group->current]->src, buckler_group->dest, (Vector2){0, 0}, 0.0f, c);
                    }
                }
            }


            if (g->framecount % FRAMEINTERVAL == 0) {
                sprite_incrframe(group->sprites[group->current]);
                if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
                    sprite_incrframe(group->sprites[group->current + 1]);

                    if (type == ENTITY_PLAYER && current == SPRITEGROUP_ANIM_HUMAN_GUARD) {
                        specifier_t spec = SPECIFIER_SHIELD_BLOCK;
                        spritegroup_t* buckler_group = hashtable_entityid_spritegroup_get_by_specifier(g->spritegroups, buckler_id, spec);
                        if (!buckler_group) {
                            merror("Failed to get buckler group");
                        } else {
                            sprite_incrframe(buckler_group->sprites[buckler_group->current]);
                        }
                    }
                }
            }
            if (g->debugpanelon) {
                Color c = {255, 0, 0, 255};
                const int x = group->dest.x;
                const int y = group->dest.y;
                const int w = group->dest.width;
                const int h = group->dest.height;
                // first draw the outer rectangle without the offset
                int ox = 0;
                int oy = 0;
                Vector2 v[4] = {{x - ox, y - oy}, {x + ox + w, y - oy}, {x + w + ox, y + h + oy}, {x - ox, y + h + oy}};
                //DrawLineV(v[0], v[1], c);
                //DrawLineV(v[1], v[2], c);
                //DrawLineV(v[2], v[3], c);
                //DrawLineV(v[3], v[0], c);
                // now lets draw it with the offset
                ox = group->off_x;
                oy = group->off_y;
                v[0] = (Vector2){x - ox, y - oy};
                v[1] = (Vector2){x + ox + w, y - oy};
                v[2] = (Vector2){x + w + ox, y + h + oy};
                v[3] = (Vector2){x - ox, y + h + oy};
                c = (Color){0, 0, 255, 255};
                DrawLineV(v[0], v[1], c);
                DrawLineV(v[1], v[2], c);
                DrawLineV(v[2], v[3], c);
                DrawLineV(v[3], v[0], c);
            }
        }
        //else {
        //char buf[256];
        //snprintf(buf, 256, "libgame_draw_entity: spritegroup is NULL for entity id: %d", id);
        //merror(buf);
        //}
    }
}




//void libgame_draw_items(gamestate* g, const itemtype_t type, const int x, const int y) {
//    const tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
//    for (int k = 0; k < vectorentityid_capacity(&t->entityids); k++) {
//        const entityid id = vectorentityid_get(&t->entityids, k);
//        const entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
//        if (entity->type == ENTITY_ITEM && entity->itemtype == type) {
//            libgame_draw_entity(g, id);
//        }
//    }
//}




//void libgame_draw_items_that_are_not(gamestate* g, const itemtype_t type, const int x, const int y) {
//    tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
//    for (int k = 0; k < vectorentityid_capacity(&t->entityids); k++) {
//        const entityid id = vectorentityid_get(&t->entityids, k);
//        entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
//        if (entity->type == ENTITY_ITEM && entity->itemtype != type) {
//            libgame_draw_entity(g, id);
//        }
//    }
//}




//void libgame_draw_entities_at(gamestate* g, const entitytype_t type, const int x, const int y) {
//    tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
//    if (t) {
//        for (int k = 0; k < vectorentityid_capacity(&t->entityids); k++) {
//            entityid id = vectorentityid_get(&t->entityids, k);
//            entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
//            if (entity->type == type) {
//                libgame_draw_entity(g, id);
//            }
//        }
//    }
//}



void libgame_draw_entities_at_lua(gamestate* g, const entitytype_t type, const int x, const int y) {
    //minfo("libgame_draw_entities_at_lua");
    const int num_entities = libgame_lua_get_num_entities_at(L, x, y);
    //if (num_entities > 0) {
    //    char buf[128];
    //    snprintf(buf, 128, "libgame_draw_entities_at_lua: num entities: %d", num_entities);
    //    minfo(buf);
    //}
    for (int k = 0; k < num_entities; k++) {
        //entityid id = libgame_lua_get_nth_entity_at(L, x, y, k + 1);
        const entityid id = libgame_lua_get_nth_entity_at(L, k + 1, x, y);
        const entitytype_t type2 = libgame_lua_get_entity_int(L, id, "type");
        if (type == type2) {
            //printf("type: %d\n", type);
            libgame_draw_entity(g, id);
        }
        /*
        else {
            char buf[128];
            snprintf(buf,
                     128,
                     "libgame_draw_entities_at_lua: type mismatch: k: %d, id: %d, x: %d, y: %d, type: %d, type2: %d",
                     k,
                     id,
                     x,
                     y,
                     type,
                     type2);
            merror(buf);
        }
            */
    }
    //msuccess("libgame_draw_entities_at_lua end");
}




//const bool libgame_entitytype_is_at(gamestate* g, const entitytype_t type, const int x, const int y) {
//    tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
//    if (t) {
//        for (int k = 0; k < vectorentityid_capacity(&t->entityids); k++) {
//            entity_t* entity = hashtable_entityid_entity_get(g->entities, vectorentityid_get(&t->entityids, k));
//            if (entity->type == type) {
//                return true;
//            }
//        }
//    }
//    return false;
//}




//const bool libgame_itemtype_is_at(gamestate* g, const itemtype_t type, const int x, const int y) {
//    tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
//    if (t) {
//        for (int k = 0; k < vectorentityid_capacity(&t->entityids); k++) {
//            entityid id = vectorentityid_get(&t->entityids, k);
//            entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
//            if (entity->type == ENTITY_ITEM && entity->itemtype == type) {
//                return true;
//            }
//        }
//    }
//    return false;
//}




void libgame_draw_gameplayscene_entities(gamestate* g) {
    if (g) {
        //minfo("libgame_draw_gameplayscene_entities begin");
        const int dw = libgame_lua_get_dungeonfloor_col_count(L);
        const int dh = libgame_lua_get_dungeonfloor_row_count(L);
        for (int i = 0; i < dh; i++) {
            for (int j = 0; j < dw; j++) {
                //libgame_draw_items(g, ITEM_TORCH, i, j);
                //libgame_draw_items_that_are_not(g, ITEM_TORCH, i, j);
                //libgame_draw_entities_at(g, ENTITY_NPC, i, j);
                //libgame_draw_entities_at(g, ENTITY_PLAYER, i, j);
                libgame_draw_entities_at_lua(g, ENTITY_SHIELD, i, j);
                libgame_draw_entities_at_lua(g, ENTITY_NPC, i, j);
                libgame_draw_entities_at_lua(g, ENTITY_PLAYER, i, j);
            }
        }
        //msuccess("libgame_draw_gameplayscene_entities end");
    } else {
        merror("libgame_draw_gameplayscene_entities: gamestate is NULL");
    }
}




void libgame_draw_gameplayscene(gamestate* g) {
    if (g) {
        //minfo("libgame_draw_gameplayscene begin");
        BeginMode2D(g->cam2d);
        ClearBackground(BLACK);
        // draw tiles
        libgame_draw_dungeonfloor(g);
        if (g->gridon) {
            libgame_drawgrid(g);
        }
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
        //msuccess("libgame_draw_gameplayscene end");
    } else {
        merror("libgame_draw_gameplayscene: gamestate is NULL");
    }
}




void libgame_draw_title_scene(gamestate* g) {
    const Color bgc = {0x66, 0x66, 0x66, 255};
    const Color fgc = WHITE;
    const Color fgc2 = BLACK;
    char b[128];
    char b2[128];
    char b3[128];
    snprintf(b, 128, "project");
    snprintf(b2, 128, "rpg");
    snprintf(b3, 128, "press space to continue");
    const Vector2 m = MeasureTextEx(g->font, b, 40, 2);
    const Vector2 m2 = MeasureTextEx(g->font, b2, 40, 2);
    const Vector2 m3 = MeasureTextEx(g->font, b3, 16, 1);
    const float tw2 = targetwidth / 2.0f;
    const float th2 = targetheight / 2.0f;
    const int offset = 100;
    const int x = tw2 - m.x / 2.0f - offset;
    const int y = th2 - m.y / 2.0f;
    const int x2 = tw2 - m2.x / 2.0f + offset;
    const int x3 = tw2 - m3.x / 2.0f;
    const int y3 = th2 + m3.y / 2.0f + 20;
    const Vector2 pos[3] = {{x, y}, {x2, y}, {x3, y3}};
    ClearBackground(bgc);
    DrawTextEx(g->font, b, pos[0], 40, 4, fgc);
    DrawTextEx(g->font, b2, pos[1], 40, 1, fgc2);
    // just below the 'project rpg' text
    DrawTextEx(g->font, b3, pos[2], 16, 1, fgc);
    libgame_handle_fade(g);
}




void libgame_draw_company_scene(gamestate* g) {
    const Color bgc = BLACK;
    const Color fgc = {0x66, 0x66, 0x66, 255};
    const int fontsize = 32;
    const int spacing = 1;
    const int interval = 120;
    const int dur = 60;
    //char b[128];
    //char b2[128];
    //char b3[128];
    //bzero(b, 128);
    //bzero(b2, 128);
    //bzero(b3, 128);
    //snprintf(b, 128, COMPANYNAME);
    //snprintf(b2, 128, COMPANYFILL);
    //snprintf(b3, 128, "presents");
    char b[128 * 3];
    bzero(b, 128 * 3);
    char* b2 = b + 128;
    char* b3 = b + 128 * 2;
    snprintf(b, 128, COMPANYNAME);
    snprintf(b2, 128, COMPANYFILL);
    snprintf(b3, 128, "presents");
    const Vector2 measure = MeasureTextEx(g->font, b, fontsize, spacing);
    if (g->framecount % interval >= 0 && g->framecount % interval < dur) {
        for (int i = 0; i < 10; i++) {
            shufflestrinplace(b);
            shufflestrinplace(b3);
        }
    }
    for (int i = 0; i < 10; i++) {
        shufflestrinplace(b2);
    }
    const Vector2 pos = {targetwidth / 2.0f - measure.x / 2.0f, targetheight / 2.0f - measure.y / 2.0f};
    ClearBackground(bgc);
    DrawTextEx(g->font, b, pos, fontsize, 1, fgc);
    DrawTextEx(g->font, b2, pos, fontsize, 1, fgc);
    const Vector2 measure3 = MeasureTextEx(g->font, b3, 20, 1);
    const Vector2 pp = {targetwidth / 2.0f - measure3.x / 2.0f, targetheight / 2.0f + measure.y / 2.0f + 20};
    DrawTextEx(g->font, b3, pp, 20, 1, fgc);
    libgame_handle_fade(g);
}




//void libgame_load_texture_from_data_pack(gamestate* g, img_data_pack_t* pack) {
//    if (g) {
//        // have to do dithering BEFORE loading as data
//        Image img = {
//            .data = pack->data,
//            .width = pack->width,
//            .height = pack->height,
//            .format = pack->format,
//            .mipmaps = 1,
//        };
//        Texture2D t = LoadTextureFromImage(img);
//        g->txinfo[pack->index].texture = t;
//        g->txinfo[pack->index].num_frames = pack->frames;
//        g->txinfo[pack->index].contexts = pack->contexts;
//        msuccess("libgame_load_texture_from_data");
//    } else {
//        merror("libgame_loadtexture: gamestate is NULL");
//    }
//}




void libgame_load_texture_from_disk(gamestate* g, int index, int contexts, int frames, bool dodither, const char* path) {
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
    } else {
        merror("libgame_loadtexture: gamestate is NULL");
    }
    //SetTextureFilter(textures[index], TEXTURE_FILTER_POINT);
    //SetTextureFilter(txinfo[index].texture, TEXTURE_FILTER_POINT);
}




void libgame_load_textures_from_disk(gamestate* g) {
    const char* texture_file_path = "textures.txt";
    FILE* fp = fopen(texture_file_path, "r");
    if (fp) {
        char line[512];
        char path[256];
        bzero(line, 512);
        bzero(path, 256);
        while (fgets(line, 512, fp)) {
            // if line begins with #, skip it
            if (line[0] == '#') {
                continue;
            }
            // if line is empty, skip it
            if (strlen(line) == 0) {
                continue;
            }
            // parse the line
            int index = -1;
            int contexts = -1;
            int frames = -1;
            int dodither = false;
            sscanf(line, "%d %d %d %d %s", &index, &contexts, &frames, &dodither, path);
            libgame_load_texture_from_disk(g, index, contexts, frames, dodither, path);
            bzero(line, 512);
            bzero(path, 256);
        }

        fclose(fp);
        msuccess("libgame_loadtextures_from_disk");
    } else {
        merror("libgame_loadtextures_from_disk: could not open file");
    }
}




void libgame_load_textures(gamestate* g) {
    if (g) {
        //libgame_load_textures_from_data(g);
        libgame_load_textures_from_disk(g);


        msuccess("libgame_loadtextures");
    } else {
        merror("libgame_loadtextures: gamestate is NULL");
    }
}




//void libgame_load_textures_from_data(gamestate* g) {
//    if (g) {
//#define NUM_PACKS (44)
//        img_data_pack_t packs[NUM_PACKS] = {
//            // 3+14+1 for tile
//            PK_TILE_DIRT_00,
//            PK_TILE_DIRT_01,
//            PK_TILE_DIRT_02,
//            PK_TILE_STONE_00,
//            PK_TILE_STONE_01,
//            PK_TILE_STONE_02,
//            PK_TILE_STONE_03,
//            PK_TILE_STONE_04,
//            PK_TILE_STONE_05,
//            PK_TILE_STONE_06,
//            PK_TILE_STONE_07,
//            PK_TILE_STONE_08,
//            PK_TILE_STONE_09,
//            PK_TILE_STONE_10,
//            PK_TILE_STONE_11,
//            PK_TILE_STONE_12,
//            PK_TILE_STONE_13,
//            PK_TILE_STONEWALL_00,
//            // 12 for human
//            PK_HUMAN_IDLE,
//            PK_HUMAN_IDLE_SHADOW,
//            PK_HUMAN_WALK,
//            PK_HUMAN_WALK_SHADOW,
//            PK_HUMAN_ATTACK,
//            PK_HUMAN_ATTACK_SHADOW,
//            PK_HUMAN_JUMP,
//            PK_HUMAN_JUMP_SHADOW,
//            PK_HUMAN_SPIN_DIE,
//            PK_HUMAN_SPIN_DIE_SHADOW,
//            PK_HUMAN_SOUL_DIE,
//            PK_HUMAN_SOUL_DIE_SHADOW,
//            // 14 for orc
//            PK_ORC_IDLE,
//            PK_ORC_IDLE_SHADOW,
//            PK_ORC_WALK,
//            PK_ORC_WALK_SHADOW,
//            PK_ORC_ATTACK,
//            PK_ORC_ATTACK_SHADOW,
//            PK_ORC_CHARGED_ATTACK,
//            PK_ORC_CHARGED_ATTACK_SHADOW,
//            PK_ORC_JUMP,
//            PK_ORC_JUMP_SHADOW,
//            PK_ORC_DIE,
//            PK_ORC_DIE_SHADOW,
//            PK_ORC_DMG,
//            PK_ORC_DMG_SHADOW,
//        };
//        for (int i = 0; i < NUM_PACKS; i++) {
//            libgame_load_texture_from_data_pack(g, &packs[i]);
//        }
//        msuccess("libgame_loadtextures_from_data");
//    } else {
//        merror("libgame_loadtextures_from_data: gamestate is NULL");
//    }
//}




void libgame_unloadtexture(gamestate* g, int index) {
    if (g) {
        if (g->txinfo[index].texture.id > 0) {
            UnloadTexture(g->txinfo[index].texture);
            //msuccess("unloading texture");
        }
    } else {
        merror("libgame_unloadtexture: gamestate is NULL");
    }
}




void libgame_unloadtextures(gamestate* g) {
    minfo("unloading textures");
    // this can be done smarter, surely...
    //for (int i = TX_DIRT_00; i <= TX_HERO_SOUL_DIE_SHADOW; i++) {
    for (int i = TX_DIRT_00; i <= TX_ORC_DIE_SHADOW; i++) {
        libgame_unloadtexture(g, i);
    }
}




//void libgame_reloadtextures(gamestate* g) {
//    libgame_unloadtextures(g);
//    libgame_loadtextures(g);
//}




void libgame_init() {
    //minfo("libgame_init");
    g = gamestateinitptr(windowwidth, windowheight, targetwidth, targetheight);
    libgame_initsharedsetup(g);
    msuccess("libgame_init");
}




//const entityid libgame_create_entity(gamestate* g, const char* name, const entitytype_t type, const int x, const int y) {
//    entity_t* e = entity_create(name);
//    if (!e) {
//        merror("could not create entity");
//        return -1;
//    }
//    e->x = x;
//    e->y = y;
//    e->type = type;
//    e->inventory = vectorentityid_new();
//    vectorentityid_add(&g->entityids, e->id);
//    tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
//    vectorentityid_add(&t->entityids, e->id);
//    hashtable_entityid_entity_insert(g->entities, e->id, e);
//    return e->id;
//}




//void libgame_create_torch_spritegroup(gamestate* g, entityid id, const float off_x, const float off_y) {
//    minfo("libgame_createtorchspritegroup begin");
//    spritegroup_t* group = spritegroup_create(4);
//    entity_t* torch_entity = hashtable_entityid_entity_get(g->entities, id);
//    int keys[1] = {TXTORCH};
//    for (int i = 0; i < 1; i++) {
//        sprite* s = sprite_create(&g->txinfo[keys[i]].texture, g->txinfo[keys[i]].contexts, g->txinfo[keys[i]].num_frames);
//        if (!s) {
//            merror("could not create sprite");
//        }
//        spritegroup_add(group, s);
//    }
//    const int tilesize = 8;
//    const sprite* s = spritegroup_get(group, 0);
//    const float ox = off_x;
//    const float oy = off_y;
//    const float x = torch_entity->x * tilesize + ox;
//    const float y = torch_entity->y * tilesize + oy;
//    Rectangle dest = {x, y, s->width, s->height};
//    group->current = 0;
//    group->dest = dest;
//    hashtable_entityid_spritegroup_insert(g->spritegroups, id, group);
//    minfo("libgame_createtorchspritegroup end");
//}




// this code is ugly as fuck but it works
// the hero has a LOT of spritegroups
// not every entity will have this many sprites
// lets try using this as a basis to get a sprite in there
//void libgame_create_herospritegroup(gamestate* g, entityid id) {
//    minfo("libgame_create_herospritegroup begin");
//    minfo("creating hero group...");
//    spritegroup_t* hero_group = spritegroup_create(20);
//    minfo("getting hero entity x and y...");
//    const int hx = libgame_lua_get_entity_int(L, id, "x");
//    const int hy = libgame_lua_get_entity_int(L, id, "y");
//    const int dw = libgame_lua_get_dungeonfloor_col_count(L);
//    const int dh = libgame_lua_get_dungeonfloor_row_count(L);
//    msuccess("got x and y");
//    if (hx < 0 || hx > dw || hy < 0 || hy > dh) {
//        char buf[128];
//        snprintf(buf, 128, "Could not create spritegroup, x: %d, y: %d, dw: %d, dh: %d", hx, hy, dw, dh);
//        merror(buf);
//        return;
//    }
//#define NUM_HERO_KEYS 12
//    minfo("Preparing to enum hero keys...");
//    const int keys[NUM_HERO_KEYS] = {TX_HERO_IDLE,
//                                     TX_HERO_IDLE_SHADOW,
//                                     TX_HERO_WALK,
//                                     TX_HERO_WALK_SHADOW,
//                                     TX_HERO_ATTACK,
//                                     TX_HERO_ATTACK_SHADOW,
//                                     TX_HERO_JUMP,
//                                     TX_HERO_JUMP_SHADOW,
//                                     TX_HERO_SPIN_DIE,
//                                     TX_HERO_SPIN_DIE_SHADOW,
//                                     TX_HERO_SOUL_DIE,
//                                     TX_HERO_SOUL_DIE_SHADOW};
//    minfo("looping thru sprite keys...");
//    for (int i = 0; i < NUM_HERO_KEYS; i++) {
//        const int txkey = keys[i];
//        const int ctxs = g->txinfo[txkey].contexts;
//        const int frames = g->txinfo[txkey].num_frames;
//        //char buf[128];
//        //snprintf(buf, 128, "i: %d, txkey: %d, ctxs: %d, frames: %d", i, txkey, ctxs, frames);
//        //minfo(buf);
//        sprite* s = sprite_create(&g->txinfo[txkey].texture, ctxs, frames);
//        if (!s) {
//            merror("could not create sprite");
//        }
//        spritegroup_add(hero_group, s);
//    }
//    msuccess("hero sprites added to sprite group");
//    // this is effectively how we will update the
//    // sprite position in relation to the entity's
//    // dungeon position
//    const float w = spritegroup_get(hero_group, 0)->width;
//    const float h = spritegroup_get(hero_group, 0)->height;
//    const float offset_x = -12; // this is hard-coded for now...
//    const float offset_y = -12; // this is hard-coded for now...
//    const float x = hx * DEFAULT_TILE_SIZE;
//    const float y = hy * DEFAULT_TILE_SIZE;
//    hero_group->current = 0;
//    hero_group->dest = (Rectangle){x + offset_x, y + offset_y, w, h};
//    hero_group->off_x = offset_x;
//    hero_group->off_y = offset_y;
//    // add the spritegroup to the hashtable
//    minfo("inserting hero spritegroup into table...");
//    //hashtable_entityid_spritegroup_insert(g->spritegroups, g->hero_id, hero_group);
//    hashtable_entityid_spritegroup_insert(g->spritegroups, id, hero_group);
//    msuccess("libgame_create_herospritegroup end");
//}


void libgame_create_spritegroup_by_id(gamestate* g, entityid id) {
    if (id != -1) {
        // want to pre-select values to feed into create_spritegroup by
        // entity-type
        //    if player or NPC, by race
        //    if item, by itemtype and specs
        //
        //libgame_create_spritegroup(g, id, TX_HERO_KEYS, TX_HERO_KEY_COUNT, -12, -12);
        const entitytype_t entity_type = libgame_lua_get_entity_int(L, id, "type");
        int* keys = NULL;
        int num_keys = 0;
        int offset_x = 0;
        int offset_y = 0;
        int default_anim = 0;
        specifier_t specifier = SPECIFIER_NONE;

        if (entity_type == ENTITY_PLAYER || entity_type == ENTITY_NPC) {
            race_t race = libgame_lua_get_entity_int(L, id, "race");
            if (race == RACE_HUMAN) {
                keys = TX_HUMAN_KEYS;
                num_keys = TX_HUMAN_KEY_COUNT;
                offset_x = -12;
                offset_y = -12;
                default_anim = SPRITEGROUP_ANIM_HUMAN_IDLE;
                specifier = SPECIFIER_NONE;
                libgame_create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, specifier);
                libgame_set_default_anim_for_id(g, id, default_anim);
            } else if (race == RACE_ORC) {
                keys = TX_ORC_KEYS;
                num_keys = TX_ORC_KEY_COUNT;
                offset_x = -12;
                offset_y = -12;
                default_anim = SPRITEGROUP_ANIM_ORC_IDLE;
                specifier = SPECIFIER_NONE;
                libgame_create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, specifier);
                libgame_set_default_anim_for_id(g, id, default_anim);
            }
            //else if (race == RACE_NONE) {
            //}
        } else if (entity_type == ENTITY_SHIELD) {
            // first we will create the spritegroup for the dungeon-item representation
            // then we will create the spritegroups for being equipped on the player
            // they will get attached in the hashtable at the same location via chaining
            // we will select the proper sprite during rendering by attaching filters to the spritegroups

            keys = TX_BUCKLER_KEYS;
            num_keys = TX_BUCKLER_KEY_COUNT;
            offset_x = 0;
            offset_y = 0;
            specifier = SPECIFIER_SHIELD_ON_TILE;
            //default_anim = SPRITEGROUP_ANIM_ORC_IDLE;
            libgame_create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, specifier);
            //libgame_set_default_anim_for_id(g, id, default_anim);

            keys = TX_GUARD_BUCKLER_KEYS;
            num_keys = TX_GUARD_BUCKLER_KEY_COUNT;
            offset_x = -10;
            offset_y = -14;
            default_anim = SPRITEGROUP_ANIM_GUARD_BUCKLER;
            specifier = SPECIFIER_SHIELD_BLOCK;
            libgame_create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, specifier);
            libgame_set_default_anim_for_id(g, id, default_anim);

            keys = TX_GUARD_BUCKLER_SUCCESS_KEYS;
            num_keys = TX_GUARD_BUCKLER_SUCCESS_KEY_COUNT;
            offset_x = -12;
            offset_y = -12;
            default_anim = SPRITEGROUP_ANIM_GUARD_BUCKLER;
            specifier = SPECIFIER_SHIELD_BLOCK_SUCCESS;
            libgame_create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, specifier);
            libgame_set_default_anim_for_id(g, id, default_anim);
        }

        if (keys == NULL) {
            char buf[128];
            snprintf(buf, 128, "Could not select keys for create_spritegroup_by_id, id: %d, entity_type: %d", id, entity_type);
            merror(buf);
        }
    }
}




void libgame_set_default_anim_for_id(gamestate* g, entityid id, int anim) {
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (group) {
        group->default_anim = anim;
    }
}




void libgame_create_spritegroup(gamestate* g, entityid id, int* keys, int num_keys, int offset_x, int offset_y, specifier_t spec) {
    minfo("libgame_create_spritegroup begin");
    minfo("creating sprite group...");
    spritegroup_t* group = spritegroup_create(20);
    minfo("getting entity x and y...");
    const int x = libgame_lua_get_entity_int(L, id, "x");
    const int y = libgame_lua_get_entity_int(L, id, "y");
    const int dw = libgame_lua_get_dungeonfloor_col_count(L);
    const int dh = libgame_lua_get_dungeonfloor_row_count(L);
    msuccess("got x and y");
    if (x < 0 || x > dw || y < 0 || y > dh) {
        char buf[128];
        snprintf(buf, 128, "Could not create spritegroup, x: %d, y: %d, dw: %d, dh: %d", x, y, dw, dh);
        merror(buf);
        return;
    }
    minfo("looping thru sprite keys...");
    for (int i = 0; i < num_keys; i++) {
        const int txkey = keys[i];
        const int ctxs = g->txinfo[txkey].contexts;
        const int frames = g->txinfo[txkey].num_frames;
        //char buf[128];
        //snprintf(buf, 128, "i: %d, txkey: %d, ctxs: %d, frames: %d", i, txkey, ctxs, frames);
        //minfo(buf);
        sprite* s = sprite_create(&g->txinfo[txkey].texture, ctxs, frames);
        if (!s) {
            merror("could not create sprite");
        } else {
            msuccess("sprite created successfully!");
        }
        spritegroup_add(group, s);
    }
    msuccess("sprites added to sprite group");
    // this is effectively how we will update the
    // sprite position in relation to the entity's
    // dungeon position
    const float w = spritegroup_get(group, 0)->width;
    const float h = spritegroup_get(group, 0)->height;
    //const float offset_x = -12; // this is hard-coded for now...
    //const float offset_y = -12; // this is hard-coded for now...
    const float dst_x = x * DEFAULT_TILE_SIZE;
    const float dst_y = y * DEFAULT_TILE_SIZE;
    group->current = 0;
    group->dest = (Rectangle){dst_x + offset_x, dst_y + offset_y, w, h};
    group->off_x = offset_x;
    group->off_y = offset_y;
    spritegroup_set_specifier(group, spec);

    // add the spritegroup to the hashtable
    char buf[128];
    snprintf(buf, 128, "inserting spritegroup into table with id: %d and spec: %s", id, specifier_get_str(spec));
    minfo(buf);

    hashtable_entityid_spritegroup_insert(g->spritegroups, id, group);
    msuccess("libgame_create_spritegroup end");
}




//void libgame_create_sword_spritegroup(gamestate* g, entityid id, const float off_x, const float off_y) {
//    minfo("libgame_create_sword_spritegroup begin");
//    spritegroup_t* group = spritegroup_create(4);
//    entity_t* e = hashtable_entityid_entity_get(g->entities, id);
//    int keys[1] = {TXSWORD};
//    for (int i = 0; i < 1; i++) {
//        sprite* s = sprite_create(&g->txinfo[keys[i]].texture, g->txinfo[keys[i]].contexts, g->txinfo[keys[i]].num_frames);
//        if (!s) {
//            merror("could not create sprite");
//        }
//        spritegroup_add(group, s);
//    }
// the padding will be different for the torch
// initialize the group current and dest
//    const sprite* s = spritegroup_get(group, 0);
//    const float x = e->x * DEFAULT_TILE_SIZE + off_x;
//    const float y = e->y * DEFAULT_TILE_SIZE + off_y;
//    Rectangle dest = {x, y, s->width, s->height};
//    group->current = 0;
//    group->dest = dest;
//    group->off_x = off_x;
//    group->off_y = off_y;
// add the spritegroup to the hashtable
//    hashtable_entityid_spritegroup_insert(g->spritegroups, id, group);
//    msuccess("libgame_create_sword_spritegroup end");
//}




//void libgame_create_shield_spritegroup(gamestate* g, const entityid id, const int off_x, const int off_y) {
//    minfo("libgame_create_shield_spritegroup begin");
//    if (g) {
//        spritegroup_t* group = spritegroup_create(4);
//        if (!group) {
//            merror("create shield spritegroup: could not create spritegroup");
//            return;
//        }
//        entity_t* e = hashtable_entityid_entity_get(g->entities, id);
//        if (!e) {
//            merror("create shield spritegroup: could not get entity");
//            return;
//        }
//        sprite* s = sprite_create(&g->txinfo[TXSHIELD].texture, g->txinfo[TXSHIELD].contexts, g->txinfo[TXSHIELD].num_frames);
//        if (!s) {
//            merror("could not create sprite");
//        }
//        spritegroup_add(group, s);
//        const int x = e->x * DEFAULT_TILE_SIZE + off_x;
//        const int y = e->y * DEFAULT_TILE_SIZE + off_y;
//        Rectangle dest = {x, y, s->width, s->height};
//        group->current = 0;
//        group->dest = dest;
//        group->off_x = off_x;
//        group->off_y = off_y;
//        hashtable_entityid_spritegroup_insert(g->spritegroups, id, group);
//        msuccess("libgame_create_shield_spritegroup end");
//    }
//}




void libgame_loadtargettexture(gamestate* g) {
    if (g) {
        //targetwidth = libgame_lua_get_int(L, "TargetWidth");
        //targetheight = libgame_lua_get_int(L, "TargetHeight");
        targetwidth = libgame_lua_get_gamestate_int(L, "TargetWidth");
        targetheight = libgame_lua_get_gamestate_int(L, "TargetHeight");
        target = LoadRenderTexture(targetwidth, targetheight);
        target_src = (Rectangle){0, 0, targetwidth, -targetheight};
        target_dest = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};
        SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
        // update the gamestate display values
        g->display.targetwidth = targetwidth;
        g->display.targetheight = targetheight;
        // can get this from init.lua....
        g->cam2d.offset.x = targetwidth / 2.0f;
        g->cam2d.offset.y = targetheight / 4.0f;
    } else {
        merror("could not load target texture: gamestate is NULL");
    }
}




void libgame_loadfont(gamestate* g) {
    if (g) {
        const int fontsize = 60;
        const int codepointct = 256;
        g->font = LoadFontEx(DEFAULT_FONT_PATH, fontsize, 0, codepointct);
    } else {
        merror("could not load font: gamestate is NULL");
    }
}




void libgame_init_datastructures(gamestate* g) {
    g->entityids = vectorentityid_create(DEFAULT_VECTOR_ENTITYID_SIZE);
    //g->entities = hashtable_entityid_entity_create(DEFAULT_HASHTABLE_ENTITYID_ENTITY_SIZE);
    g->spritegroups = hashtable_entityid_spritegroup_create(DEFAULT_HASHTABLE_ENTITYID_SPRITEGROUP_SIZE);
    const tiletype_t base_type = TILETYPE_DIRT_00;
    const int w = 16;
    const int h = 16;
    g->dungeonfloor = create_dungeonfloor(w, h, base_type);
    if (!g->dungeonfloor) {
        merror("could not create dungeonfloor");
        // we could use an 'emergency shutdown' in case an error causes us
        // to need to 'panic' and force game close properly
    }
    libgame_lua_create_dungeonfloor(L, w, h, base_type);
    // lets try setting some random tiles to different tile types
    //libgame_init_dungeonfloor(g);
    //minfo("libgame_initdatastructures end");
}




//void libgame_createitembytype(gamestate* g, const itemtype_t type, const int x, const int y) {
//    minfo("libgame_createitembytype begin");
//    switch (type) {
//    case ITEM_TORCH: {
//        const entityid torch_id = libgame_create_entity(g, "torch", ENTITY_ITEM, x, y);
//        entity_t* torch = hashtable_entityid_entity_get(g->entities, torch_id);
//        if (torch) {
//            msuccess("torch entity created");
//            torch->itemtype = ITEM_TORCH;
//            libgame_create_torch_spritegroup(g, torch_id, 0, -DEFAULT_TILE_SIZE);
//        }
//    } break;
//    default:
//        break;
//    }
//    minfo("libgame_createitembytype end");
//}




void libgame_create_hero_lua(gamestate* g, const char* name, const int x, const int y) {
    minfo("libgame_create_hero_lua begin");
    const entitytype_t entity_type = ENTITY_PLAYER;
    const entityid id = libgame_lua_create_entity(L, name, entity_type, x, y);
    if (id != -1) {
        libgame_lua_set_gamestate_int(L, "HeroId", id);
        // set race type
        race_t race = RACE_HUMAN;
        //direction_t dir = DIRECTION_DOWN_LEFT;
        direction_t dir = DIRECTION_DOWN_RIGHT;
        libgame_lua_set_entity_int(L, id, "race", race);
        libgame_lua_set_entity_int(L, id, "direction", dir);
        libgame_create_spritegroup_by_id(g, id);
        libgame_update_spritegroup(g, id, dir);
        //char buf[128];
        //snprintf(buf, 128, "libgame_create_hero_lua: hero entityid %d", id);
        //msuccess(buf);
    } else {
        merror("libgame_create_hero_lua: could not create hero entity, expect crashes");
        libgame_lua_set_gamestate_int(L, "HeroId", -1);
    }
}




const entityid libgame_create_buckler_lua(gamestate* g, const char* name, const int x, const int y) {
    char buf[128];
    snprintf(buf, 128, "libgame_create_buckler_lua: creating buckler entity %s at %d, %d", name, x, y);
    minfo(buf);
    if (!g) {
        merror("libgame_create_buckler_lua: gamestate is NULL");
        return -1;
    }
    if (!name) {
        merror("libgame_create_buckler_lua: name is NULL");
        return -1;
    }
    if (x < 0 || y < 0) {
        merror("libgame_create_buckler_lua: x or y is less than 0");
        return -1;
    }
    const int dw = libgame_lua_get_dungeonfloor_col_count(L);
    const int dh = libgame_lua_get_dungeonfloor_row_count(L);
    if (x >= dw || y >= dh) {
        merror("libgame_create_buckler_lua: x or y is greater than dungeonfloor dimensions");
        return -1;
    }
    const entityid id = libgame_lua_create_entity(L, name, ENTITY_SHIELD, x, y);
    if (id != -1) {
        // set buckler race
        libgame_lua_set_entity_int(L, id, "race", RACE_NONE);
        libgame_create_spritegroup_by_id(g, id);
        bzero(buf, 128);
        snprintf(buf, 128, "libgame_create_buckler_lua: buckler entityid %d", id);
        msuccess(buf);
    } else {
        merror("libgame_create_buckler_lua: could not create buckler entity");
    }
    return id;
}




const entityid libgame_create_orc_lua(gamestate* g, const char* name, const int x, const int y) {
    char buf[128];
    snprintf(buf, 128, "libgame_create_orc_lua: creating orc entity %s at %d, %d", name, x, y);
    minfo(buf);
    if (!g) {
        merror("libgame_create_orc_lua: gamestate is NULL");
        return -1;
    }
    if (!name) {
        merror("libgame_create_orc_lua: name is NULL");
        return -1;
    }
    if (x < 0 || y < 0) {
        merror("libgame_create_orc_lua: x or y is less than 0");
        return -1;
    }
    const int dw = libgame_lua_get_dungeonfloor_col_count(L);
    const int dh = libgame_lua_get_dungeonfloor_row_count(L);
    if (x >= dw || y >= dh) {
        merror("libgame_create_orc_lua: x or y is greater than dungeonfloor dimensions");
        return -1;
    }
    const entityid id = libgame_lua_create_entity(L, name, ENTITY_NPC, x, y);
    if (id != -1) {
        // set orc race
        libgame_lua_set_entity_int(L, id, "race", RACE_ORC);
        libgame_create_spritegroup_by_id(g, id);
        bzero(buf, 128);
        snprintf(buf, 128, "libgame_create_orc_lua: orc entityid %d", id);
        msuccess(buf);
    } else {
        merror("libgame_create_orc_lua: could not create orc entity");
    }
    return id;
}



//void libgame_create_sword(gamestate* g, const char* name, const int x, const int y) {
//    const entityid id = libgame_create_entity(g, name, ENTITY_ITEM, x, y);
//    if (id != -1) {
//        entity_t* e = hashtable_entityid_entity_get(g->entities, id);
//        if (e) {
//            minfo("orc entity created");
//            e->itemtype = ITEM_WEAPON;
//            e->weapontype = WEAPON_SWORD;
//            libgame_create_sword_spritegroup(g, id, 0, -2);
//        }
//    }
//}




//void libgame_create_shield(gamestate* g, const char* name, const int x, const int y) {
//    const entityid id = libgame_create_entity(g, name, ENTITY_ITEM, x, y);
//    if (id != -1) {
//        entity_t* e = hashtable_entityid_entity_get(g->entities, id);
//        if (e) {
//            minfo("shield entity created");
//            e->itemtype = ITEM_SHIELD;
//            e->shieldtype = SHIELD_BASIC;
//            libgame_create_shield_spritegroup(g, id, 0, -2);
//        }
//    }
//}




void libgame_initsharedsetup(gamestate* g) {
    minfo("libgame_initsharedsetup begin");
    if (g) {
        // init lua
        L = luaL_newstate();
        luaL_openlibs(L);
        const char* filename = "init.lua";
        if (luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0)) {
            luaL_error(L, "cannot run %s: %s", filename, lua_tostring(L, -1));
        }
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

        libgame_loadfont(g);
        libgame_loadtargettexture(g);
        libgame_load_textures(g);
        libgame_init_datastructures(g);
        //minfo("creating hero");
        //libgame_create_hero_lua(g, "hero", 1, 0);
        //if (g->hero_id == -1) {
        libgame_create_hero_lua(g, "hero", 1, 1);
        // keeping track of buckler_id to test buckler drawing on the entity
        buckler_id = libgame_create_buckler_lua(g, "buckler", 2, 1);
        //}
        //msuccess("hero created");
        //minfo("creating sword...");
        //libgame_create_sword(g, "sword", 2, 0);
        //msuccess("sword created");
        //minfo("creating shield...");
        //libgame_create_shield(g, "shield", 3, 0);
        //msuccess("shield created");
        //for (int i = 0; i < g->dungeonfloor->wid; i++) {
        //    for (int j = 0; j < g->dungeonfloor->len; j++) {
        //        if (rand() % 16 == 0) {
        //            char tmp[32] = {0};
        //            snprintf(tmp, 32, "orc%d", i * g->dungeonfloor->wid + j);
        //            libgame_create_orc(g, tmp, (Vector2){i, j});
        //        }
        //    }
        //}
        //libgame_create_orc(g, "orc1", (Vector2){2, 2});
        //libgame_create_orc(g, "orc2", (Vector2){2, 3});
        //libgame_create_orc(g, "orc3", (Vector2){3, 3});
        //libgame_create_orc(g, "orc4", (Vector2){1, 3});
        //libgame_create_orc(g, "orc5", (Vector2){1, 2});
        // these dont work right until the text buffer of the debugpanel is filled
        libgame_update_debug_panel_buffer(g);
        libgame_calc_debugpanel_size(g);
        setdebugpanelbottomleft(g);
    } else {
        merror("libgame_initsharedsetup: gamestate is NULL");
    }
    msuccess("libgame_initsharedsetup end");
}




void libgame_initwithstate(gamestate* state) {
    if (state == NULL) {
        merror("libgame_initwithstate: state is NULL");
        return;
    }
    g = state;
    libgame_initsharedsetup(g);
    msuccess("libgame_initwithstate end");
}




void libgame_closesavegamestate() {
    minfo("libgame_closesavegamestate");
    libgame_closeshared(g);
    msuccess("libgame_closesavegamestate end");
}




void libgame_close(gamestate* g) {
    minfo("libgame_close");
    libgame_closeshared(g);
    gamestatefree(g);
    msuccess("libgame_close end");
}




void libgame_closeshared(gamestate* g) {
    // dont need to free most of gamestate
    minfo("libgame_closeshared");

    //UnloadMusicStream(test_music);
    //CloseAudioDevice();

    UnloadFont(g->font);

    libgame_unloadtextures(g);

    UnloadRenderTexture(target);

    CloseWindow();

    lua_close(L);

    msuccess("libgame_closeshared end");
}




gamestate* libgame_getgamestate() {
    return g;
}




const bool libgame_external_check_reload() {
    return IsKeyPressed(KEY_R);
}
