#include "controlmode.h"
#include "dungeonfloor.h"
#include "entitytype.h"
#include "fadestate.h"
#include "gamestate.h"
#include "get_txkey_for_tiletype.h"
//#include "hashtable_entityid_entity.h"
#include "hashtable_entityid_spritegroup.h"
#include "libgame.h"
#include "libgame_defines.h"
#include "mprint.h"
#include "race.h"
#include "scene.h"
#include "setdebugpanel.h"
#include "sprite.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"
#include "textureinfo.h"
#include "tiletype.h"
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

// Data packing
#include "img_data_packs.h"
#include "tx_keys.h"

//old, may not be needed
//#include "img_data.h"
//#include "itemtype.h"
//#include "entity.h"
//#include "race.h"

//------------------------------------------------------------------
// libgame global variables
//------------------------------------------------------------------
gamestate* g = NULL;

//#ifndef WEB
lua_State* L = NULL;
//#endif

RenderTexture target;
Rectangle target_src = (Rectangle){0, 0, 0, 0};
Rectangle target_dest = (Rectangle){0, 0, 0, 0};
Vector2 target_origin = (Vector2){0, 0};


int activescene = GAMEPLAYSCENE;
//int activescene = COMPANYSCENE;
int targetwidth = -1;
int targetheight = -1;
//int targetwidth = DEFAULT_TARGET_WIDTH;
//int targetheight = DEFAULT_TARGET_HEIGHT;
//int windowwidth = DEFAULT_WINDOW_WIDTH;
//int windowheight = DEFAULT_WINDOW_HEIGHT;
int windowwidth = -1;
int windowheight = -1;


//const bool libgame_entity_try_attack(gamestate* g, entityid id, const int x, const int y) {
//    // we dont have full rules or functions for attacking yet
//    // however
//    // given the entityid and the position they are attack
//    minfo("try_attack: starting...");
//    entity_t* e = hashtable_entityid_entity_get(g->entities, id);
//    if (e) {
//        // get the tile at pos
//        //tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, pos);
//        tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
//        if (t) {
//            // check to see if tile has anything
//            const bool occupied = vectorentityid_capacity(&t->entityids) > 0;
//            if (occupied) {
//                // get the entityids at that tile
//                for (int i = 0; i < vectorentityid_capacity(&t->entityids); i++) {
//                    entityid id2 = vectorentityid_get(&t->entityids, i);
//                    entity_t* e2 = hashtable_entityid_entity_get(g->entities, id2);
//                    if (e2) {
//                        entitytype_t type = e2->type;
//                        // check "all" the types that can be attacked
//                        if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
//                            msuccess("attack attempt was successful");
//                            // at this point we would process the rules for attacking
//                            // i.e. entity rolls a 1d20 + attack bonus versus target's AC
//                            // if the roll is greater than the target's AC, then the attack hits
//                            // and we roll damage
//                            return true;
//                        }
//                    }
//                }
//            } else {
//                merror("try_attack: tile is empty");
//                return false;
//            }
//        } else {
//            // attacking out-of-bounds or something else
//            merror("try_attack: tile is NULL");
//            return false;
//        }
//    } else {
//        merror("try_attack: entity is NULL");
//    }
//    return false;
//}




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




//void libgame_entity_anim_incr(gamestate* g, entityid id) {
//    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
//    if (group) {
//        spritegroup_incr(group);
//    }
//}




void libgame_entity_anim_set(gamestate* g, entityid id, int index) {
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (group) {
        spritegroup_set_current(group, index);
    }
}




//const bool libgame_entity_inventory_contains_type(gamestate* g, entityid id, itemtype_t type) {
//    entity_t* e = hashtable_entityid_entity_get(g->entities, id);
//    if (e) {
//        for (int i = 0; i < vectorentityid_capacity(&e->inventory); i++) {
//            entityid id = vectorentityid_get(&e->inventory, i);
//            entity_t* item = hashtable_entityid_entity_get(g->entities, id);
//            if (item && item->itemtype == type) {
//                return true;
//            }
//        }
//    }
//    return false;
//}




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




//void libgame_handle_player_attack(gamestate* g) {
//    if (libgame_entity_inventory_contains_type(g, g->hero_id, ITEM_WEAPON)) {
//        entity_t* hero = hashtable_entityid_entity_get(g->entities, g->hero_id);
//        if (hero) {
//            if (libgame_entity_try_attack(g, g->hero_id, hero->x + 1, hero->y)) {
//                tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, hero->x + 1, hero->y);
//                entityid orc_id = -1;
//                for (int i = 0; i < vectorentityid_capacity(&t->entityids); i++) {
//                    entityid id = vectorentityid_get(&t->entityids, i);
//                    entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
//                    if (entity->type == ENTITY_NPC) {
//                        orc_id = id;
//                        break;
//                    }
//                }
//                libgame_entity_anim_set(g, orc_id, 10);
//                msuccess("attack success!");
//            } else {
//                merror("attack failed...");
//            }
//            // in any case, set the attack animation
//            libgame_entity_anim_set(g, g->hero_id, SPRITEGROUP_ANIM_HUMAN_ATTACK);
//        }
//    } else {
//        libgame_entity_anim_set(g, g->hero_id, SPRITEGROUP_ANIM_HUMAN_IDLE);
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

    if (IsKeyPressed(KEY_A)) {

        entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
        if (hero_id != -1) {
            spritegroup_t* grp = hashtable_entityid_spritegroup_get(g->spritegroups, hero_id);
            if (grp) {
                spritegroup_incr(grp);
            }
        }



        // lets cycle the animation for the hero
        //libgame_entity_anim_incr(g, g->hero_id);

        // technically we dont want to be able to attack until we have picked up a weapon...



        //libgame_handle_player_attack(g);
        //g->player_input_received = true;
    }

    if (IsKeyPressed(KEY_E)) {
        libgame_test_enemy_placement(g);
        //    g->player_input_received = true;
    }


    //if (IsKeyPressed(KEY_P)) {
    //    char buf[128];
    //    snprintf(buf, 128, "hero_id: %d", g->hero_id);
    //    msuccess(buf);
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
        switch (g->controlmode) {
        case CONTROLMODE_CAMERA:
            g->controlmode = CONTROLMODE_PLAYER;
            break;
        default:
            g->controlmode = CONTROLMODE_CAMERA;
            break;
        }
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
        int ctx = group->sprites[group->current]->currentcontext;
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

        default:
            break;
        }
        spritegroup_setcontexts(group, ctx);
        spritegroup_set_current(group, SPRITEGROUP_ANIM_HUMAN_WALK);
        //group->current = SPRITEGROUP_ANIM_HUMAN_WALK;
        //libgame_entity_anim_set(g, id, SPRITEGROUP_ANIM_HUMAN_WALK);
    }
}




void libgame_update_spritegroup_move(gamestate* g, entityid id, int x, int y) {
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (sg) {
        sg->move_x += x;
        sg->move_y += y;
    }
}




void libgame_handleplayerinput_move(gamestate* g, int xdir, int ydir) {
    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
    if (hero_id != -1) {
        libgame_lua_create_action(L, hero_id, 2, xdir, ydir);
    } else {
        merror("handleplayerinput_move: hero_id is -1");
    }
}




void libgame_handle_player_input_movement_key(gamestate* g, direction_t dir) {
    if (g) {
        const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
        if (hero_id != -1) {
            libgame_update_spritegroup(g, hero_id, dir);

            int xdir = 0;
            int ydir = 0;
            switch (dir) {
            case DIRECTION_RIGHT:
                xdir = 1;
                break;
            case DIRECTION_LEFT:
                xdir = -1;
                break;
            case DIRECTION_DOWN:
                ydir = 1;
                break;
            case DIRECTION_UP:
                ydir = -1;
                break;
            case DIRECTION_UP_LEFT:
                xdir = -1;
                ydir = -1;
                break;
            case DIRECTION_UP_RIGHT:
                xdir = 1;
                ydir = -1;
                break;
            case DIRECTION_DOWN_LEFT:
                xdir = -1;
                ydir = 1;
                break;
            case DIRECTION_DOWN_RIGHT:
                xdir = 1;
                ydir = 1;
                break;
            default:
                break;
            }

            libgame_handleplayerinput_move(g, xdir, ydir);

        } else {
            merror("handleplayerinput_key_right: hero_id is -1");
        }
    }
}




//void libgame_handleplayerinput_key_right(gamestate* g) {
//    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
//    if (hero_id != -1) {
//        libgame_update_spritegroup(g, hero_id, DIRECTION_RIGHT);
//        libgame_handleplayerinput_move(g, 1, 0);
//    } else {
//        merror("handleplayerinput_key_right: hero_id is -1");
//    }
//}




//void libgame_handleplayerinput_key_left(gamestate* g) {
//    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
//    if (hero_id != -1) {
//        libgame_update_spritegroup(g, hero_id, DIRECTION_LEFT);
//        libgame_handleplayerinput_move(g, -1, 0);
//    } else {
//        merror("handleplayerinput_key_left: hero_id is -1");
//    }
//}




//void libgame_handleplayerinput_key_down(gamestate* g) {
//
//    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
//    if (hero_id != -1) {
//        libgame_update_spritegroup(g, hero_id, DIRECTION_DOWN);
//        libgame_handleplayerinput_move(g, 0, 1);
//    } else {
//        merror("handleplayerinput_key_down: hero_id is -1");
//    }
//}




//void libgame_handleplayerinput_key_up(gamestate* g) {
//    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
//    if (hero_id != -1) {
//        libgame_update_spritegroup(g, hero_id, DIRECTION_UP);
//        libgame_handleplayerinput_move(g, 0, -1);
//    } else {
//        merror("handleplayerinput_key_up: hero_id is -1");
//    }
//}




//void libgame_handleplayerinput_key_down_left(gamestate* g) {
//    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
//    if (hero_id != -1) {
//        libgame_update_spritegroup(g, hero_id, DIRECTION_LEFT);
//        libgame_handleplayerinput_move(g, -1, 1);
//    } else {
//        merror("handleplayerinput_key_down_left: hero_id is -1");
//    }
//}




//void libgame_handleplayerinput_key_down_right(gamestate* g) {
//    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
//    if (hero_id != -1) {
//        libgame_update_spritegroup(g, hero_id, DIRECTION_RIGHT);
//        libgame_handleplayerinput_move(g, 1, 1);
//    } else {
//        merror("handleplayerinput_key_down_right: hero_id is -1");
//    }
//}




//void libgame_handleplayerinput_key_up_left(gamestate* g) {
//    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
//    if (hero_id != -1) {
//        libgame_update_spritegroup(g, hero_id, DIRECTION_LEFT);
//        libgame_handleplayerinput_move(g, -1, -1);
//    } else {
//        merror("handleplayerinput_key_up_left: hero_id is -1");
//    }
//}




//void libgame_handleplayerinput_key_up_right(gamestate* g) {
//    const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
//    if (hero_id != -1) {
//        libgame_update_spritegroup(g, hero_id, DIRECTION_RIGHT);
//        libgame_handleplayerinput_move(g, 1, -1);
//    } else {
//        merror("handleplayerinput_key_up_right: hero_id is -1");
//    }
//}




// we will eventually flesh this out to append messages and events
// to their respective logs, however we choose to handle that
//void libgame_entity_look(gamestate* g, entityid id) {
//    entity_t* e = hashtable_entityid_entity_get(g->entities, id);
//    if (e) {
//        tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, e->x, e->y);
//        if (t) {
//            for (int i = 0; i < vectorentityid_capacity(&t->entityids); i++) {
//                entityid id = vectorentityid_get(&t->entityids, i);
//                if (id != e->id) {
//                    entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
//                    if (entity) {
//                        char tmp[256];
//                        snprintf(tmp, 256, "entity id: %d, name: %s", id, entity->name);
//                        msuccess(tmp);
//                    }
//                }
//            }
//        }
//    }
//}




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



#ifdef MOBILE
void libgame_handle_input_player_mobile(gamestate* g) {
    if (g->controlmode == CONTROLMODE_PLAYER) {

        int current_gesture = GetGestureDetected();

        switch (current_gesture) {
        case GESTURE_TAP:
            spritegroup_t* grp = hashtable_entityid_spritegroup_get(g->spritegroups, libgame_lua_get_gamestate_int(L, "HeroId"));
            if (grp) {
                spritegroup_incr(grp);
                g->player_input_received = true;
            }
            break;
        case GESTURE_HOLD:
            libgame_test_enemy_placement(g);
            g->player_input_received = true;
            break;
        case GESTURE_PINCH_OUT:
            g->cam2d.zoom += 1.0f;
            break;
        case GESTURE_PINCH_IN:
            g->cam2d.zoom -= 1.0f;
            if (g->cam2d.zoom < 1.0f) {
                g->cam2d.zoom = 1.0f;
            }
            break;
        case GESTURE_SWIPE_UP:
            libgame_handle_player_input_movement_key(g, DIRECTION_UP);
            g->player_input_received = true;

            break;
        case GESTURE_SWIPE_DOWN:
            libgame_handle_player_input_movement_key(g, DIRECTION_DOWN);
            g->player_input_received = true;
            break;
        case GESTURE_SWIPE_LEFT:
            libgame_handle_player_input_movement_key(g, DIRECTION_LEFT);
            g->player_input_received = true;
            break;
        case GESTURE_SWIPE_RIGHT:
            libgame_handle_player_input_movement_key(g, DIRECTION_RIGHT);
            g->player_input_received = true;
            break;
        default:
            break;
        }
    }
}
#endif



void libgame_handle_input_player(gamestate* g) {
    //minfo("handle_playerinput: starting...");
    if (g->controlmode == CONTROLMODE_PLAYER) {
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
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_KP_4) || IsKeyPressed(KEY_LEFT)) {
            //libgame_handleplayerinput_key_left(g);
            libgame_handle_player_input_movement_key(g, DIRECTION_LEFT);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_KP_2) || IsKeyPressed(KEY_DOWN)) {
            //libgame_handleplayerinput_key_down(g);
            libgame_handle_player_input_movement_key(g, DIRECTION_DOWN);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_KP_8) || IsKeyPressed(KEY_UP)) {
            //libgame_handleplayerinput_key_up(g);
            libgame_handle_player_input_movement_key(g, DIRECTION_UP);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_KP_1)) {
            //libgame_handleplayerinput_key_down_left(g);
            libgame_handle_player_input_movement_key(g, DIRECTION_DOWN_LEFT);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_KP_3)) {
            //libgame_handleplayerinput_key_down_right(g);
            libgame_handle_player_input_movement_key(g, DIRECTION_DOWN_RIGHT);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_KP_7)) {
            //libgame_handleplayerinput_key_up_left(g);
            libgame_handle_player_input_movement_key(g, DIRECTION_UP_LEFT);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_KP_9)) {
            //libgame_handleplayerinput_key_up_right(g);
            libgame_handle_player_input_movement_key(g, DIRECTION_UP_RIGHT);
            g->player_input_received = true;
        }
        //}

        //else if (IsKeyPressed(KEY_COMMA)) {
        //minfo("Comma key pressed");
        // look at the tile the player is on
        //libgame_entity_pickup_item(g, g->hero_id);
        //g->player_input_received = true;
        //}

        if (IsKeyPressed(KEY_P)) {
            minfo("Key pressed: P");
            libgame_lua_mytest(L);
            //    libgame_lua_reserialization_test(L);
        }

        else if (IsKeyPressed(KEY_SPACE)) {
            // randomize the dungeon tiles
            int w = 4;
            int h = 4;
            const entityid hero_id = libgame_lua_get_gamestate_int(L, "HeroId");
            //int hx = libgame_lua_get_entity_int(L, g->hero_id, "x") - w / 2;
            //int hy = libgame_lua_get_entity_int(L, g->hero_id, "y") - h / 2;
            int hx = libgame_lua_get_entity_int(L, hero_id, "x") - w / 2;
            int hy = libgame_lua_get_entity_int(L, hero_id, "y") - h / 2;
            libgame_lua_randomize_dungeon_tiles(L, hx, hy, w, h);
        }

        //else if (IsKeyPressed(KEY_PERIOD)) {
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




void libgame_process_turn(gamestate* g) {
    const int action_count = libgame_lua_get_action_count(L);
    if (action_count > 0) {
        //char buf[128];
        //snprintf(buf, 128, "action count: %d", action_count);
        //minfo("processing actions...");
        //minfo(buf);
        for (int i = 0; i < action_count; i++) {
            const int entity_id = libgame_lua_process_action(L, i + 1);
            //bool result = libgame_lua_process_action(L, 1);
            if (entity_id != -1) {
                //bzero(buf, 128);
                //snprintf(buf, 128, "action processed successfully, entity id: %d", entity_id);
                //msuccess(buf);
                // get entity last move
                const int xdir = libgame_lua_get_entity_int(L, entity_id, "last_move_x");
                const int ydir = libgame_lua_get_entity_int(L, entity_id, "last_move_y");
                direction_t dir = DIRECTION_RIGHT;
                if (xdir == -1 && ydir == -1) {
                    dir = DIRECTION_UP_LEFT;
                } else if (xdir == 0 && ydir == -1) {
                    dir = DIRECTION_UP;
                } else if (xdir == 1 && ydir == -1) {
                    dir = DIRECTION_UP_RIGHT;
                } else if (xdir == -1 && ydir == 0) {
                    dir = DIRECTION_LEFT;
                } else if (xdir == 1 && ydir == 0) {
                    dir = DIRECTION_RIGHT;
                } else if (xdir == -1 && ydir == 1) {
                    dir = DIRECTION_DOWN_LEFT;
                } else if (xdir == 0 && ydir == 1) {
                    dir = DIRECTION_DOWN;
                } else if (xdir == 1 && ydir == 1) {
                    dir = DIRECTION_DOWN_RIGHT;
                }
                libgame_update_spritegroup(g, entity_id, dir);
                libgame_update_spritegroup_move(g, entity_id, xdir * DEFAULT_TILE_SIZE, ydir * DEFAULT_TILE_SIZE);
            }
        }
        libgame_lua_clear_actions(L);
    }
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




void libgame_update_debugpanelbuffer(gamestate* g) {
    int hx = -1;
    int hy = -1;
    const entityid id = libgame_lua_get_gamestate_int(L, "HeroId");
    if (id != -1) {
        hx = libgame_lua_get_entity_int(L, id, "x");
        hy = libgame_lua_get_entity_int(L, id, "y");
    }
    int dw = libgame_lua_get_dungeonfloor_row_count(L);
    int dh = libgame_lua_get_dungeonfloor_col_count(L);
    int action_count = libgame_lua_get_action_count(L);
    int entity_count = libgame_lua_get_num_entities(L);

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
             "Inventory capacity: %ld\n"
             "Dungeon size: %dx%d\n"
             "Action count: %d\n"
             "Entity count: %d\n"

             ,

             g->framecount,
             g->timebeganbuf,
             g->currenttimebuf,
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
             0L,
             //vectorentityid_capacity(&hero->inventory),
             dh,
             dw,
             action_count,
             entity_count

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
    //spritegroup_t* hero_group = hashtable_entityid_spritegroup_get(g->spritegroups, g->hero_id);
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
    const bool result = libgame_lua_create_action(L, id, 2, xdir, ydir);
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



//void libgame_handle_npc_turn(gamestate* g, entityid id) {
//    if (g) {
//        entity_t* e = hashtable_entityid_entity_get(g->entities, id);
//        if (e && e->type == ENTITY_NPC) {
//            // select a random direction to move in, up/left/down/right/ul/ur/dl/dr
//            int dir = GetRandomValue(0, 7);
//            bool result = false;
//            if (dir == 0) {
//                libgame_update_spritegroup_right(g, id);
//                result = libgame_entity_move(g, id, 1, 0);
//                if (result) {
//                    minfo("NPC turn success");
//                    libgame_update_spritegroup_move(g, id, DEFAULT_TILE_SIZE, 0);
//                }
//            } else if (dir == 1) {
//                libgame_update_spritegroup_left(g, id);
//                result = libgame_entity_move(g, id, -1, 0);
//                if (result) {
//                    minfo("NPC turn success");
//                    libgame_update_spritegroup_move(g, id, -DEFAULT_TILE_SIZE, 0);
//                }
//            } else if (dir == 2) {
//                libgame_update_spritegroup_down(g, id);
//                result = libgame_entity_move(g, id, 0, 1);
//                if (result) {
//                    minfo("NPC turn success");
//                    libgame_update_spritegroup_move(g, id, 0, DEFAULT_TILE_SIZE);
//                }
//            } else if (dir == 3) {
//                libgame_update_spritegroup_up(g, id);
//                result = libgame_entity_move(g, id, 0, -1);
//                if (result) {
//                    minfo("NPC turn success");
//                    libgame_update_spritegroup_move(g, id, 0, -DEFAULT_TILE_SIZE);
//                }
//            } else if (dir == 4) {
//                libgame_update_spritegroup_right(g, id);
//                result = libgame_entity_move(g, id, 1, -1);
//                if (result) {
//                    minfo("NPC turn success");
//                    libgame_update_spritegroup_move(g, id, DEFAULT_TILE_SIZE, -DEFAULT_TILE_SIZE);
//                }
//            } else if (dir == 5) {
//                libgame_update_spritegroup_left(g, id);
//                result = libgame_entity_move(g, id, -1, -1);
//                if (result) {
//                    minfo("NPC turn success");
//                    libgame_update_spritegroup_move(g, id, -DEFAULT_TILE_SIZE, -DEFAULT_TILE_SIZE);
//                }
//            } else if (dir == 6) {
//                libgame_update_spritegroup_right(g, id);
//                result = libgame_entity_move(g, id, 1, 1);
//                if (result) {
//                    minfo("NPC turn success");
//                    libgame_update_spritegroup_move(g, id, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE);
//                }
//            } else if (dir == 7) {
//                libgame_update_spritegroup_left(g, id);
//                result = libgame_entity_move(g, id, -1, 1);
//                if (result) {
//                    minfo("NPC turn success");
//                    libgame_update_spritegroup_move(g, id, -DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE);
//                }
//            }
//        }
//    }
//}




void libgame_updategamestate(gamestate* g) {
    //minfo("begin libgame_updategamestate");
    //minfo("libgame_updategamestate: update debug panel buffer");
    libgame_update_debugpanelbuffer(g);
    //setdebugpanelcenter(g);
    //minfo("libgame_updategamestate: update smooth move");
    //libgame_update_smoothmove(g, g->hero_id);
    libgame_update_smoothmove(g, libgame_lua_get_gamestate_int(L, "HeroId"));
    //minfo("libgame_updategamestate: do camera lockon");
    //libgame_do_camera_lock_on(g);
    // at this point, we can take other NPC turns
    // lets iterate over our entities, find the NPCs, and make them move in a random direction
    // then, we will update their smooth moves
    // we will need to eventually disable player input during smooth moving
    if (g->player_input_received) {
        //minfo("libgame_updategamestate: handle npcs turn lua");
        libgame_handle_npcs_turn_lua(g);
        libgame_process_turn(g);
        g->player_input_received = false;
    }
    //minfo("libgame_updategamestate: update smooth moves for NPCs");
    // update smooth move for NPCs and other entities
    libgame_update_smoothmoves_for_entitytype(g, ENTITY_NPC);
    //msuccess("end libgame_updategamestate");
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
    gamestateupdatecurrenttime(g);
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
    libgame_draw_debugpanel(g);

    const int pad_w = 2;
    const int pad_h = 10;
    //const int pad_w = 0;
    //const int pad_h = 0;
    DrawRectangleLines(0, 0, windowwidth - pad_w, windowheight - pad_h, RED);
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




inline void libgame_draw_debugpanel(gamestate* g) {
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
        if (group) {
            const Color c = WHITE;
            const int current = group->current;
            const entitytype_t type = libgame_lua_get_entity_int(L, id, "type");
            if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
                // draw entity shadow, which should exist at current+1 if loaded correctly
                DrawTexturePro(*group->sprites[current + 1]->texture, group->sprites[current + 1]->src, group->dest, (Vector2){0, 0}, 0.0f, c);
            }
            DrawTexturePro(*group->sprites[current]->texture, group->sprites[current]->src, group->dest, (Vector2){0, 0}, 0.0f, c);
            if (g->framecount % FRAMEINTERVAL == 0) {
                sprite_incrframe(group->sprites[group->current]);
                sprite_incrframe(group->sprites[group->current + 1]);
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
                DrawLineV(v[0], v[1], c);
                DrawLineV(v[1], v[2], c);
                DrawLineV(v[2], v[3], c);
                DrawLineV(v[3], v[0], c);
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




void libgame_load_texture_from_data_pack(gamestate* g, img_data_pack_t* pack) {
    if (g) {
        // have to do dithering BEFORE loading as data
        Image img = {
            .data = pack->data,
            .width = pack->width,
            .height = pack->height,
            .format = pack->format,
            .mipmaps = 1,
        };
        Texture2D t = LoadTextureFromImage(img);
        g->txinfo[pack->index].texture = t;
        g->txinfo[pack->index].num_frames = pack->frames;
        g->txinfo[pack->index].contexts = pack->contexts;
        msuccess("libgame_load_texture_from_data");
    } else {
        merror("libgame_loadtexture: gamestate is NULL");
    }
}




void libgame_loadtexture(gamestate* g, int index, int contexts, int frames, bool dodither, const char* path) {
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




void libgame_load_textures(gamestate* g) {
    if (g) {
        libgame_load_textures_from_data(g);
        msuccess("libgame_loadtextures");
    } else {
        merror("libgame_loadtextures: gamestate is NULL");
    }
}




void libgame_load_textures_from_data(gamestate* g) {
    if (g) {
#define NUM_PACKS (44)
        img_data_pack_t packs[NUM_PACKS] = {
            // 3+14+1 for tile
            PK_TILE_DIRT_00,
            PK_TILE_DIRT_01,
            PK_TILE_DIRT_02,

            PK_TILE_STONE_00,
            PK_TILE_STONE_01,
            PK_TILE_STONE_02,
            PK_TILE_STONE_03,
            PK_TILE_STONE_04,
            PK_TILE_STONE_05,
            PK_TILE_STONE_06,
            PK_TILE_STONE_07,
            PK_TILE_STONE_08,
            PK_TILE_STONE_09,
            PK_TILE_STONE_10,
            PK_TILE_STONE_11,
            PK_TILE_STONE_12,
            PK_TILE_STONE_13,

            PK_TILE_STONEWALL_00,


            // 12 for human
            PK_HUMAN_IDLE,
            PK_HUMAN_IDLE_SHADOW,
            PK_HUMAN_WALK,
            PK_HUMAN_WALK_SHADOW,
            PK_HUMAN_ATTACK,
            PK_HUMAN_ATTACK_SHADOW,
            PK_HUMAN_JUMP,
            PK_HUMAN_JUMP_SHADOW,
            PK_HUMAN_SPIN_DIE,
            PK_HUMAN_SPIN_DIE_SHADOW,
            PK_HUMAN_SOUL_DIE,
            PK_HUMAN_SOUL_DIE_SHADOW,

            // 14 for orc
            PK_ORC_IDLE,
            PK_ORC_IDLE_SHADOW,
            PK_ORC_WALK,
            PK_ORC_WALK_SHADOW,
            PK_ORC_ATTACK,
            PK_ORC_ATTACK_SHADOW,
            PK_ORC_CHARGED_ATTACK,
            PK_ORC_CHARGED_ATTACK_SHADOW,
            PK_ORC_JUMP,
            PK_ORC_JUMP_SHADOW,
            PK_ORC_DIE,
            PK_ORC_DIE_SHADOW,
            PK_ORC_DMG,
            PK_ORC_DMG_SHADOW,
        };
        for (int i = 0; i < NUM_PACKS; i++) {
            libgame_load_texture_from_data_pack(g, &packs[i]);
        }
        msuccess("libgame_loadtextures_from_data");
    } else {
        merror("libgame_loadtextures_from_data: gamestate is NULL");
    }
}




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
    for (int i = TX_DIRT_00; i <= TX_HERO_SOUL_DIE_SHADOW; i++) {
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

        entitytype_t entity_type = libgame_lua_get_entity_int(L, id, "type");

        int* keys = NULL;
        int num_keys = 0;
        int offset_x = 0;
        int offset_y = 0;

        if (entity_type == ENTITY_PLAYER || entity_type == ENTITY_NPC) {
            race_t race = libgame_lua_get_entity_int(L, id, "race");
            if (race == RACE_HUMAN) {
                keys = TX_HERO_KEYS;
                num_keys = TX_HERO_KEY_COUNT;
                offset_x = -12;
                offset_y = -12;
            } else if (race == RACE_ORC) {
                keys = TX_ORC_KEYS;
                num_keys = TX_ORC_KEY_COUNT;
                offset_x = -12;
                offset_y = -12;
            }
        }
        //else if (entity_type == ENTITY_ITEM) {
        //
        //}

        if (keys != NULL) {
            libgame_create_spritegroup(g, id, keys, num_keys, offset_x, offset_y);
        } else {
            char buf[128];
            snprintf(buf, 128, "Could not select keys for create_spritegroup_by_id, id: %d, entity_type: %d", id, entity_type);
            merror(buf);
        }
    }
}


void libgame_create_spritegroup(gamestate* g, entityid id, int* keys, int num_keys, int offset_x, int offset_y) {
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
    // add the spritegroup to the hashtable
    minfo("inserting spritegroup into table...");
    hashtable_entityid_spritegroup_insert(g->spritegroups, id, group);
    msuccess("libgame_create_spritegroup end");
}




//void libgame_create_orcspritegroup(gamestate* g, entityid id) {
//    minfo("libgame_create_orcspritegroup begin");
//    spritegroup_t* orc_group = spritegroup_create(20);
//    const int orc_x = libgame_lua_get_entity_int(L, id, "x");
//    const int orc_y = libgame_lua_get_entity_int(L, id, "y");
//    int keys[14] = {TXORCIDLE,
//                    TXORCIDLESHADOW,
//                    TXORCWALK,
//                    TXORCWALKSHADOW,
//                    TXORCATTACK,
//                    TXORCATTACKSHADOW,
//                    TXORCCHARGEDATTACK,
//                    TXORCCHARGEDATTACKSHADOW,
//                    TXORCJUMP,
//                    TXORCJUMPSHADOW,
//                    TXORCDIE,
//                    TXORCDIESHADOW,
//                    TXORCDMG,
//                    TXORCDMGSHADOW};
//    for (int i = 0; i < 14; i++) {
//        sprite* s = sprite_create(&g->txinfo[keys[i]].texture, g->txinfo[keys[i]].contexts, g->txinfo[keys[i]].num_frames);
//        if (!s) {
//            merror("could not create sprite");
//        }
//        spritegroup_add(orc_group, s);
//    }
//    const float w = spritegroup_get(orc_group, 0)->width;
//    const float h = spritegroup_get(orc_group, 0)->height;
//    const float offset_x = -12; // this is hard-coded
//    const float offset_y = -12; // this is hard-coded
//    const float x = orc_x * DEFAULT_TILE_SIZE;
//    const float y = orc_y * DEFAULT_TILE_SIZE;
//    Rectangle dest = {x + offset_x, y + offset_y, w, h};
//    orc_group->current = 0;
//    orc_group->dest = dest;
//    orc_group->off_x = offset_x;
//    orc_group->off_y = offset_y;
//    hashtable_entityid_spritegroup_insert(g->spritegroups, id, orc_group);
//    msuccess("libgame_create_orcspritegroup end");
//}




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




//void libgame_init_dungeonfloor_random_tiles(gamestate* g) {
//    minfo("libgame_init_dungeonfloor_random_tiles begin");
//    if (g) {
//        if (g->dungeonfloor) {
//            libgame_init_dungeonfloor_random_tiles_unsafe(g);
//            msuccess("libgame_init_dungeonfloor_random_tiles end");
//        } else {
//            merror("could not initialize dungeonfloor: dungeonfloor is NULL");
//        }
//    } else {
//        merror("could not initialize dungeonfloor: gamestate is NULL");
//    }
//}




//void libgame_init_dungeonfloor_random_tiles_unsafe(gamestate* g) {
//    const tiletype_t start_type = TILETYPE_STONE_00;
//    const tiletype_t end_type = TILETYPE_STONE_13;
//    for (int i = 0; i < g->dungeonfloor->len; i++) {
//        for (int j = 0; j < g->dungeonfloor->wid; j++) {
//            dungeonfloor_set_tiletype_at_unsafe(g->dungeonfloor, GetRandomValue(start_type, end_type), i, j);
//        }
//    }
//}




//void libgame_init_dungeonfloor(gamestate* g) {
//    minfo("libgame_init_dungeonfloor begin");
//    if (g) {
//        if (g->dungeonfloor) {
//            minfo("setting tiles");
//            libgame_init_dungeonfloor_random_tiles_unsafe(g);
//        } else {
//            merror("could not initialize dungeonfloor: dungeonfloor is NULL");
//        }
//    } else {
//        merror("could not initialize dungeonfloor: gamestate is NULL");
//    }
//}




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
        libgame_lua_set_entity_int(L, id, "race", race);

        //libgame_create_herospritegroup(g, id);
        //libgame_create_spritegroup(g, id, TX_HERO_KEYS, TX_HERO_KEY_COUNT, -12, -12);
        libgame_create_spritegroup_by_id(g, id);


        char buf[128];
        snprintf(buf, 128, "libgame_create_hero_lua: hero entityid %d", id);
        msuccess(buf);
    } else {
        merror("libgame_create_hero_lua: could not create hero entity, expect crashes");
        //g->hero_id = -1;
        libgame_lua_set_gamestate_int(L, "HeroId", -1);
    }
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

        //libgame_create_orcspritegroup(g, id);
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
#ifdef MOBILE
        // just testing on my own phone for now, pixel 6a
        const int tw = 720;
        const int th = 1400;
        const int scale = libgame_lua_get_int(L, "Scale");
        const int dw = tw * scale;
        const int dh = th * scale;
        libgame_lua_set_gamestate_int(L, "TargetWidth", tw);
        libgame_lua_set_gamestate_int(L, "TargetHeight", th);
        libgame_lua_set_gamestate_int(L, "WindowWidth", dw);
        libgame_lua_set_gamestate_int(L, "WindowHeight", dh);
#endif
        libgame_initwindow(g);
        SetRandomSeed(time(NULL));
        libgame_loadfont(g);
        libgame_loadtargettexture(g);
        libgame_load_textures(g);
        libgame_init_datastructures(g);
        //minfo("creating hero");
        //libgame_create_hero_lua(g, "hero", 1, 0);
        //if (g->hero_id == -1) {
        libgame_create_hero_lua(g, "hero", 1, 1);
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
        libgame_update_debugpanelbuffer(g);
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




//void libgame_init_with_state_and_lua(gamestate* state, lua_State* L2) {
//    if (state == NULL) {
//        merror("libgame_initwithstate: state is NULL");
//        return;
//    }
//    if (L == NULL) {
//        merror("libgame_initwithstate: lua state is NULL");
//        return;
//    }
//    g = state;
//    L = L2;
//    libgame_initsharedsetup(g);
//    msuccess("libgame_initwithstate end");
//}




void libgame_closesavegamestate() {
    minfo("libgame_closesavegamestate");
    libgame_closeshared(g);
    msuccess("libgame_closesavegamestate end");
}



//void libgame_close_save_gamestate_save_lua() {
//    minfo("libgame_close_save_gamestate_save_lua");
//    msuccess("libgame_close_save_gamestate_save_lua end");
//}




void libgame_close(gamestate* g) {
    minfo("libgame_close");
    libgame_closeshared(g);
    gamestatefree(g);
    msuccess("libgame_close end");
}




void libgame_closeshared(gamestate* g) {
    // dont need to free most of gamestate
    minfo("libgame_closeshared");
    UnloadFont(g->font);
    libgame_unloadtextures(g);
    UnloadRenderTexture(target);
    CloseWindow();
    lua_close(L);
    msuccess("libgame_closeshared end");
}



//void libgame_close_shared_minus_lua(gamestate* g) {
// dont need to free most of gamestate
//    minfo("libgame_closeshared");
//    UnloadFont(g->font);
//    libgame_unloadtextures(g);
//    UnloadRenderTexture(target);
//    CloseWindow();
//lua_close(L);
//    msuccess("libgame_closeshared end");
//}




gamestate* libgame_getgamestate() {
    return g;
}



//lua_State* libgame_getlua() {
//    return L;
//}




//const bool libgame_entity_move_lua(gamestate* g, entityid id, int x, int y) {
//minfo("libgame_entity_move_lua");
//bool retval = libgame_lua_entity_move(L, id, x, y);
//bool retval = libgame_lua_create_action(L, 2, id, x, y);
//bool retval = libgame_lua_create_action(L, id, 2, x, y);
//if (!retval) {
//    merror("libgame_entity_move: move unsuccessful");
//}
//return retval;
//}




//const bool libgame_is_tile_occupied_with_entitytype(gamestate* g, const entitytype_t type, const int x, const int y) {
//    tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
//    if (t) {
//        int count = vectorentityid_capacity(&t->entityids);
//        // get is tile occupied
//        for (int i = 0; i < count; i++) {
//            entityid id = vectorentityid_get(&t->entityids, i);
//            entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
//            if (entity->type == type) {
//                minfo("is_tile_occupied: tile occupied");
//                fprintf(stdout, "entity type: %d at (%d, %d)\n", entity->type, x, y);
//                return true;
//            }
//        }
//    }
//    return false;
//}



//const bool libgame_entity_move_check(gamestate* g, entity_t* e, int x, int y) {
//    bool retval = false;
//    if (e) {
//        // check bounds
//        if (e->x + x < 0 || e->x + x >= g->dungeonfloor->len) {
//            merror("move_check: out of bounds x");
//            retval = false;
//        } else if (e->y + y < 0 || e->y + y >= g->dungeonfloor->wid) {
//            merror("move_check: out of bounds y");
//            retval = false;
//        } else {
//            bool r0 = !libgame_is_tile_occupied_with_entitytype(g, ENTITY_PLAYER, e->x + x, e->y + y);
//            bool r1 = !libgame_is_tile_occupied_with_entitytype(g, ENTITY_NPC, e->x + x, e->y + y);
//            return r0 && r1;
//        }
//    } else {
//        merror("move_check: entity is NULL");
//        retval = false;
//    }
//    return retval;
//}




const bool libgame_external_check_reload() {
    return IsKeyPressed(KEY_R);
}
