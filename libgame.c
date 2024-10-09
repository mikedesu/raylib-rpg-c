#include "controlmode.h"
#include "dungeonfloor.h"
#include "entity.h"
#include "entitytype.h"
#include "fadestate.h"
#include "gamestate.h"
#include "get_txkey_for_tiletype.h"
#include "hashtable_entityid_entity.h"
#include "hashtable_entityid_spritegroup.h"
#include "itemtype.h"
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
#include "utils.h"
#include "vectorentityid.h"
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
Rectangle target_src = (Rectangle){0, 0, 0, 0};
Rectangle target_dest = (Rectangle){0, 0, 0, 0};
Vector2 target_origin = (Vector2){0, 0};


int activescene = GAMEPLAYSCENE;
int targetwidth = DEFAULT_TARGET_WIDTH;
int targetheight = DEFAULT_TARGET_HEIGHT;
int windowwidth = DEFAULT_WINDOW_WIDTH;
int windowheight = DEFAULT_WINDOW_HEIGHT;




const bool libgame_entity_try_attack(gamestate* g, entityid id, const int x, const int y) {
    // we dont have full rules or functions for attacking yet
    // however
    // given the entityid and the position they are attack
    minfo("try_attack: starting...");
    entity_t* e = hashtable_entityid_entity_get(g->entities, id);
    if (e) {
        // get the tile at pos
        //tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, pos);
        tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
        if (t) {
            // check to see if tile has anything
            const bool occupied = vectorentityid_capacity(&t->entityids) > 0;
            if (occupied) {
                // get the entityids at that tile
                for (int i = 0; i < vectorentityid_capacity(&t->entityids); i++) {
                    entityid id2 = vectorentityid_get(&t->entityids, i);
                    entity_t* e2 = hashtable_entityid_entity_get(g->entities, id2);
                    if (e2) {
                        entitytype_t type = e2->type;
                        // check "all" the types that can be attacked
                        if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
                            msuccess("attack attempt was successful");
                            // at this point we would process the rules for attacking
                            // i.e. entity rolls a 1d20 + attack bonus versus target's AC
                            // if the roll is greater than the target's AC, then the attack hits
                            // and we roll damage
                            return true;
                        }
                    }
                }
            } else {
                merror("try_attack: tile is empty");
                return false;
            }
        } else {
            // attacking out-of-bounds or something else
            merror("try_attack: tile is NULL");
            return false;
        }
    } else {
        merror("try_attack: entity is NULL");
    }
    return false;
}




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
        spritegroup_set_current(group, index);
    }
}




const bool libgame_entity_inventory_contains_type(gamestate* g, entityid id, itemtype_t type) {
    entity_t* e = hashtable_entityid_entity_get(g->entities, id);
    if (e) {
        for (int i = 0; i < vectorentityid_capacity(&e->inventory); i++) {
            entityid id = vectorentityid_get(&e->inventory, i);
            entity_t* item = hashtable_entityid_entity_get(g->entities, id);
            if (item && item->itemtype == type) {
                return true;
            }
        }
    }
    return false;
}




void libgame_test_enemy_placement(gamestate* g) {
    entity_t* hero = hashtable_entityid_entity_get(g->entities, g->hero_id);
    if (hero) {
        // check to see if there are any items at that location
        //tile_t* t0 = dungeonfloor_get_tile(g->dungeonfloor, hero->x + 1, hero->y);
        //if (t0) {
        if (!libgame_entitytype_is_at(g, ENTITY_NPC, hero->x + 1, hero->y)) {
            libgame_create_orc(g, "orc", hero->x + 1, hero->y);
        }
        //}
    }
}



void libgame_handle_player_attack(gamestate* g);

void libgame_handle_player_attack(gamestate* g) {
    if (libgame_entity_inventory_contains_type(g, g->hero_id, ITEM_WEAPON)) {
        entity_t* hero = hashtable_entityid_entity_get(g->entities, g->hero_id);
        if (hero) {
            // try an attack
            if (libgame_entity_try_attack(g, g->hero_id, hero->x + 1, hero->y)) {
                // successfull attack
                // lets try setting the orc animation sprite on success
                tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, hero->x + 1, hero->y);
                entityid orc_id = -1;
                for (int i = 0; i < vectorentityid_capacity(&t->entityids); i++) {
                    entityid id = vectorentityid_get(&t->entityids, i);
                    entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
                    if (entity->type == ENTITY_NPC) {
                        orc_id = id;
                        break;
                    }
                }
                libgame_entity_anim_set(g, orc_id, 10);
            }
            // in any case, set the attack animation
            libgame_entity_anim_set(g, g->hero_id, SPRITEGROUP_ANIM_HUMAN_ATTACK);
        }
    } else {
        libgame_entity_anim_set(g, g->hero_id, SPRITEGROUP_ANIM_HUMAN_IDLE);
    }
}



void libgame_handleinput(gamestate* g) {
    //minfo("handleinput: starting...");
    //if (IsKeyPressed(KEY_SPACE)) {
    //minfo("key space pressed");
    //if (g->fadestate == FADESTATENONE) {
    //    g->fadestate = FADESTATEOUT;
    //}
    //g->do_one_rotation = true;
    //}
    if (IsKeyPressed(KEY_A)) {
        // technically we dont want to be able to attack until we have picked up a weapon...
        libgame_handle_player_attack(g);
        g->player_input_received = true;
    }

    if (IsKeyPressed(KEY_E)) {
        libgame_test_enemy_placement(g);
        g->player_input_received = true;
    }

    // lets place a torch where the player is standing
    if (IsKeyPressed(KEY_T)) {
        entity_t* hero = hashtable_entityid_entity_get(g->entities, g->hero_id);
        if (hero) {
            // check to see if there are any items at that location
            tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, hero->x, hero->y);
            if (t) {
                for (int i = 0; i < vectorentityid_capacity(&t->entityids); i++) {
                    entityid id = vectorentityid_get(&t->entityids, i);
                    entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
                    if (entity->type == ENTITY_ITEM && entity->itemtype == ITEM_TORCH) {
                        minfo("torch already exists at this location");
                    } else {
                        libgame_createitembytype(g, ITEM_TORCH, hero->x, hero->y);
                    }
                }
            }
        }
        g->player_input_received = true;
    }
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




void libgame_update_spritegroup_right(gamestate* g, entityid id) {
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (group) {
        int ctx = SG_CTX_R_D;
        switch (group->sprites[group->current]->currentcontext) {
        case SG_CTX_R_D:
            ctx = SG_CTX_R_D;
            break;
        case SG_CTX_L_D:
            ctx = SG_CTX_R_D;
            break;
        case SG_CTX_R_U:
            ctx = SG_CTX_R_U;
            break;
        case SG_CTX_L_U:
            ctx = SG_CTX_R_U;
            break;
        default:
            ctx = SG_CTX_R_D;
            break;
        }
        spritegroup_setcontexts(group, ctx);
        group->current = SPRITEGROUP_ANIM_HUMAN_WALK;
    }
}




void libgame_update_spritegroup_left(gamestate* g, entityid id) {
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (group) {
        int ctx = SG_CTX_L_D;
        switch (group->sprites[group->current]->currentcontext) {
        case SG_CTX_R_D:
            ctx = SG_CTX_L_D;
            break;
        case SG_CTX_L_D:
            ctx = SG_CTX_L_D;
            break;
        case SG_CTX_R_U:
            ctx = SG_CTX_L_U;
            break;
        case SG_CTX_L_U:
            ctx = SG_CTX_L_U;
            break;
        default:
            ctx = SG_CTX_L_D;
            break;
        }
        spritegroup_setcontexts(group, ctx);
        group->current = SPRITEGROUP_ANIM_HUMAN_WALK;
    }
}




void libgame_update_spritegroup_up(gamestate* g, entityid id) {
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (group) {
        int ctx = SG_CTX_R_U;
        switch (group->sprites[group->current]->currentcontext) {
        case SG_CTX_R_D:
            ctx = SG_CTX_R_U;
            break;
        case SG_CTX_L_D:
            ctx = SG_CTX_L_U;
            break;
        case SG_CTX_R_U:
            ctx = SG_CTX_R_U;
            break;
        case SG_CTX_L_U:
            ctx = SG_CTX_L_U;
            break;
        default:
            ctx = SG_CTX_R_U;
            break;
        }
        spritegroup_setcontexts(group, ctx);
        group->current = SPRITEGROUP_ANIM_HUMAN_WALK;
    }
}




void libgame_update_spritegroup_down(gamestate* g, entityid id) {
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (group) {
        int ctx = SG_CTX_R_D;
        switch (group->sprites[group->current]->currentcontext) {
        case SG_CTX_R_D:
            ctx = SG_CTX_R_D;
            break;
        case SG_CTX_L_D:
            ctx = SG_CTX_L_D;
            break;
        case SG_CTX_R_U:
            ctx = SG_CTX_R_D;
            break;
        case SG_CTX_L_U:
            ctx = SG_CTX_L_D;
            break;
        default:
            ctx = SG_CTX_R_D;
            break;
        }
        spritegroup_setcontexts(group, ctx);
        group->current = SPRITEGROUP_ANIM_HUMAN_WALK;
    }
}




void libgame_updateherospritegroup_right(gamestate* g) {
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_get(g->spritegroups, g->hero_id);
    if (hero_group) {
        int ctx = SG_CTX_R_D;
        switch (hero_group->sprites[hero_group->current]->currentcontext) {
        case SG_CTX_R_D:
            ctx = SG_CTX_R_D;
            break;
        case SG_CTX_L_D:
            ctx = SG_CTX_R_D;
            break;
        case SG_CTX_R_U:
            ctx = SG_CTX_R_U;
            break;
        case SG_CTX_L_U:
            ctx = SG_CTX_R_U;
            break;
        default:
            ctx = SG_CTX_R_D;
            break;
        }
        spritegroup_setcontexts(hero_group, ctx);
        hero_group->current = SPRITEGROUP_ANIM_HUMAN_WALK;
    }
}




void libgame_updateherospritegroup_left(gamestate* g) {
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_get(g->spritegroups, g->hero_id);
    if (hero_group) {
        int ctx = SG_CTX_L_D;
        switch (hero_group->sprites[hero_group->current]->currentcontext) {
        case SG_CTX_R_D:
            ctx = SG_CTX_L_D;
            break;
        case SG_CTX_L_D:
            ctx = SG_CTX_L_D;
            break;
        case SG_CTX_R_U:
            ctx = SG_CTX_L_U;
            break;
        case SG_CTX_L_U:
            ctx = SG_CTX_L_U;
            break;
        default:
            ctx = SG_CTX_L_D;
            break;
        }
        spritegroup_setcontexts(hero_group, ctx);
        hero_group->current = SPRITEGROUP_ANIM_HUMAN_WALK;
    }
}




void libgame_updateherospritegroup_up(gamestate* g) {
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_get(g->spritegroups, g->hero_id);
    if (hero_group) {
        int ctx = SG_CTX_R_U;
        switch (hero_group->sprites[hero_group->current]->currentcontext) {
        case SG_CTX_R_D:
            ctx = SG_CTX_R_U;
            break;
        case SG_CTX_L_D:
            ctx = SG_CTX_L_U;
            break;
        case SG_CTX_R_U:
            ctx = SG_CTX_R_U;
            break;
        case SG_CTX_L_U:
            ctx = SG_CTX_L_U;
            break;
        default:
            ctx = SG_CTX_R_U;
            break;
        }
        spritegroup_setcontexts(hero_group, ctx);
        hero_group->current = SPRITEGROUP_ANIM_HUMAN_WALK;
    }
}




void libgame_updateherospritegroup_down(gamestate* g) {
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_get(g->spritegroups, g->hero_id);
    if (hero_group) {
        int ctx = SG_CTX_R_D;
        switch (hero_group->sprites[hero_group->current]->currentcontext) {
        case SG_CTX_R_D:
            ctx = SG_CTX_R_D;
            break;
        case SG_CTX_L_D:
            ctx = SG_CTX_L_D;
            break;
        case SG_CTX_R_U:
            ctx = SG_CTX_R_D;
            break;
        case SG_CTX_L_U:
            ctx = SG_CTX_L_D;
            break;
        default:
            ctx = SG_CTX_R_D;
            break;
        }
        spritegroup_setcontexts(hero_group, ctx);
        hero_group->current = SPRITEGROUP_ANIM_HUMAN_WALK;
    }
}




void libgame_update_spritegroup_move(gamestate* g, entityid id, int x, int y) {
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (sg) {
        //sg->move = (Vector2){x, y};
        sg->move_x = x;
        sg->move_y = y;
    }
}




void libgame_handleplayerinput_key_right(gamestate* g) {
    libgame_updateherospritegroup_right(g);
    bool result = libgame_entity_move(g, g->hero_id, 1, 0);
    if (result) {
        libgame_update_spritegroup_move(g, g->hero_id, DEFAULT_TILE_SIZE, 0);
    }
}




void libgame_handleplayerinput_key_left(gamestate* g) {
    libgame_updateherospritegroup_left(g);
    bool result = libgame_entity_move(g, g->hero_id, -1, 0);
    if (result) {
        libgame_update_spritegroup_move(g, g->hero_id, -DEFAULT_TILE_SIZE, 0);
    }
}




void libgame_handleplayerinput_key_down(gamestate* g) {
    libgame_updateherospritegroup_down(g);
    bool result = libgame_entity_move(g, g->hero_id, 0, 1);
    if (result) {
        libgame_update_spritegroup_move(g, g->hero_id, 0, DEFAULT_TILE_SIZE);
    }
}




void libgame_handleplayerinput_key_up(gamestate* g) {
    libgame_updateherospritegroup_up(g);
    bool result = libgame_entity_move(g, g->hero_id, 0, -1);
    if (result) {
        libgame_update_spritegroup_move(g, g->hero_id, 0, -DEFAULT_TILE_SIZE);
    }
}




void libgame_handleplayerinput_key_down_left(gamestate* g) {
    libgame_updateherospritegroup_left(g);
    bool result = libgame_entity_move(g, g->hero_id, -1, 1);
    if (result) {
        libgame_update_spritegroup_move(g, g->hero_id, -DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE);
    }
}




void libgame_handleplayerinput_key_down_right(gamestate* g) {
    libgame_updateherospritegroup_right(g);
    bool result = libgame_entity_move(g, g->hero_id, 1, 1);
    if (result) {
        libgame_update_spritegroup_move(g, g->hero_id, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE);
    }
}




void libgame_handleplayerinput_key_up_left(gamestate* g) {
    libgame_updateherospritegroup_left(g);
    bool result = libgame_entity_move(g, g->hero_id, -1, -1);
    if (result) {
        libgame_update_spritegroup_move(g, g->hero_id, -DEFAULT_TILE_SIZE, -DEFAULT_TILE_SIZE);
    }
}




void libgame_handleplayerinput_key_up_right(gamestate* g) {
    libgame_updateherospritegroup_right(g);
    bool result = libgame_entity_move(g, g->hero_id, 1, -1);
    if (result) {
        libgame_update_spritegroup_move(g, g->hero_id, DEFAULT_TILE_SIZE, -DEFAULT_TILE_SIZE);
    }
}




// we will eventually flesh this out to append messages and events
// to their respective logs, however we choose to handle that
void libgame_entity_look(gamestate* g, entityid id) {
    entity_t* e = hashtable_entityid_entity_get(g->entities, id);
    if (e) {
        tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, e->x, e->y);
        if (t) {
            for (int i = 0; i < vectorentityid_capacity(&t->entityids); i++) {
                entityid id = vectorentityid_get(&t->entityids, i);
                if (id != e->id) {
                    entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
                    if (entity) {
                        char tmp[256];
                        snprintf(tmp, 256, "entity id: %d, name: %s", id, entity->name);
                        msuccess(tmp);
                    }
                }
            }
        }
    }
}




const entityid libgame_entity_pickup_item(gamestate* g, const entityid id) {
    entityid retval = -1;
    entity_t* e = hashtable_entityid_entity_get(g->entities, id);
    tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, e->x, e->y);
    if (e && t) {
        minfo("entity and tile pointers acquired, entering loop...");
        for (int i = 0; i < vectorentityid_capacity(&t->entityids); i++) {
            entityid id2 = vectorentityid_get(&t->entityids, i);
            entity_t* entity = hashtable_entityid_entity_get(g->entities, id2);
            if (entity && entity->type == ENTITY_ITEM) {
                minfo("item found on tile, removing...");
                // we are going to remove the item from the tile
                // and add it to the player's inventory
                vectorentityid_remove_value(&t->entityids, id2);
                // add the item to the player's inventory
                vectorentityid_add(&e->inventory, id2);
                retval = id2;
                break;
            } else {
                merror("no item found on tile");
            }
        }
    }
    return retval;
}



void libgame_handle_input_player(gamestate* g) {
    //minfo("handle_playerinput: starting...");
    if (g->controlmode == CONTROLMODE_PLAYER) {
        //const bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        // this is just a test
        // the real setup will involve managing the player's dungeon position
        // and then translating that into a destination on screen

        // eventually we will create a mapping for custom controls
        // that way we can centralize handling of the controls
        if (IsKeyPressed(KEY_KP_6) || IsKeyPressed(KEY_RIGHT)) {
            libgame_handleplayerinput_key_right(g);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_KP_4) || IsKeyPressed(KEY_LEFT)) {
            libgame_handleplayerinput_key_left(g);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_KP_2) || IsKeyPressed(KEY_DOWN)) {
            libgame_handleplayerinput_key_down(g);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_KP_8) || IsKeyPressed(KEY_UP)) {
            libgame_handleplayerinput_key_up(g);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_KP_1)) {
            libgame_handleplayerinput_key_down_left(g);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_KP_3)) {
            libgame_handleplayerinput_key_down_right(g);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_KP_7)) {
            libgame_handleplayerinput_key_up_left(g);
            g->player_input_received = true;
        } else if (IsKeyPressed(KEY_KP_9)) {
            libgame_handleplayerinput_key_up_right(g);
            g->player_input_received = true;
        }


        else if (IsKeyPressed(KEY_COMMA)) {
            //minfo("Comma key pressed");
            // look at the tile the player is on
            libgame_entity_pickup_item(g, g->hero_id);
            g->player_input_received = true;
        }

        else if (IsKeyPressed(KEY_PERIOD)) {
            libgame_entity_look(g, g->hero_id);
            spritegroup_t* hero_group = hashtable_entityid_spritegroup_get(g->spritegroups, g->hero_id);
            if (hero_group) {
                hero_group->current = 0; //standing/idle
            }
            g->player_input_received = true;
        }
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




void libgame_initwindow() {
    //minfo("begin libgame_initwindow");
    const char* title = DEFAULT_WINDOW_TITLE;
    // have to do inittitlescene after initwindow
    // cant load textures before initwindow
    InitWindow(windowwidth, windowheight, title);
    while (!IsWindowReady())
        ;
    // this is hard-coded for now so we can auto-position the window
    // for easier config during streaming
    //SetWindowMonitor(0);
    SetWindowMonitor(1);
    const int x = DEFAULT_WINDOW_POS_X;
    const int y = DEFAULT_WINDOW_POS_Y;
    SetWindowPosition(x, y);
    SetTargetFPS(DEFAULT_TARGET_FPS);
    SetExitKey(KEY_Q);
    //minfo("end of libgame_initwindow");
}




void libgame_closewindow() {
    CloseWindow();
}




void libgame_update_debugpanelbuffer(gamestate* g) {
    entity_t* hero = hashtable_entityid_entity_get(g->entities, g->hero_id);

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
             hero->x,
             hero->y,
             vectorentityid_capacity(&hero->inventory),
             g->dungeonfloor->len,
             g->dungeonfloor->wid);
}




void libgame_update_smoothmove(gamestate* g, entityid id) {
    //spritegroup_t* hero_group = hashtable_entityid_spritegroup_get(g->spritegroups, g->hero_id);
    if (g) {
        //g->is_updating_smooth_move = true;
        spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
        const int tilesize = DEFAULT_TILE_SIZE;
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
            if (group->move_x == 0.0f && group->move_y == 0.0f) {
                //g->is_updating_smooth_move = false;
                entity_t* e = hashtable_entityid_entity_get(g->entities, id);
                if (e) {
                    //group->dest.x = e->pos.x * tilesize + group->off_x;
                    group->dest.x = e->x * tilesize + group->off_x;
                    //group->dest.y = e->pos.y * tilesize + group->off_y;
                    group->dest.y = e->y * tilesize + group->off_y;
                }
                //g->smooth_move_index++;
                //if (g->smooth_move_index >= vectorentityid_capacity(&g->entityids)) {
                //    g->smooth_move_index = 0;
                //}
            }
        }
    }
}




void libgame_do_cameralockon(gamestate* g) {
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_get(g->spritegroups, g->hero_id);
    if (g->cam_lockon) {
        g->cam2d.target = (Vector2){hero_group->dest.x, hero_group->dest.y};
    }
}




void libgame_do_one_camera_rotation(gamestate* g) {
    if (g->do_one_rotation) {
        g->cam2d.rotation += 8.0f;
        if (g->cam2d.rotation >= 360.0f) {
            g->cam2d.rotation = 0.0f;
            g->do_one_rotation = false;
        }
    }
}




void libgame_handle_npc_turn(gamestate* g, entityid id) {
    if (g) {
        entity_t* e = hashtable_entityid_entity_get(g->entities, id);
        if (e && e->type == ENTITY_NPC) {
            // select a random direction to move in, up/left/down/right/ul/ur/dl/dr
            int dir = GetRandomValue(0, 7);
            bool result = false;
            if (dir == 0) {
                libgame_update_spritegroup_right(g, id);
                result = libgame_entity_move(g, id, 1, 0);
                if (result) {
                    minfo("NPC turn success");
                    libgame_update_spritegroup_move(g, id, DEFAULT_TILE_SIZE, 0);
                }
            } else if (dir == 1) {
                libgame_update_spritegroup_left(g, id);
                result = libgame_entity_move(g, id, -1, 0);
                if (result) {
                    minfo("NPC turn success");
                    libgame_update_spritegroup_move(g, id, -DEFAULT_TILE_SIZE, 0);
                }
            } else if (dir == 2) {
                libgame_update_spritegroup_down(g, id);
                result = libgame_entity_move(g, id, 0, 1);
                if (result) {
                    minfo("NPC turn success");
                    libgame_update_spritegroup_move(g, id, 0, DEFAULT_TILE_SIZE);
                }
            } else if (dir == 3) {
                libgame_update_spritegroup_up(g, id);
                result = libgame_entity_move(g, id, 0, -1);
                if (result) {
                    minfo("NPC turn success");
                    libgame_update_spritegroup_move(g, id, 0, -DEFAULT_TILE_SIZE);
                }
            } else if (dir == 4) {
                libgame_update_spritegroup_right(g, id);
                result = libgame_entity_move(g, id, 1, -1);
                if (result) {
                    minfo("NPC turn success");
                    libgame_update_spritegroup_move(g, id, DEFAULT_TILE_SIZE, -DEFAULT_TILE_SIZE);
                }
            } else if (dir == 5) {
                libgame_update_spritegroup_left(g, id);
                result = libgame_entity_move(g, id, -1, -1);
                if (result) {
                    minfo("NPC turn success");
                    libgame_update_spritegroup_move(g, id, -DEFAULT_TILE_SIZE, -DEFAULT_TILE_SIZE);
                }
            } else if (dir == 6) {
                libgame_update_spritegroup_right(g, id);
                result = libgame_entity_move(g, id, 1, 1);
                if (result) {
                    minfo("NPC turn success");
                    libgame_update_spritegroup_move(g, id, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE);
                }
            } else if (dir == 7) {
                libgame_update_spritegroup_left(g, id);
                result = libgame_entity_move(g, id, -1, 1);
                if (result) {
                    minfo("NPC turn success");
                    libgame_update_spritegroup_move(g, id, -DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE);
                }
            }
        }
    }
}




void libgame_updategamestate(gamestate* g) {
    //minfo("begin libgame_updategamestate");
    libgame_update_debugpanelbuffer(g);
    //setdebugpanelcenter(g);
    libgame_update_smoothmove(g, g->hero_id);
    libgame_do_cameralockon(g);
    // at this point, we can take other NPC turns
    // lets iterate over our entities, find the NPCs, and make them move in a random direction
    // then, we will update their smooth moves
    // we will need to eventually disable player input during smooth moving
    if (g->player_input_received) {
        for (int i = 0; i < vectorentityid_capacity(&g->entityids); i++) {
            entityid id = vectorentityid_get(&g->entityids, i);
            libgame_handle_npc_turn(g, id);
        }
        g->player_input_received = false;
    }

    // update smooth move for NPCs and other entities
    libgame_update_smoothmoves_for_entitytype(g, ENTITY_NPC);
    //minfo("end libgame_updategamestate");
}




void libgame_update_smoothmoves_for_entitytype(gamestate* g, entitytype_t type) {
    for (int i = 0; i < vectorentityid_capacity(&g->entityids); i++) {
        entityid id = vectorentityid_get(&g->entityids, i);
        entity_t* e = hashtable_entityid_entity_get(g->entities, id);
        if (e && e->type == type) {
            libgame_update_smoothmove(g, id);
        }
    }
}




void libgame_drawframeend(gamestate* g) {
    EndDrawing();
    g->framecount++;
    gamestateupdatecurrenttime(g);
}




void libgame_drawframe(gamestate* g) {
    BeginDrawing();
    BeginTextureMode(target);
    switch (activescene) {
    case SCENE_COMPANY:
        libgame_drawcompanyscene(g);
        break;
    case SCENE_TITLE:
        libgame_drawtitlescene(g);
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
    libgame_drawframeend(g);
}




void libgame_calc_debugpanel_size(gamestate* g) {
    const int sz = 14, sp = 1;
    const Vector2 m = MeasureTextEx(g->font, g->debugpanel.buffer, sz, sp);
    g->debugpanel.w = m.x, g->debugpanel.h = m.y;
}




inline void libgame_draw_debugpanel(gamestate* g) {
    if (g && g->debugpanelon) {
        const int fontsize = 14;
        const int spacing = 1;
        const int xy = 10;
        const int wh = 20;
        Color c = {0x33, 0x33, 0x33, 255};
        Color c2 = WHITE;
        const Vector2 p = {g->debugpanel.x, g->debugpanel.y}, o = {0, 0};
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
    Color c = GREEN;
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
    Rectangle tile_src = {0, 0, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE};
    Rectangle tile_dest = {0, 0, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE};
    Color c = WHITE;
    float rotation = 0;
    for (int i = 0; i < g->dungeonfloor->len; i++) {
        for (int j = 0; j < g->dungeonfloor->wid; j++) {
            // get the tile
            tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, i, j);
            if (t) {
                // check the tile type
                tile_dest.x = i * DEFAULT_TILE_SIZE;
                tile_dest.y = j * DEFAULT_TILE_SIZE;
                int key = -1;
                // this is very inefficient i think?
                key = get_txkey_for_tiletype(t->type);
                if (key != -1) {
                    DrawTexturePro(g->txinfo[key].texture, tile_src, tile_dest, (Vector2){0, 0}, rotation, c);
                }
            }
        }
    }
}




void libgame_draw_entity(gamestate* g, entityid id) {
    if (g) {
        spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
        if (group) {
            const Color c = WHITE;
            DrawTexturePro(*group->sprites[group->current]->texture,
                           group->sprites[group->current]->src,
                           group->dest,
                           (Vector2){0, 0},
                           0.0f,
                           c);
            // the problem here is we dont have any information on the offset we used at spritegroup creation
            // if we keep record of the offsets used when loading spritegroups, we could use that here
            // so we can keep the tile and entity lighting generic
            if (g->framecount % FRAMEINTERVAL == 0) {
                sprite_incrframe(group->sprites[group->current]);
            }
            if (g->debugpanelon) {
                Color c = {51, 51, 51, 255};
                const int x = group->dest.x;
                const int y = group->dest.y;
                const int w = group->dest.width;
                const int h = group->dest.height;
                const Vector2 v[4] = {{x, y}, {x + w, y}, {x + w, y + h}, {x, y + h}};
                DrawLineV(v[0], v[1], c);
                DrawLineV(v[1], v[2], c);
                DrawLineV(v[2], v[3], c);
                DrawLineV(v[3], v[0], c);
            }
        }
    }
}




void libgame_draw_items(gamestate* g, const itemtype_t type, const int x, const int y) {
    const tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
    for (int k = 0; k < vectorentityid_capacity(&t->entityids); k++) {
        const entityid id = vectorentityid_get(&t->entityids, k);
        const entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
        if (entity->type == ENTITY_ITEM && entity->itemtype == type) {
            libgame_draw_entity(g, id);
        }
    }
}




void libgame_draw_items_that_are_not(gamestate* g, const itemtype_t type, const int x, const int y) {
    tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
    for (int k = 0; k < vectorentityid_capacity(&t->entityids); k++) {
        entityid id = vectorentityid_get(&t->entityids, k);
        entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
        if (entity->type == ENTITY_ITEM && entity->itemtype != type) {
            libgame_draw_entity(g, id);
        }
    }
}




void libgame_draw_entities_at(gamestate* g, const entitytype_t type, const int x, const int y) {
    tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
    if (t) {
        for (int k = 0; k < vectorentityid_capacity(&t->entityids); k++) {
            entityid id = vectorentityid_get(&t->entityids, k);
            entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
            if (entity->type == type) {
                libgame_draw_entity(g, id);
            }
        }
    }
}




const bool libgame_entitytype_is_at(gamestate* g, const entitytype_t type, const int x, const int y) {
    tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
    if (t) {
        for (int k = 0; k < vectorentityid_capacity(&t->entityids); k++) {
            entity_t* entity = hashtable_entityid_entity_get(g->entities, vectorentityid_get(&t->entityids, k));
            if (entity->type == type) {
                return true;
            }
        }
    }
    return false;
}




const bool libgame_itemtype_is_at(gamestate* g, const itemtype_t type, const int x, const int y) {
    tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
    if (t) {
        for (int k = 0; k < vectorentityid_capacity(&t->entityids); k++) {
            entityid id = vectorentityid_get(&t->entityids, k);
            entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
            if (entity->type == ENTITY_ITEM && entity->itemtype == type) {
                return true;
            }
        }
    }
    return false;
}




void libgame_draw_gameplayscene_entities(gamestate* g) {
    if (g) {
        for (int i = 0; i < g->dungeonfloor->len; i++) {
            for (int j = 0; j < g->dungeonfloor->wid; j++) {
                libgame_draw_items(g, ITEM_TORCH, i, j);
                libgame_draw_items_that_are_not(g, ITEM_TORCH, i, j);
                libgame_draw_entities_at(g, ENTITY_NPC, i, j);
                libgame_draw_entities_at(g, ENTITY_PLAYER, i, j);
            }
        }
    }
}




void libgame_draw_gameplayscene(gamestate* g) {
    if (g) {
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
    }
}




void libgame_drawtitlescene(gamestate* g) {
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




void libgame_drawcompanyscene(gamestate* g) {
    const Color bgc = BLACK;
    const Color fgc = {0x66, 0x66, 0x66, 255};
    const int fontsize = 32;
    const int spacing = 1;
    const int interval = 120;
    const int dur = 60;
    char b[128];
    char b2[128];
    char b3[128];
    bzero(b, 128);
    bzero(b2, 128);
    bzero(b3, 128);
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




void libgame_loadtexture(gamestate* g, int index, int contexts, int frames, bool dodither, const char* path) {
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
    //SetTextureFilter(textures[index], TEXTURE_FILTER_POINT);
    //SetTextureFilter(txinfo[index].texture, TEXTURE_FILTER_POINT);
}




void libgame_loadtextures(gamestate* g) {
    minfo("load textures");
    libgame_loadtexturesfromfile(g, "textures.txt");
}




void libgame_loadtexturesfromfile(gamestate* g, const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) {
        //mprint("could not open file");
        return;
    }
    int index = 0;
    int contexts = 0;
    int frames = 0;
    int dodither = 0;
    char line[256];
    char txpath[256];
    while (fgets(line, 256, f)) {
        // if the line begins with a #, skip it
        if (line[0] == '#') {
            continue;
        }
        sscanf(line, "%d %d %d %d %s", &index, &contexts, &frames, &dodither, txpath);
        libgame_loadtexture(g, index, contexts, frames, dodither, txpath);
        bzero(line, 256);
        bzero(txpath, 256);
    }
    fclose(f);
}




void libgame_unloadtexture(gamestate* g, int index) {
    //minfo("unloading texture");
    if (g->txinfo[index].texture.id > 0) {
        UnloadTexture(g->txinfo[index].texture);
    }
}




void libgame_unloadtextures(gamestate* g) {
    //minfo("unloading textures");
    libgame_unloadtexture(g, TXHERO);
    libgame_unloadtexture(g, TX_DIRT_00);
    libgame_unloadtexture(g, TX_DIRT_01);
    libgame_unloadtexture(g, TX_DIRT_02);
    libgame_unloadtexture(g, TX_TILE_STONE_00);
    libgame_unloadtexture(g, TX_TILE_STONE_01);
    libgame_unloadtexture(g, TX_TILE_STONE_02);
    libgame_unloadtexture(g, TX_TILE_STONE_03);
    libgame_unloadtexture(g, TX_TILE_STONE_04);
    libgame_unloadtexture(g, TX_TILE_STONE_05);
    libgame_unloadtexture(g, TX_TILE_STONE_06);
    libgame_unloadtexture(g, TX_TILE_STONE_07);
    libgame_unloadtexture(g, TX_TILE_STONE_08);
    libgame_unloadtexture(g, TX_TILE_STONE_09);
    libgame_unloadtexture(g, TX_TILE_STONE_10);
    libgame_unloadtexture(g, TX_TILE_STONE_11);
    libgame_unloadtexture(g, TX_TILE_STONE_12);
    libgame_unloadtexture(g, TX_TILE_STONE_13);
    //libgame_unloadtexture(g, TX_TILE_STONE_14);
    libgame_unloadtexture(g, TXTORCH);
    libgame_unloadtexture(g, TXHEROSHADOW);
    libgame_unloadtexture(g, TXHEROWALK);
    libgame_unloadtexture(g, TXHEROWALKSHADOW);
    libgame_unloadtexture(g, TXHEROATTACK);
    libgame_unloadtexture(g, TXHEROATTACKSHADOW);
    libgame_unloadtexture(g, TXHEROJUMP);
    libgame_unloadtexture(g, TXHEROJUMPSHADOW);
    libgame_unloadtexture(g, TXHEROSPINDIE);
    libgame_unloadtexture(g, TXHEROSPINDIESHADOW);
    libgame_unloadtexture(g, TXHEROSOULDIE);
    libgame_unloadtexture(g, TXHEROSOULDIESHADOW);
    libgame_unloadtexture(g, TX_TILE_STONE_WALL_00);
    libgame_unloadtexture(g, TXSWORD);
    libgame_unloadtexture(g, TXORCIDLE);
    libgame_unloadtexture(g, TXORCIDLESHADOW);
    libgame_unloadtexture(g, TXORCWALK);
    libgame_unloadtexture(g, TXORCWALKSHADOW);
    libgame_unloadtexture(g, TXORCATTACK);
    libgame_unloadtexture(g, TXORCATTACKSHADOW);
    libgame_unloadtexture(g, TXORCJUMP);
    libgame_unloadtexture(g, TXORCJUMPSHADOW);
    libgame_unloadtexture(g, TXORCDIE);
    libgame_unloadtexture(g, TXORCDIESHADOW);
}




//void libgame_reloadtextures(gamestate* g) {
//    libgame_unloadtextures(g);
//    libgame_loadtextures(g);
//}




void libgame_init() {
    //minfo("libgame_init");
    g = gamestateinitptr(windowwidth, windowheight, targetwidth, targetheight);
    libgame_initsharedsetup(g);
}




const entityid
libgame_create_entity(gamestate* g, const char* name, const entitytype_t type, const int x, const int y) {
    entity_t* e = entity_create(name);
    if (!e) {
        //merror("could not create entity");
        return -1;
    }
    e->x = x;
    e->y = y;
    e->type = type;
    e->inventory = vectorentityid_new();
    vectorentityid_add(&g->entityids, e->id);
    tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
    vectorentityid_add(&t->entityids, e->id);
    hashtable_entityid_entity_insert(g->entities, e->id, e);
    return e->id;
}




void libgame_create_torch_spritegroup(gamestate* g, entityid id, const float off_x, const float off_y) {
    minfo("libgame_createtorchspritegroup begin");
    spritegroup_t* group = spritegroup_create(4);
    entity_t* torch_entity = hashtable_entityid_entity_get(g->entities, id);
    int keys[1] = {TXTORCH};
    for (int i = 0; i < 1; i++) {
        sprite* s =
            sprite_create(&g->txinfo[keys[i]].texture, g->txinfo[keys[i]].contexts, g->txinfo[keys[i]].num_frames);
        if (!s) {
            merror("could not create sprite");
        }
        spritegroup_add(group, s);
    }
    // the padding will be different for the torch
    // initialize the group current and dest
    const int tilesize = 8;
    const sprite* s = spritegroup_get(group, 0);
    const float ox = off_x;
    const float oy = off_y;
    //const float x = torch_entity->pos.x * tilesize + ox;
    const float x = torch_entity->x * tilesize + ox;
    //const float y = torch_entity->pos.y * tilesize + oy;
    const float y = torch_entity->y * tilesize + oy;
    Rectangle dest = {x, y, s->width, s->height};
    group->current = 0;
    group->dest = dest;
    // add the spritegroup to the hashtable
    hashtable_entityid_spritegroup_insert(g->spritegroups, id, group);
    minfo("libgame_createtorchspritegroup end");
}




// this code is ugly as fuck but it works
// the hero has a LOT of spritegroups
// not every entity will have this many sprites
// lets try using this as a basis to get a sprite in there
void libgame_create_herospritegroup(gamestate* g, entityid id) {
    minfo("libgame_create_herospritegroup begin");
    minfo("creating hero group...");
    spritegroup_t* hero_group = spritegroup_create(20);
    minfo("getting hero entity...");
    entity_t* hero = hashtable_entityid_entity_get(g->entities, id);
    int keys[12] = {TXHERO,
                    TXHEROSHADOW,
                    TXHEROWALK,
                    TXHEROWALKSHADOW,
                    TXHEROATTACK,
                    TXHEROATTACKSHADOW,
                    TXHEROJUMP,
                    TXHEROJUMPSHADOW,
                    TXHEROSPINDIE,
                    TXHEROSPINDIESHADOW,
                    TXHEROSOULDIE,
                    TXHEROSOULDIESHADOW};
    minfo("looping thru sprite keys...");
    for (int i = 0; i < 12; i++) {
        printf("i: %d\n", i);
        int txkey = keys[i];
        sprite* s = sprite_create(&g->txinfo[txkey].texture, g->txinfo[txkey].contexts, g->txinfo[txkey].num_frames);
        if (!s) {
            merror("could not create sprite");
        }
        spritegroup_add(hero_group, s);
    }
    // this is effectively how we will update the
    // sprite position in relation to the entity's
    // dungeon position
    const float w = spritegroup_get(hero_group, 0)->width;
    const float h = spritegroup_get(hero_group, 0)->height;
    const float offset_x = -12;
    const float offset_y = -12;
    // this is gross we can probably do this better
    //const float x = hero->pos.x * 8;
    const float x = hero->x * DEFAULT_TILE_SIZE;
    //const float y = hero->pos.y * 8;
    const float y = hero->y * DEFAULT_TILE_SIZE;
    Rectangle dest = {x + offset_x, y + offset_y, w, h};
    hero_group->current = 0;
    hero_group->dest = dest;
    hero_group->off_x = offset_x;
    hero_group->off_y = offset_y;
    // add the spritegroup to the hashtable
    minfo("inserting hero spritegroup into table...");
    hashtable_entityid_spritegroup_insert(g->spritegroups, g->hero_id, hero_group);
    msuccess("libgame_create_herospritegroup end");
}




void libgame_create_orcspritegroup(gamestate* g, entityid id) {
    minfo("libgame_create_orcspritegroup begin");
    spritegroup_t* orc_group = spritegroup_create(20);
    entity_t* orc = hashtable_entityid_entity_get(g->entities, id);
    int keys[14] = {TXORCIDLE,
                    TXORCIDLESHADOW,
                    TXORCWALK,
                    TXORCWALKSHADOW,
                    TXORCATTACK,
                    TXORCATTACKSHADOW,
                    TXORCCHARGEDATTACK,
                    TXORCCHARGEDATTACKSHADOW,
                    TXORCJUMP,
                    TXORCJUMPSHADOW,
                    TXORCDIE,
                    TXORCDIESHADOW,
                    TXORCDMG,
                    TXORCDMGSHADOW};
    for (int i = 0; i < 14; i++) {
        //for (int i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        sprite* s =
            sprite_create(&g->txinfo[keys[i]].texture, g->txinfo[keys[i]].contexts, g->txinfo[keys[i]].num_frames);
        if (!s) {
            merror("could not create sprite");
        }
        spritegroup_add(orc_group, s);
    }
    const float w = spritegroup_get(orc_group, 0)->width;
    const float h = spritegroup_get(orc_group, 0)->height;
    const float offset_x = -12;
    const float offset_y = -12;
    const float x = orc->x * DEFAULT_TILE_SIZE;
    const float y = orc->y * DEFAULT_TILE_SIZE;
    Rectangle dest = {x + offset_x, y + offset_y, w, h};
    orc_group->current = 0;
    orc_group->dest = dest;
    orc_group->off_x = offset_x;
    orc_group->off_y = offset_y;
    hashtable_entityid_spritegroup_insert(g->spritegroups, id, orc_group);
    msuccess("libgame_create_orcspritegroup end");
}




void libgame_create_sword_spritegroup(gamestate* g, entityid id, const float off_x, const float off_y) {
    minfo("libgame_create_sword_spritegroup begin");
    spritegroup_t* group = spritegroup_create(4);
    entity_t* e = hashtable_entityid_entity_get(g->entities, id);
    int keys[1] = {TXSWORD};
    for (int i = 0; i < 1; i++) {
        sprite* s =
            sprite_create(&g->txinfo[keys[i]].texture, g->txinfo[keys[i]].contexts, g->txinfo[keys[i]].num_frames);
        if (!s) {
            merror("could not create sprite");
        }
        spritegroup_add(group, s);
    }
    // the padding will be different for the torch
    // initialize the group current and dest
    //const int tilesize = DEFAULT_TILE_SIZE;
    const sprite* s = spritegroup_get(group, 0);
    //const float ox = 0;
    //const float oy = -2;
    //const float x = e->pos.x * tilesize + off_x;
    const float x = e->x * DEFAULT_TILE_SIZE + off_x;
    //const float y = e->pos.y * tilesize + off_y;
    const float y = e->y * DEFAULT_TILE_SIZE + off_y;
    Rectangle dest = {x, y, s->width, s->height};
    group->current = 0;
    group->dest = dest;
    group->off_x = off_x;
    group->off_y = off_y;
    // add the spritegroup to the hashtable
    hashtable_entityid_spritegroup_insert(g->spritegroups, id, group);
    msuccess("libgame_create_sword_spritegroup end");
}




void libgame_create_shield_spritegroup(gamestate* g, const entityid id, const int off_x, const int off_y) {
    minfo("libgame_create_shield_spritegroup begin");
    if (g) {
        spritegroup_t* group = spritegroup_create(4);
        if (!group) {
            merror("create shield spritegroup: could not create spritegroup");
            return;
        }
        entity_t* e = hashtable_entityid_entity_get(g->entities, id);
        if (!e) {
            merror("create shield spritegroup: could not get entity");
            return;
        }
        sprite* s =
            sprite_create(&g->txinfo[TXSHIELD].texture, g->txinfo[TXSHIELD].contexts, g->txinfo[TXSHIELD].num_frames);
        if (!s) {
            merror("could not create sprite");
        }
        spritegroup_add(group, s);
        const int x = e->x * DEFAULT_TILE_SIZE + off_x;
        const int y = e->y * DEFAULT_TILE_SIZE + off_y;
        Rectangle dest = {x, y, s->width, s->height};
        group->current = 0;
        group->dest = dest;
        group->off_x = off_x;
        group->off_y = off_y;
        hashtable_entityid_spritegroup_insert(g->spritegroups, id, group);
        msuccess("libgame_create_shield_spritegroup end");
    }
}




void libgame_loadtargettexture(gamestate* g) {
    if (g) {
        target = LoadRenderTexture(targetwidth, targetheight);
        target_src = (Rectangle){0, 0, target.texture.width, -target.texture.height};
        target_dest = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};
        SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
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




void libgame_init_dungeonfloor_random_tiles(gamestate* g) {
    minfo("libgame_init_dungeonfloor_random_tiles begin");
    if (g) {
        if (g->dungeonfloor) {
            libgame_init_dungeonfloor_random_tiles_unsafe(g);
            msuccess("libgame_init_dungeonfloor_random_tiles end");
        } else {
            merror("could not initialize dungeonfloor: dungeonfloor is NULL");
        }
    } else {
        merror("could not initialize dungeonfloor: gamestate is NULL");
    }
}




void libgame_init_dungeonfloor_random_tiles_unsafe(gamestate* g) {
    //tiletype_t start_type = TILETYPE_DIRT_00;
    tiletype_t start_type = TILETYPE_STONE_00;
    const tiletype_t end_type = TILETYPE_STONE_13;
    for (int i = 0; i < g->dungeonfloor->len; i++) {
        for (int j = 0; j < g->dungeonfloor->wid; j++) {
            dungeonfloor_set_tiletype_at_unsafe(g->dungeonfloor, GetRandomValue(start_type, end_type), i, j);
        }
    }
}




void libgame_init_dungeonfloor(gamestate* g) {
    minfo("libgame_init_dungeonfloor begin");
    if (g) {
        if (g->dungeonfloor) {
            minfo("setting tiles");
            libgame_init_dungeonfloor_random_tiles_unsafe(g);
        } else {
            merror("could not initialize dungeonfloor: dungeonfloor is NULL");
        }
    } else {
        merror("could not initialize dungeonfloor: gamestate is NULL");
    }
}




void libgame_init_datastructures(gamestate* g) {
    g->entityids = vectorentityid_create(DEFAULT_VECTOR_ENTITYID_SIZE);
    g->entities = hashtable_entityid_entity_create(DEFAULT_HASHTABLE_ENTITYID_ENTITY_SIZE);
    g->spritegroups = hashtable_entityid_spritegroup_create(DEFAULT_HASHTABLE_ENTITYID_SPRITEGROUP_SIZE);
    const tiletype_t base_type = TILETYPE_DIRT_00;
    const int w = 8;
    const int h = 4;
    g->dungeonfloor = create_dungeonfloor(w, h, base_type);
    if (!g->dungeonfloor) {
        merror("could not create dungeonfloor");
        // we could use an 'emergency shutdown' in case an error causes us
        // to need to 'panic' and force game close properly
    }
    // lets try setting some random tiles to different tile types
    libgame_init_dungeonfloor(g);
    //minfo("libgame_initdatastructures end");
}




void libgame_createitembytype(gamestate* g, const itemtype_t type, const int x, const int y) {
    minfo("libgame_createitembytype begin");
    switch (type) {
    case ITEM_TORCH: {
        const entityid torch_id = libgame_create_entity(g, "torch", ENTITY_ITEM, x, y);
        entity_t* torch = hashtable_entityid_entity_get(g->entities, torch_id);
        if (torch) {
            msuccess("torch entity created");
            torch->itemtype = ITEM_TORCH;
            libgame_create_torch_spritegroup(g, torch_id, 0, -DEFAULT_TILE_SIZE);
        }
    } break;
    default:
        break;
    }
    minfo("libgame_createitembytype end");
}




void libgame_create_hero(gamestate* g, const char* name, const int x, const int y) {
    const entityid id = libgame_create_entity(g, name, ENTITY_PLAYER, x, y);
    if (id != -1) {
        g->hero_id = id;
        entity_t* hero = hashtable_entityid_entity_get(g->entities, id);
        if (hero) {
            minfo("hero entity created");
            hero->race.primary = RACETYPE_HUMAN;
            hero->race.secondary = RACETYPE_NONE;
            libgame_create_herospritegroup(g, id);
        }
    }
}




void libgame_create_orc(gamestate* g, const char* name, const int x, const int y) {
    const entityid id = libgame_create_entity(g, name, ENTITY_NPC, x, y);
    if (id != -1) {
        entity_t* orc = hashtable_entityid_entity_get(g->entities, id);
        if (orc) {
            minfo("orc entity created");
            orc->race.primary = RACETYPE_ORC;
            orc->race.secondary = RACETYPE_NONE;
            libgame_create_orcspritegroup(g, id);
        }
    }
}




void libgame_create_sword(gamestate* g, const char* name, const int x, const int y) {
    const entityid id = libgame_create_entity(g, name, ENTITY_ITEM, x, y);
    if (id != -1) {
        entity_t* e = hashtable_entityid_entity_get(g->entities, id);
        if (e) {
            minfo("orc entity created");
            e->race.primary = RACETYPE_NONE;
            e->race.secondary = RACETYPE_NONE;
            e->itemtype = ITEM_WEAPON;
            e->weapontype = WEAPON_SWORD;
            libgame_create_sword_spritegroup(g, id, 0, -2);
        }
    }
}




void libgame_create_shield(gamestate* g, const char* name, const int x, const int y) {
    const entityid id = libgame_create_entity(g, name, ENTITY_ITEM, x, y);
    if (id != -1) {
        entity_t* e = hashtable_entityid_entity_get(g->entities, id);
        if (e) {
            minfo("shield entity created");
            e->race.primary = RACETYPE_NONE;
            e->race.secondary = RACETYPE_NONE;
            e->itemtype = ITEM_SHIELD;
            e->weapontype = WEAPON_NONE;
            e->shieldtype = SHIELD_BASIC;
            libgame_create_shield_spritegroup(g, id, 0, -2);
        }
    }
}




void libgame_initsharedsetup(gamestate* g) {
    minfo("libgame_initsharedsetup begin");
    if (g) {
        libgame_initwindow();
        libgame_loadfont(g);
        libgame_loadtargettexture(g);
        libgame_loadtextures(g);

        SetRandomSeed(time(NULL));
        libgame_init_datastructures(g);



        minfo("creating hero");
        // this is just a mock-up for now
        libgame_create_hero(g, "hero", 1, 0);
        msuccess("hero created");
        minfo("creating sword...");
        libgame_create_sword(g, "sword", 2, 0);
        msuccess("sword created");
        minfo("creating shield...");
        libgame_create_shield(g, "shield", 3, 0);
        msuccess("shield created");
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
    UnloadFont(g->font);
    libgame_unloadtextures(g);
    UnloadRenderTexture(target);
    CloseWindow();
    msuccess("libgame_closeshared end");
}




gamestate* libgame_getgamestate() {
    return g;
}




const bool libgame_entity_move(gamestate* g, entityid id, int x, int y) {
    minfo("libgame_entity_move");
    entity_t* e = hashtable_entityid_entity_get(g->entities, id);
    if (libgame_entity_move_check(g, e, x, y)) {
        // move successful
        // create a copy of the old pos
        const int old_x = e->x;
        const int old_y = e->y;
        entity_move(e, x, y);
        // we want to update the tile entityids
        tile_t* from_tile = dungeonfloor_get_tile(g->dungeonfloor, old_x, old_y);
        // remove the entityid from the old tile
        vectorentityid_remove_value(&from_tile->entityids, e->id);
        tile_t* to_tile = dungeonfloor_get_tile(g->dungeonfloor, e->x, e->y);
        // add the entityid to the new tile
        vectorentityid_add(&to_tile->entityids, e->id);
        msuccess("libgame_entity_move: move successful");
        return true;
    }
    // move unsuccessful
    minfo("libgame_entity_move: move unsuccessful");
    return false;
}




const bool libgame_is_tile_occupied_with_entitytype(gamestate* g, const entitytype_t type, const int x, const int y) {
    tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, x, y);
    if (t) {
        int count = vectorentityid_capacity(&t->entityids);
        // get is tile occupied
        for (int i = 0; i < count; i++) {
            entityid id = vectorentityid_get(&t->entityids, i);
            entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
            if (entity->type == type) {
                minfo("is_tile_occupied: tile occupied");
                fprintf(stdout, "entity type: %d at (%d, %d)\n", entity->type, x, y);
                return true;
            }
        }
    }
    return false;
}



const bool libgame_entity_move_check(gamestate* g, entity_t* e, int x, int y) {
    bool retval = false;
    if (e) {
        // check bounds
        if (e->x + x < 0 || e->x + x >= g->dungeonfloor->len) {
            merror("move_check: out of bounds x");
            retval = false;
        } else if (e->y + y < 0 || e->y + y >= g->dungeonfloor->wid) {
            merror("move_check: out of bounds y");
            retval = false;
        } else {
            bool r0 = !libgame_is_tile_occupied_with_entitytype(g, ENTITY_PLAYER, e->x + x, e->y + y);
            bool r1 = !libgame_is_tile_occupied_with_entitytype(g, ENTITY_NPC, e->x + x, e->y + y);
            return r0 && r1;
        }
    } else {
        merror("move_check: entity is NULL");
        retval = false;
    }
    return retval;
}




bool libgame_external_check_reload() {
    return IsKeyPressed(KEY_R);
}
