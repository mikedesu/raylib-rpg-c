#include "controlmode.h"
#include "dungeonfloor.h"
#include "entity.h"
#include "fadestate.h"
#include "gamestate.h"
#include "hashtable_entityid_entity.h"
#include "hashtable_entityid_spritegroup.h"
#include "itemtype.h"
#include "libgame_defines.h"
#include "mprint.h"
#include "scene.h"
#include "setdebugpanel.h"
#include "sprite.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"
#include "textureinfo.h"
#include "utils.h"
#include "vectorentityid.h"




#include <raylib.h>
#include <rlgl.h>
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




//------------------------------------------------------------------
// function declarations
//------------------------------------------------------------------
bool libgame_windowshouldclose();
gamestate* libgame_getgamestate();
void libgame_initwindow();
void libgame_closewindow();
void libgame_init();
bool libgame_external_check_reload();

const bool libgame_entity_move(gamestate* g, entityid id, int x, int y);
const bool libgame_entity_move_check(gamestate* g, entity_t* e, int x, int y);
const bool libgame_entityid_move_check(gamestate* g, entityid id, const Vector2 dir);

const entityid libgame_createentity(gamestate* g, const char* name, entitytype_t type, Vector2 pos);
const entityid libgame_createtorchentity(gamestate* g);

void libgame_updatedebugpanelbuffer(gamestate* g);
void libgame_updategamestate(gamestate* g);
void libgame_close(gamestate* g);
void libgame_drawframe(gamestate* g);
void libgame_handleinput(gamestate* g);
void libgame_loadtexture(gamestate* g, int index, int contexts, int frames, bool dodither, const char* path);
void libgame_unloadtexture(gamestate* g, int index);
void libgame_unloadtextures(gamestate* g);
void libgame_loadtextures(gamestate* g);
void libgame_loadtexturesfromfile(gamestate* g, const char* path);
//void libgame_reloadtextures(gamestate* g);
void libgame_closeshared(gamestate* g);
void libgame_closesavegamestate();

void libgame_drawdebugpanel(gamestate* g);

void libgame_drawgameplayscene_messagelog(gamestate* g);

void libgame_drawcompanyscene(gamestate* g);
void libgame_drawtitlescene(gamestate* g);
void libgame_drawgameplayscene(gamestate* g);
void libgame_drawfade(gamestate* g);
void libgame_drawgrid(gamestate* g);
void libgame_drawdungeonfloor(gamestate* g);
void libgame_initwithstate(gamestate* state);
void libgame_initsharedsetup(gamestate* g);
void libgame_drawframeend(gamestate* g);
void libgame_handleplayerinput(gamestate* g);
void libgame_handlecaminput(gamestate* g);
void libgame_handledebugpanelswitch(gamestate* g);
void libgame_handlemodeswitch(gamestate* g);
void libgame_handlefade(gamestate* g);
void libgame_createherospritegroup(gamestate* g, entityid id);
void libgame_loadtargettexture(gamestate* g);
void libgame_loadfont(gamestate* g);
void libgame_initdatastructures(gamestate* g);
void libgame_updateherospritegroup_right(gamestate* g);
void libgame_updateherospritegroup_left(gamestate* g);
void libgame_updateherospritegroup_up(gamestate* g);
void libgame_updateherospritegroup_down(gamestate* g);
void libgame_createtorchspritegroup(gamestate* g, entityid id);
void libgame_drawtorchgroup(gamestate* g);
void libgame_updatesmoothmove(gamestate* g);
void libgame_docameralockon(gamestate* g);
void libgame_do_one_camera_rotation(gamestate* g);
void libgame_update_spritegroup_move(entityid id, int x, int y);
void libgame_createitembytype(gamestate* g, itemtype_t type, Vector2 pos);
void libgame_drawentity(gamestate* g, entityid id);
void libgame_entity_anim_incr(entityid id);
void libgame_calc_debugpanel_size(gamestate* g);
void libgame_createhero(gamestate* g);
void libgame_handleplayerinput_key_right(gamestate* g);
void libgame_handleplayerinput_key_left(gamestate* g);
void libgame_handleplayerinput_key_down(gamestate* g);
void libgame_handleplayerinput_key_up(gamestate* g);
void libgame_handleplayerinput_key_down_left(gamestate* g);
void libgame_handleplayerinput_key_up_left(gamestate* g);
void libgame_handleplayerinput_key_up_right(gamestate* g);
void libgame_handleplayerinput_key_down_right(gamestate* g);
void libgame_initdungeonfloor(gamestate* g);




void libgame_drawfade(gamestate* g) {
    if (g->fadealpha > 0) {
        Color c = {0, 0, 0, g->fadealpha};
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), c);
    }
}




void libgame_handlefade(gamestate* g) {
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
    libgame_drawfade(g);
}




void libgame_entity_anim_incr(entityid id) {
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (group) {
        spritegroup_incr(group);
    }
}




void libgame_handleinput(gamestate* g) {
    //if (IsKeyPressed(KEY_SPACE)) {
    //minfo("key space pressed");
    //if (g->fadestate == FADESTATENONE) {
    //    g->fadestate = FADESTATEOUT;
    //}
    //g->do_one_rotation = true;
    //}

    //if (IsKeyPressed(KEY_A)) {
    //    libgame_entity_anim_incr(g->hero_id);
    //}

    if (IsKeyPressed(KEY_T)) {
        // lets place a torch where the player is standing
        entity_t* hero = hashtable_entityid_entity_get(g->entities, g->hero_id);
        if (hero) {
            // check to see if there are any items at that location
            tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, hero->pos);
            if (t) {
                for (int i = 0; i < vectorentityid_capacity(&t->entityids); i++) {
                    entityid id = vectorentityid_get(&t->entityids, i);
                    entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
                    if (entity->type == ENTITY_ITEM && entity->itemtype == ITEM_TORCH) {
                        minfo("torch already exists at this location");
                    } else {
                        libgame_createitembytype(g, ITEM_TORCH, hero->pos);
                    }
                }
            }
        }
    }

    libgame_handlemodeswitch(g);
    libgame_handledebugpanelswitch(g);
    libgame_handleplayerinput(g);
    libgame_handlecaminput(g);
}




void libgame_handlemodeswitch(gamestate* g) {
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




void libgame_handledebugpanelswitch(gamestate* g) {
    if (IsKeyPressed(KEY_D)) {
        minfo("debug panel switch");
        g->debugpanelon = !g->debugpanelon;
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
        hero_group->current = SPRITEGROUP_ANIM_WALK;
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
        hero_group->current = SPRITEGROUP_ANIM_WALK;
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
        hero_group->current = SPRITEGROUP_ANIM_WALK;
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
        hero_group->current = SPRITEGROUP_ANIM_WALK;
    }
}




void libgame_update_spritegroup_move(entityid id, int x, int y) {
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(g->spritegroups, id);
    if (sg) {
        sg->move = (Vector2){x, y};
    }
}




void libgame_handleplayerinput_key_right(gamestate* g) {
    libgame_updateherospritegroup_right(g);
    bool result = libgame_entity_move(g, g->hero_id, 1, 0);
    if (result) {
        libgame_update_spritegroup_move(g->hero_id, 8, 0);
    }
}




void libgame_handleplayerinput_key_left(gamestate* g) {
    libgame_updateherospritegroup_left(g);
    bool result = libgame_entity_move(g, g->hero_id, -1, 0);
    if (result) {
        libgame_update_spritegroup_move(g->hero_id, -8, 0);
    }
}




void libgame_handleplayerinput_key_down(gamestate* g) {
    libgame_updateherospritegroup_down(g);
    bool result = libgame_entity_move(g, g->hero_id, 0, 1);
    if (result) {
        libgame_update_spritegroup_move(g->hero_id, 0, 8);
    }
}




void libgame_handleplayerinput_key_up(gamestate* g) {
    libgame_updateherospritegroup_up(g);
    bool result = libgame_entity_move(g, g->hero_id, 0, -1);
    if (result) {
        libgame_update_spritegroup_move(g->hero_id, 0, -8);
    }
}




void libgame_handleplayerinput_key_down_left(gamestate* g) {
    libgame_updateherospritegroup_left(g);
    bool result = libgame_entity_move(g, g->hero_id, -1, 1);
    if (result) {
        libgame_update_spritegroup_move(g->hero_id, -8, 8);
    }
}



void libgame_handleplayerinput_key_down_right(gamestate* g) {
    libgame_updateherospritegroup_right(g);
    bool result = libgame_entity_move(g, g->hero_id, 1, 1);
    if (result) {
        libgame_update_spritegroup_move(g->hero_id, 8, 8);
    }
}


void libgame_handleplayerinput_key_up_left(gamestate* g) {
    libgame_updateherospritegroup_left(g);
    bool result = libgame_entity_move(g, g->hero_id, -1, -1);
    if (result) {
        libgame_update_spritegroup_move(g->hero_id, -8, -8);
    }
}


void libgame_handleplayerinput_key_up_right(gamestate* g) {
    libgame_updateherospritegroup_right(g);
    bool result = libgame_entity_move(g, g->hero_id, 1, -1);
    if (result) {
        libgame_update_spritegroup_move(g->hero_id, 8, -8);
    }
}



void libgame_handleplayerinput(gamestate* g) {
    if (g->controlmode == CONTROLMODE_PLAYER) {
        //const bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        // this is just a test
        // the real setup will involve managing the player's dungeon position
        // and then translating that into a destination on screen

        // left-handed controls
        // eventually we will create a mapping for custom controls
        if (IsKeyPressed(KEY_KP_6)) {

            libgame_handleplayerinput_key_right(g);

        } else if (IsKeyPressed(KEY_KP_4)) {

            libgame_handleplayerinput_key_left(g);

        } else if (IsKeyPressed(KEY_KP_2)) {

            libgame_handleplayerinput_key_down(g);

        } else if (IsKeyPressed(KEY_KP_8)) {

            libgame_handleplayerinput_key_up(g);

        } else if (IsKeyPressed(KEY_KP_1)) {

            libgame_handleplayerinput_key_down_left(g);
        } else if (IsKeyPressed(KEY_KP_3)) {

            minfo("key KP 3 pressed");
            libgame_handleplayerinput_key_down_right(g);
        } else if (IsKeyPressed(KEY_KP_7)) {

            minfo("key KP 7 pressed");
            libgame_handleplayerinput_key_up_left(g);
        } else if (IsKeyPressed(KEY_KP_9)) {
            minfo("key KP 9 pressed");
            libgame_handleplayerinput_key_up_right(g);
        }

        else if (IsKeyPressed(KEY_P)) {

            minfo("key P pressed");
            // we are going to attempt to pick up any torch on the tile
            entity_t* hero = hashtable_entityid_entity_get(g->entities, g->hero_id);
            tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, hero->pos);
            if (hero && t) {
                minfo("hero and tile pointers acquired, entering loop...");
                for (int i = 0; i < vectorentityid_capacity(&t->entityids); i++) {
                    entityid id = vectorentityid_get(&t->entityids, i);
                    entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
                    if (entity && entity->type == ENTITY_ITEM && entity->itemtype == ITEM_TORCH) {
                        minfo("torch found on tile, removing...");
                        // we are going to remove the torch from the tile
                        // and add it to the player's inventory
                        vectorentityid_remove_value(&t->entityids, id);

                        // add the torch to the player's inventory
                        vectorentityid_add(&hero->inventory, id);
                    } else {
                        merror("no torch found on tile");
                    }
                }
            }
        }



        else if (IsKeyPressed(KEY_PERIOD)) {
            spritegroup_t* hero_group = hashtable_entityid_spritegroup_get(g->spritegroups, g->hero_id);
            if (hero_group) {
                hero_group->current = 0; //standing/idle
            }
        }
    }
}




void libgame_handlecaminput(gamestate* g) {
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
        }

        if (IsKeyDown(KEY_LEFT)) {
            g->cam2d.offset.x += cam_move_incr;
        } else if (IsKeyDown(KEY_RIGHT)) {
            g->cam2d.offset.x -= cam_move_incr;
        }
    }
}




bool libgame_windowshouldclose() {
    return WindowShouldClose();
}




void libgame_initwindow() {
    ////minfo("begin libgame_initwindow");
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




void libgame_updatedebugpanelbuffer(gamestate* g) {
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
             "Hero position: %.0f,%.0f\n"
             "Inventory capacity: %ld\n"

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
             hero->pos.x,
             hero->pos.y,

             vectorentityid_capacity(&hero->inventory)


    );
}




void libgame_updatesmoothmove(gamestate* g) {
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_get(g->spritegroups, g->hero_id);

    if (hero_group) {
        float move_unit = 1.0f;
        // only do it 1 unit at a time
        if (hero_group->move.x > 0) {
            hero_group->dest.x += move_unit;
            hero_group->move.x -= move_unit;
        } else if (hero_group->move.x < 0) {
            hero_group->dest.x -= move_unit;
            hero_group->move.x += move_unit;
        }

        if (hero_group->move.y > 0) {
            hero_group->dest.y += move_unit;
            hero_group->move.y -= move_unit;
        } else if (hero_group->move.y < 0) {
            hero_group->dest.y -= move_unit;
            hero_group->move.y += move_unit;
        }

        if (hero_group->move.x == 0.0f && hero_group->move.y == 0.0f) {
            entity_t* hero = hashtable_entityid_entity_get(g->entities, g->hero_id);
            if (hero) {
                hero_group->dest.x = hero->pos.x * 8 - 12;
                hero_group->dest.y = hero->pos.y * 8 - 12;
            }
        }
    }
}




void libgame_docameralockon(gamestate* g) {
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




void libgame_updategamestate(gamestate* g) {
    libgame_updatedebugpanelbuffer(g);
    //setdebugpanelcenter(g);
    libgame_updatesmoothmove(g);

    libgame_docameralockon(g);
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
        libgame_drawgameplayscene(g);
        break;
    default:
        break;
    }

    EndTextureMode();
    DrawTexturePro(target.texture, target_src, target_dest, target_origin, 0.0f, WHITE);

    libgame_drawgameplayscene_messagelog(g);

    libgame_drawdebugpanel(g);

    libgame_drawframeend(g);
}



void libgame_calc_debugpanel_size(gamestate* g) {
    const int sz = 14, sp = 1;
    const Vector2 m = MeasureTextEx(g->font, g->debugpanel.buffer, sz, sp);
    g->debugpanel.w = m.x, g->debugpanel.h = m.y;
}




inline void libgame_drawgameplayscene_messagelog(gamestate* g) {
    if (g) {
        //const int fontsize = 14, spacing = 1, xy = 10, wh = 20;
        const char* text = "you have entered the dungeon\nmessages will appear here\n";
        const int x = 20;
        const int y = 20;
        const int w = 300;
        const int h = 300;
        //const int pad = 5;
        const Rectangle box1 = {x, y, w, h};
        const Rectangle box2 = {x, y, box1.width, box1.height};
        const Vector2 origin = {0, 0};
        const Vector2 text_origin = {30, 30};

        DrawRectanglePro(box1, origin, 0.0f, (Color){0x33, 0x33, 0x33, 0xFF});
        DrawRectangleLinesEx(box2, 1, WHITE);
        DrawTextEx(g->font, text, text_origin, 14, 1, WHITE);
    }
}




inline void libgame_drawdebugpanel(gamestate* g) {
    if (g && g->debugpanelon) {
        //const int fontsize = 14, spacing = 1, xy = 10, wh = 20;
        const int fontsize = 14, spacing = 1, xy = 10, wh = 20;
        const Vector2 p = {g->debugpanel.x, g->debugpanel.y}, o = {0, 0};
        const Rectangle box = {
            g->debugpanel.x - xy, g->debugpanel.y - xy, g->debugpanel.w + wh, g->debugpanel.h + wh};
        DrawRectanglePro(box, o, 0.0f, (Color){0x33, 0x33, 0x33, 128});
        DrawTextEx(g->font, g->debugpanel.buffer, p, fontsize, spacing, WHITE);
    }
}




void libgame_drawgrid(gamestate* g) {
    Color c = GREEN;
    //const int w = g->txinfo[TXDIRT].texture.width, h = g->txinfo[TXDIRT].texture.height;
    const int w = 8, h = 8;
    const int len = g->dungeonfloor->len;
    const int wid = g->dungeonfloor->wid;
    for (int i = 0; i <= len; i++) {
        DrawLine(i * w, 0, i * w, wid * h, c);
    }
    for (int i = 0; i <= wid; i++) {
        DrawLine(0, i * h, len * w, i * h, c);
    }
}




void libgame_drawdungeonfloor(gamestate* g) {
    //const int w = g->txinfo[TXDIRT].texture.width, h = g->txinfo[TXDIRT].texture.height;
    const int w = 8, h = 8;
    //const int w = 16, h = 16;
    Rectangle tile_src = {0, 0, w, h}, tile_dest = {0, 0, w, h};
    Color c = WHITE;
    float rotation = 0;
    Vector2 origin = {0, 0};
    for (int i = 0; i < g->dungeonfloor->len; i++) {
        for (int j = 0; j < g->dungeonfloor->wid; j++) {
            // get the tile
            tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, (Vector2){i, j});
            // check the tile type
            tile_dest.x = i * w;
            tile_dest.y = j * h;

            int key = -1;

            // this is very inefficient
            // we want a quick way to map
            // tiletypes to texture keys
            //
            // texture keys should prob be an enum
            // instead of macros
            //
            // if we have one texture per tile type
            // we can have a direct mapping
            //
            // we need this so we can draw the same texture
            // for the tile
            // on every round
            //
            // another possibility
            // is that we can
            // attach the texture key
            // to the tile
            // and just do a lookup on that
            switch (t->type) {

            case TILETYPE_DIRT_00:
                key = TX_DIRT_00;
                break;


            case TILETYPE_DIRT_01:
                key = TX_DIRT_01;
                break;

            case TILETYPE_DIRT_02:
                key = TX_DIRT_02;
                break;

            case TILETYPE_STONE_00:
                key = TX_TILE_STONE_00;
                break;


            case TILETYPE_STONE_01:
                key = TX_TILE_STONE_01;
                break;

            case TILETYPE_STONE_02:
                key = TX_TILE_STONE_02;
                break;

            case TILETYPE_STONE_03:
                key = TX_TILE_STONE_03;
                break;

            case TILETYPE_STONE_04:
                key = TX_TILE_STONE_04;
                break;

            case TILETYPE_STONE_05:
                key = TX_TILE_STONE_05;
                break;

            case TILETYPE_STONE_06:
                key = TX_TILE_STONE_06;
                break;

            case TILETYPE_STONE_07:
                key = TX_TILE_STONE_07;
                break;

            case TILETYPE_STONE_08:
                key = TX_TILE_STONE_08;
                break;

            case TILETYPE_STONE_09:
                key = TX_TILE_STONE_09;
                break;

            case TILETYPE_STONE_10:
                key = TX_TILE_STONE_10;
                break;

            case TILETYPE_STONE_11:
                key = TX_TILE_STONE_11;
                break;

            case TILETYPE_STONE_12:
                key = TX_TILE_STONE_12;
                break;

            case TILETYPE_STONE_13:
                key = TX_TILE_STONE_13;
                break;

                //case TILETYPE_STONE_14:
                //    key = TX_TILE_STONE_14;
                //    break;

            case TILETYPE_STONE_WALL_00:
                key = TX_TILE_STONE_WALL_00;
                break;



            default:
                break;
            }

            if (key != -1) {
                DrawTexturePro(g->txinfo[key].texture, tile_src, tile_dest, origin, rotation, c);
            }
        }
    }
}




// we are going to have to temporarily save the torchid
// as we are not enumerating thru the map yet
void libgame_drawtorchgroup(gamestate* g) {
    spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, g->torch_id);
    if (group) {
        DrawTexturePro(*group->sprites[group->current]->texture,
                       group->sprites[group->current]->src,
                       group->dest,
                       (Vector2){0, 0},
                       0.0f,
                       WHITE);
    }

    if (g->framecount % FRAMEINTERVAL == 0) {
        sprite_incrframe(group->sprites[group->current]);
    }
}




void libgame_drawentity(gamestate* g, entityid id) {
    if (g) {
        spritegroup_t* group = hashtable_entityid_spritegroup_get(g->spritegroups, id);
        if (group) {
            DrawTexturePro(*group->sprites[group->current]->texture,
                           group->sprites[group->current]->src,
                           group->dest,
                           (Vector2){0, 0},
                           0.0f,
                           WHITE);

            if (g->framecount % FRAMEINTERVAL == 0) {
                sprite_incrframe(group->sprites[group->current]);
            }

            if (g->debugpanelon) {
                Color c = (Color){51, 51, 51, 255};
                Vector2 v[4] = {{group->dest.x, group->dest.y},
                                {group->dest.x + group->dest.width, group->dest.y},
                                {group->dest.x + group->dest.width, group->dest.y + group->dest.height},
                                {group->dest.x, group->dest.y + group->dest.height}};
                DrawLineV(v[0], v[1], c);
                DrawLineV(v[1], v[2], c);
                DrawLineV(v[2], v[3], c);
                DrawLineV(v[3], v[0], c);
            }
        }
    }
}




void libgame_drawgameplayscene(gamestate* g) {
    //minfo("libgame_drawgameplayscene begin");
    BeginMode2D(g->cam2d);
    ClearBackground(BLACK);

    libgame_drawdungeonfloor(g);

    if (g->debugpanelon) {
        libgame_drawgrid(g);
        //libgame_drawgrid_thick(g, 2);
    }

    // this code needs to be wrapped-up so that we are drawing all of the entities that are on-screen
    // this is essentially 'hard-coded' right now
    // we want to generalize this
    // will need to close and re-open the game to re-fresh the game state after we write this
    // but i want to see the torches i just made
    //libgame_drawtorchgroup(g);
    //libgame_drawentity(g, g->torch_id);

    // we want to iterate over every entityid in gamestate->entityids

    // lets iterate thru the dungeonfloor tiles
    // we can utilize that function we just wrote

    for (int i = 0; i < g->dungeonfloor->len; i++) {
        for (int j = 0; j < g->dungeonfloor->wid; j++) {
            tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, (Vector2){i, j});

            // draw entities on the tile

            // draw torches etc first
            for (int k = 0; k < vectorentityid_capacity(&t->entityids); k++) {
                entityid id = vectorentityid_get(&t->entityids, k);
                entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
                if (entity->type == ENTITY_ITEM && entity->itemtype == ITEM_TORCH) {
                    libgame_drawentity(g, id);
                }
            }

            // draw players and other entities
            for (int k = 0; k < vectorentityid_capacity(&t->entityids); k++) {
                entityid id = vectorentityid_get(&t->entityids, k);
                entity_t* entity = hashtable_entityid_entity_get(g->entities, id);
                if (entity->type == ENTITY_PLAYER) {
                    libgame_drawentity(g, id);
                }
            }
        }
    }


    libgame_handlefade(g);

    EndMode2D();

    //DrawRectangle(0, 0, 100, 100, BLACK);
    //DrawRectangleLines(10, 10, 80, 80, WHITE);
    //DrawTextEx(g->font, "message log", (Vector2){20, 20}, 12, 1, WHITE);
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
    libgame_handlefade(g);
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
    libgame_handlefade(g);
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
    libgame_loadtexturesfromfile(g, "textures.txt");
}




void libgame_loadtexturesfromfile(gamestate* g, const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) {
        //mprint("could not open file");
        return;
    }

    int index = 0, contexts = 0, frames = 0, dodither = 0;
    char line[256], txpath[256];

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




const entityid libgame_createentity(gamestate* g, const char* name, entitytype_t type, Vector2 pos) {
    entity_t* e = entity_create(name);
    if (!e) {
        //merror("could not create entity");
        return -1;
    }
    e->pos.x = pos.x;
    e->pos.y = pos.y;
    e->type = type;
    e->inventory = vectorentityid_new();

    vectorentityid_add(&g->entityids, e->id);
    tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, pos);
    //vectorentityid_pushback_unique(&g->dungeonfloor->grid[(int)pos.x][(int)pos.y]->entityids, e->id);
    vectorentityid_add(&t->entityids, e->id);
    hashtable_entityid_entity_insert(g->entities, e->id, e);
    return e->id;
}




const entityid libgame_createtorchentity(gamestate* g) {
    entityid torch_id = libgame_createentity(g, "torch", ENTITY_ITEM, (Vector2){0, 0});
    return torch_id;
}




void libgame_createtorchspritegroup(gamestate* g, entityid id) {
    minfo("libgame_createtorchspritegroup begin");
    spritegroup_t* group = spritegroup_create(4);
    entity_t* torch_entity = hashtable_entityid_entity_get(g->entities, id);
    int keys[1] = {TXTORCH};
    for (int i = 0; i < 1; i++) {
        sprite* s = sprite_create(
            &g->txinfo[keys[i]].texture, g->txinfo[keys[i]].contexts, g->txinfo[keys[i]].num_frames);
        if (!s) {
            merror("could not create sprite");
        }
        spritegroup_add(group, s);
    }

    // the padding will be different for the torch
    // initialize the group current and dest
    const int tilesize = 8;
    const sprite* s = spritegroup_get(group, 0);
    const float ox = 0;
    const float oy = -s->height / 2.0f;
    const float x = torch_entity->pos.x * tilesize + ox;
    const float y = torch_entity->pos.y * tilesize + oy;
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
void libgame_createherospritegroup(gamestate* g, entityid id) {
    spritegroup_t* hero_group = spritegroup_create(20);
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

    for (int i = 0; i < 12; i++) {
        sprite* s = sprite_create(
            &g->txinfo[keys[i]].texture, g->txinfo[keys[i]].contexts, g->txinfo[keys[i]].num_frames);
        if (!s) {
            //merror("could not create sprite");
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
    const float x = hero->pos.x * 8;
    const float y = hero->pos.y * 8;
    Rectangle dest = {x + offset_x, y + offset_y, w, h};
    hero_group->current = 0;
    hero_group->dest = dest;

    // add the spritegroup to the hashtable
    hashtable_entityid_spritegroup_insert(g->spritegroups, g->hero_id, hero_group);
}




void libgame_loadtargettexture(gamestate* g) {
    target = LoadRenderTexture(targetwidth, targetheight);
    target_src = (Rectangle){0, 0, target.texture.width, -target.texture.height};
    target_dest = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
}




void libgame_loadfont(gamestate* g) {
    const int fontsize = 60;
    const int codepointct = 256;
    g->font = LoadFontEx(DEFAULT_FONT_PATH, fontsize, 0, codepointct);
}




const char* get_str_for_tiletype(tiletype_t type) {
    switch (type) {
    case TILETYPE_DIRT_00:
        return "dirt 00";
    case TILETYPE_DIRT_01:
        return "dirt 01";
    case TILETYPE_DIRT_02:
        return "dirt 02";
    case TILETYPE_STONE_00:
        return "stone 00";
    case TILETYPE_STONE_01:
        return "stone 01";
    case TILETYPE_STONE_02:
        return "stone 02";
    case TILETYPE_STONE_03:
        return "stone 03";
    case TILETYPE_STONE_04:
        return "stone 04";
    case TILETYPE_STONE_05:
        return "stone 05";
    case TILETYPE_STONE_06:
        return "stone 06";
    case TILETYPE_STONE_07:
        return "stone 07";
    case TILETYPE_STONE_08:
        return "stone 08";
    case TILETYPE_STONE_09:
        return "stone 09";
    case TILETYPE_STONE_10:
        return "stone 10";
    case TILETYPE_STONE_11:
        return "stone 11";
    case TILETYPE_STONE_12:
        return "stone 12";
    case TILETYPE_STONE_13:
        return "stone 13";
    case TILETYPE_STONE_14:
        return "stone 14";
    case TILETYPE_STONE_WALL_00:
        return "stone wall 00";
    default:
        return "unknown";
    }
}




void libgame_initdungeonfloor(gamestate* g) {
    minfo("libgame_initdungeonfloor begin");
    if (g->dungeonfloor) {
        minfo("setting random tiles");
        tiletype_t start_type = TILETYPE_STONE_00;
        const tiletype_t end_type = TILETYPE_STONE_13;
        printf("start type: %s\n", get_str_for_tiletype(start_type));
        printf("end type: %s\n", get_str_for_tiletype(end_type));
        for (int i = 0; i < g->dungeonfloor->wid; i++) {
            tile_t* t = dungeonfloor_get_tile(g->dungeonfloor, (Vector2){i, 0});
            t->type = start_type;
            start_type++;
            if (start_type > end_type) {
                start_type = TILETYPE_STONE_00;
            }
        }
    }
}



void libgame_initdatastructures(gamestate* g) {
    //minfo("libgame_initdatastructures begin");
    g->entityids = vectorentityid_create(DEFAULT_VECTOR_ENTITYID_SIZE);
    g->entities = hashtable_entityid_entity_create(DEFAULT_HASHTABLE_ENTITYID_ENTITY_SIZE);

    g->spritegroups = hashtable_entityid_spritegroup_create(DEFAULT_HASHTABLE_ENTITYID_SPRITEGROUP_SIZE);

    const tiletype_t base_type = TILETYPE_DIRT_00;
    //const tiletype_t base_type = TILETYPE_STONE_00;
    const int w = 16;
    const int h = 16;
    g->dungeonfloor = create_dungeonfloor(w, h, base_type);
    if (!g->dungeonfloor) {
        //merror("could not create dungeonfloor");
        // we could use an 'emergency shutdown' in case an error causes us
        // to need to 'panic' and force game close properly
    }

    // lets try setting some random tiles to different tile types
    libgame_initdungeonfloor(g);

    //minfo("libgame_initdatastructures end");
}




//void libgame_createitembytype(gamestate* g, itemtype_t type, int x, int y) {
void libgame_createitembytype(gamestate* g, itemtype_t type, Vector2 pos) {
    minfo("libgame_createitembytype begin");
    switch (type) {
    case ITEM_TORCH: {
        entityid torch_id = libgame_createentity(g, "torch", ENTITY_ITEM, pos);
        entity_t* torch = hashtable_entityid_entity_get(g->entities, torch_id);
        if (torch) {
            minfo("torch entity created");
            torch->type = ENTITY_ITEM;
            torch->itemtype = ITEM_TORCH;
            libgame_createtorchspritegroup(g, torch_id);
        }


    } break;
    default:
        break;
    }
    minfo("libgame_createitembytype end");
}




void libgame_createhero(gamestate* g) {
    entityid id = libgame_createentity(g, "hero", ENTITY_PLAYER, (Vector2){0, 1});
    if (id != -1) {
        g->hero_id = id;
        entity_t* hero = hashtable_entityid_entity_get(g->entities, id);
        if (hero) {
            minfo("hero entity created");
            hero->type = ENTITY_PLAYER;

            libgame_createherospritegroup(g, id);
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
        libgame_initdatastructures(g);

        // this is just a mock-up for now
        libgame_createhero(g);

        // these dont work right until the text buffer of the debugpanel is filled
        libgame_updatedebugpanelbuffer(g);
        libgame_calc_debugpanel_size(g);
        setdebugpanelbottomleft(g);
        //setdebugpaneltopright(g);
        //setdebugpanelbottomright(g);
        //setdebugpanelbottomleft(g, g->display.targetheight);

    } else {
        merror("libgame_initsharedsetup: gamestate is NULL");
    }
}




void libgame_initwithstate(gamestate* state) {
    if (state == NULL) {
        //merror("libgame_initwithstate: state is NULL");
        return;
    }
    //minfo("libgame_initwithstate");
    g = state;
    libgame_initsharedsetup(g);
}




void libgame_closesavegamestate() {
    //minfo("libgame_closesavegamestate");
    libgame_closeshared(g);
}




void libgame_close(gamestate* g) {
    //minfo("libgame_close");
    libgame_closeshared(g);
    gamestatefree(g);
}




void libgame_closeshared(gamestate* g) {
    // dont need to free most of gamestate
    UnloadFont(g->font);
    libgame_unloadtextures(g);

    //minfo("unloading render texture");
    UnloadRenderTexture(target);

    //minfo("closing window");
    CloseWindow();
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
        Vector2 old_pos = e->pos;
        entity_move(e, (Vector2){x, y});

        // we want to update the tile entityids
        tile_t* from_tile = dungeonfloor_get_tile(g->dungeonfloor, old_pos);
        // remove the entityid from the old tile
        vectorentityid_remove_value(&from_tile->entityids, e->id);

        tile_t* to_tile = dungeonfloor_get_tile(g->dungeonfloor, e->pos);
        // add the entityid to the new tile
        vectorentityid_add(&to_tile->entityids, e->id);

        return true;
    }
    // move unsuccessful
    return false;
}




const bool libgame_entityid_move_check(gamestate* g, entityid id, const Vector2 dir) {
    entity_t* e = hashtable_entityid_entity_get(g->entities, id);
    return libgame_entity_move_check(g, e, dir.x, dir.y);
}



const bool libgame_entity_move_check(gamestate* g, entity_t* e, int x, int y) {
    bool retval = false;
    if (e) {
        // check bounds
        if (e->pos.x + x < 0 || e->pos.x + x >= g->dungeonfloor->len) {
            merror("move_check: out of bounds x");
            retval = false;
        } else if (e->pos.y + y < 0 || e->pos.y + y >= g->dungeonfloor->wid) {
            merror("move_check: out of bounds y");
            retval = false;
        } else {
            minfo("move_check: move successful");
            retval = true;
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
