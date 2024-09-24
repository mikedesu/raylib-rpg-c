#include "controlmode.h"
#include "dungeonfloor.h"
#include "entity.h"
#include "fadestate.h"
#include "gamestate.h"
#include "hashtable_entityid_entity.h"
#include "hashtable_entityid_spritegroup.h"
#include "libgame_defines.h"
#include "mprint.h"
#include "setdebugpanel.h"
#include "sprite.h"
#include "spritegroup.h"
#include "textureinfo.h"
#include "utils.h"
#include "vectorentityid.h"

#include <raylib.h>
#include <rlgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define BASE_SIZE 8
#define TEST_SIZE 0.2
#define FRAMEINTERVAL 10


//--------------------------------------------------------------------
// libgame global variables
//--------------------------------------------------------------------
char debugpanelbuffer[1024] = {0};

int activescene = GAMEPLAYSCENE;

gamestate* g = NULL;

Font gfont;

RenderTexture target;
Rectangle target_src;
Rectangle target_dest;
Vector2 target_origin = (Vector2){0, 0};

int targetwidth = DEFAULT_TARGET_WIDTH;
int targetheight = DEFAULT_TARGET_HEIGHT;
int windowwidth = DEFAULT_WINDOW_WIDTH;
int windowheight = DEFAULT_WINDOW_HEIGHT;

entityid hero_id;

vectorentityid_t entityids;

hashtable_entityid_entity_t* entities = NULL;

hashtable_entityid_spritegroup_t* spritegroups = NULL;

textureinfo txinfo[20];

dungeonfloor_t* dungeonfloor = NULL;

//--------------------------------------------------------------------
// function declarations
//--------------------------------------------------------------------
bool libgame_windowshouldclose();

gamestate* libgame_getgamestate();

void libgame_close();
void libgame_initwindow();
void libgame_closewindow();
void libgame_updatedebugpanelbuffer();
void libgame_updategamestate();
void libgame_drawframe();
void libgame_init();
void libgame_handleinput();
void libgame_loadtexture(
    gamestate* g, int index, int contexts, int frames, bool dodither, const char* path);
void libgame_unloadtexture(gamestate* g, int index);
void libgame_unloadtextures(gamestate* g);
void libgame_loadtextures(gamestate* g);
void libgame_loadtexturesfromfile(gamestate* g, const char* path);
void libgame_handlereloadtextures(gamestate* g);
void libgame_reloadtextures(gamestate* g);
void libgame_closeshared(gamestate* g);
void libgame_closesavegamestate();
void libgame_drawdebugpanel(gamestate* g);
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
void libgame_createherospritegroup(gamestate* g);
void libgame_createheroentity(gamestate* g);
void libgame_drawherogroup(gamestate* g);
void libgame_drawherogrouphitbox(gamestate* g);
void libgame_loadtargettexture(gamestate* g);
void libgame_loadfont(gamestate* g);
void libgame_initdatastructures(gamestate* g);

entityid libgame_createentity(gamestate* g, const char* name);

//--------------------------------------------------------------------
// definitions
//--------------------------------------------------------------------
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


void libgame_handlereloadtextures(gamestate* g) {
    if (IsKeyPressed(KEY_R)) {
        libgame_reloadtextures(g);
    }
}


void libgame_handleinput() {
    if (IsKeyPressed(KEY_SPACE)) {
        minfo("key space pressed");
        //if (g->fadestate == FADESTATENONE) {
        //    g->fadestate = FADESTATEOUT;
        //}
    }

    if (IsKeyPressed(KEY_A)) {
        // increment the 'current' of the hero group
        spritegroup_t* hero_group = hashtable_entityid_spritegroup_search(spritegroups, hero_id);
        spritegroup_incr(hero_group);
    }

    libgame_handlereloadtextures(g);
    libgame_handlemodeswitch(g);
    libgame_handledebugpanelswitch(g);
    libgame_handleplayerinput(g);
    libgame_handlecaminput(g);
}


void libgame_handlemodeswitch(gamestate* g) {
    if (IsKeyPressed(KEY_C)) {
        switch (g->controlmode) {
        case CONTROLMODE_CAMERA:
            minfo("control mode: camera");
            g->controlmode = CONTROLMODE_PLAYER;
            break;
        default:
            minfo("control mode: player");
            g->controlmode = CONTROLMODE_CAMERA;
            break;
        }
    }
}


void libgame_handledebugpanelswitch(gamestate* g) {
    if (IsKeyPressed(KEY_D)) {
        g->debugpanelon = !g->debugpanelon;
    }
}


void libgame_handleplayerinput(gamestate* g) {
    if (g->controlmode == CONTROLMODE_PLAYER) {
        const bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        // this is just a test
        // the real setup will involve managing the player's dungeon position
        // and then translating that into a destination on screen
        if (IsKeyPressed(KEY_RIGHT)) {
            spritegroup_t* hero_group =
                hashtable_entityid_spritegroup_search(spritegroups, hero_id);
            if (hero_group) {
                int cc = hero_group->sprites[hero_group->current]->currentcontext;
                int context = cc == SG_CTX_R_D   ? SG_CTX_R_D
                              : cc == SG_CTX_L_D ? SG_CTX_R_D
                              : cc == SG_CTX_R_U ? SG_CTX_R_U
                                                 : SG_CTX_R_U;
                spritegroup_setcontexts(hero_group, context);
                hero_group->move = (Vector2){8, 0};
                hero_group->current = 2;
            }

        } else if (IsKeyPressed(KEY_LEFT)) {
            //hero_group->dest.x -= BASE_SIZE;

            spritegroup_t* hero_group =
                hashtable_entityid_spritegroup_search(spritegroups, hero_id);
            if (hero_group) {

                int cc = hero_group->sprites[hero_group->current]->currentcontext;
                int context = cc == SG_CTX_R_D   ? SG_CTX_L_D
                              : cc == SG_CTX_L_D ? SG_CTX_L_D
                              : cc == SG_CTX_R_U ? SG_CTX_L_U
                                                 : SG_CTX_L_U;
                spritegroup_setcontexts(hero_group, context);
                hero_group->move = (Vector2){-8, 0};
                hero_group->current = 2;
            }
        }

        if (IsKeyPressed(KEY_DOWN)) {
            //hero_group->dest.y += BASE_SIZE;
            spritegroup_t* hero_group =
                hashtable_entityid_spritegroup_search(spritegroups, hero_id);
            if (hero_group) {

                int cc = hero_group->sprites[hero_group->current]->currentcontext;
                int context = cc == SG_CTX_R_D   ? SG_CTX_R_D
                              : cc == SG_CTX_L_D ? SG_CTX_L_D
                              : cc == SG_CTX_R_U ? SG_CTX_R_D
                                                 : SG_CTX_L_D;
                spritegroup_setcontexts(hero_group, context);
                hero_group->move = (Vector2){0, 8};
                hero_group->current = 2;
            }

        } else if (IsKeyPressed(KEY_UP)) {
            //hero_group->dest.y -= BASE_SIZE;
            spritegroup_t* hero_group =
                hashtable_entityid_spritegroup_search(spritegroups, hero_id);
            if (hero_group) {

                int cc = hero_group->sprites[hero_group->current]->currentcontext;
                int context = cc == SG_CTX_R_D   ? SG_CTX_R_U
                              : cc == SG_CTX_L_D ? SG_CTX_L_U
                              : cc == SG_CTX_R_U ? SG_CTX_R_U
                                                 : SG_CTX_L_U;
                spritegroup_setcontexts(hero_group, context);
                hero_group->move = (Vector2){0, -8};
                hero_group->current = 2;
            }
        }


        if (IsKeyPressed(KEY_PERIOD)) {
            spritegroup_t* hero_group =
                hashtable_entityid_spritegroup_search(spritegroups, hero_id);
            if (hero_group) {
                hero_group->current = 0;
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
    minfo("begin libgame_initwindow");
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
    minfo("end of libgame_initwindow");
}


void libgame_closewindow() {
    //rlglClose();
    CloseWindow();
}


void libgame_updatedebugpanelbuffer() {
    //spritegroup_t* hero_group = hashtable_entityid_spritegroup_search(spritegroups, hero_id);

    snprintf(debugpanelbuffer,
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
             "Control mode: %d\n",
             //"Herogroup current: %d\n"
             //"Herogroup size: %d\n"
             //"Herogroup capacity: %d\n",
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
             g->controlmode

    );
}


void libgame_updategamestate() {
    libgame_updatedebugpanelbuffer();
    //setdebugpanelcenter(g);

    // smooth movement:
    // lets try using the  move vector on our spritegroup
    // to move the spritegroup dest rectangle
    // so that we can begin translating
    // player input
    // into real moves
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_search(spritegroups, hero_id);
    if (hero_group) {

        // only do it 1 unit at a time
        if (hero_group->move.x > 0) {
            hero_group->dest.x++;
            hero_group->move.x--;
        } else if (hero_group->move.x < 0) {
            hero_group->dest.x--;
            hero_group->move.x++;
        }

        if (hero_group->move.y > 0) {
            hero_group->dest.y++;
            hero_group->move.y--;
        } else if (hero_group->move.y < 0) {
            hero_group->dest.y--;
            hero_group->move.y++;
        }
    }

    if (g->cam_lockon) {
        g->cam2d.target = (Vector2){hero_group->dest.x, hero_group->dest.y};
    }

    //g->cam2d.rotation += 0.5f;
}


void libgame_drawframeend(gamestate* g) {
    EndDrawing();
    g->framecount++;
    gamestateupdatecurrenttime(g);
}


void libgame_drawframe() {
    BeginDrawing();
    BeginTextureMode(target);

    if (activescene == 0) {
        libgame_drawcompanyscene(g);
    } else if (activescene == 1) {
        libgame_drawtitlescene(g);
    } else if (activescene == 2) {
        libgame_drawgameplayscene(g);
    }

    EndTextureMode();
    DrawTexturePro(target.texture, target_src, target_dest, target_origin, 0.0f, WHITE);
    libgame_drawdebugpanel(g);
    libgame_drawframeend(g);
}


inline void libgame_drawdebugpanel(gamestate* g) {
    if (g->debugpanelon) {
        const int fontsize = 14;
        const int spacing = 1;
        Vector2 p = {g->debugpanel.x, g->debugpanel.y};
        Vector2 m = MeasureTextEx(gfont, debugpanelbuffer, fontsize, spacing);
        // update the debug panel width and height
        // this code will go elsewhere like an 'update debug panel' method
        // to separate the debug panel update from the draw
        // we store the root measurement because
        // the box is drawn relative to
        // the size of the text
        g->debugpanel.w = m.x;
        g->debugpanel.h = m.y;

        int xy = 10;
        int wh = 20;
        Rectangle box = {p.x - xy, p.y - xy, m.x + wh, m.y + wh};

        //DrawRectangle(bx, by, bw, bh, (Color){0x33, 0x33, 0x33, 128});
        DrawRectanglePro(box, (Vector2){0.0f, 0.0f}, 0.0f, (Color){0x33, 0x33, 0x33, 128});

        DrawTextEx(gfont, debugpanelbuffer, p, fontsize, spacing, WHITE);
    }
}


void libgame_drawgrid(gamestate* g) {
    const int w = txinfo[TXDIRT].texture.width;
    const int h = txinfo[TXDIRT].texture.height;
    Color c = GREEN;
    for (int i = 0; i <= dungeonfloor->len; i++) {
        DrawLine(i * w, 0, i * w, dungeonfloor->wid * h, c);
    }

    for (int i = 0; i <= dungeonfloor->wid; i++) {
        DrawLine(0, i * h, dungeonfloor->len * w, i * h, c);
    }
}


void libgame_drawdungeonfloor(gamestate* g) {
    const int w = txinfo[TXDIRT].texture.width;
    const int h = txinfo[TXDIRT].texture.height;
    Rectangle tile_src = {0, 0, w, h};
    Rectangle tile_dest = {0, 0, w, h};
    Color c = WHITE;
    float rotation = 0;
    Vector2 origin = {0, 0};
    for (int i = 0; i < dungeonfloor->len; i++) {
        for (int j = 0; j < dungeonfloor->wid; j++) {
            tile_dest.x = i * w;
            tile_dest.y = j * h;
            //DrawTexturePro(textures[TXDIRT], tile_src, tile_dest, origin, rotation, c);
            DrawTexturePro(txinfo[TXDIRT].texture, tile_src, tile_dest, origin, rotation, c);
        }
    }
}


void libgame_drawherogrouphitbox(gamestate* g) {
    spritegroup_t* sg = hashtable_entityid_spritegroup_search(spritegroups, hero_id);
    if (sg && g->debugpanelon) {
        Color c = (Color){51, 51, 51, 255};
        Vector2 v[4] = {{sg->dest.x, sg->dest.y},
                        {sg->dest.x + sg->dest.width, sg->dest.y},
                        {sg->dest.x + sg->dest.width, sg->dest.y + sg->dest.height},
                        {sg->dest.x, sg->dest.y + sg->dest.height}};
        DrawLineV(v[0], v[1], c);
        DrawLineV(v[1], v[2], c);
        DrawLineV(v[2], v[3], c);
        DrawLineV(v[3], v[0], c);
    }
}


void libgame_drawherogroup(gamestate* g) {
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_search(spritegroups, hero_id);
    if (hero_group) {
        // draw hero and its shadow
        DrawTexturePro(*hero_group->sprites[hero_group->current]->texture,
                       hero_group->sprites[hero_group->current]->src,
                       hero_group->dest,
                       (Vector2){0, 0},
                       0.0f,
                       (Color){255, 255, 255, 255});

        DrawTexturePro(*hero_group->sprites[hero_group->current + 1]->texture,
                       hero_group->sprites[hero_group->current + 1]->src,
                       hero_group->dest,
                       (Vector2){0, 0},
                       0.0f,
                       WHITE);
    }


    if (g->framecount % FRAMEINTERVAL == 0) {
        sprite_incrframe(hero_group->sprites[hero_group->current]);
    }
}


void libgame_drawgameplayscene(gamestate* g) {
    BeginMode2D(g->cam2d);
    ClearBackground(BLACK);
    const int w = txinfo[TXDIRT].texture.width;
    const int h = txinfo[TXDIRT].texture.height;

    Rectangle tile_src = {0, 0, w, h};
    Rectangle tile_dest = {0, 0, w, h};
    Vector2 origin = {0, 0};
    Color c = WHITE;
    Color border0 = RED;
    Color border1 = GREEN;
    float rotation = 0;

    // draw the dungeon floor
    libgame_drawdungeonfloor(g);

    // we want to draw a grid of lines on top of the tiles
    // previously we were drawing rectanglelines around each tile
    // but this looked ugly
    // instead
    // lets go row by row, column by column
    // and draw lines
    if (g->debugpanelon) {
        libgame_drawgrid(g);
    }


    libgame_drawherogroup(g);

    libgame_drawherogrouphitbox(g);

    //spritegroup_t* hero_group = hashtable_entityid_spritegroup_search(spritegroups, hero_id);
    //if (hero_group) {
    //    // draw hero and its shadow
    //    DrawTexturePro(*hero_group->sprites[hero_group->current]->texture,
    //                   hero_group->sprites[hero_group->current]->src,
    //                   hero_group->dest,
    //                   origin,
    //                   rotation,
    //                   (Color){255, 255, 255, 255});
    //    DrawTexturePro(*hero_group->sprites[hero_group->current + 1]->texture,
    //                   hero_group->sprites[hero_group->current + 1]->src,
    //                   hero_group->dest,
    //                   origin,
    //                   rotation,
    //                   c);
    //    if (g->debugpanelon) {
    //        // do the rectangle using lines
    //        DrawLine(hero_group->dest.x,
    //                 hero_group->dest.y,
    //                 hero_group->dest.x + hero_group->dest.width,
    //                 hero_group->dest.y,
    //                 (Color){51, 51, 51, 255});
    //
    //            DrawLine(hero_group->dest.x,
    //                     hero_group->dest.y,
    //                     hero_group->dest.x,
    //                     hero_group->dest.y + hero_group->dest.height,
    //                     (Color){51, 51, 51, 255});
    //
    //            DrawLine(hero_group->dest.x + hero_group->dest.width,
    //                     hero_group->dest.y,
    //                     hero_group->dest.x + hero_group->dest.width,
    //                     hero_group->dest.y + hero_group->dest.height,
    //                     (Color){51, 51, 51, 255});
    //
    //            DrawLine(hero_group->dest.x,
    //                     hero_group->dest.y + hero_group->dest.height,
    //                     hero_group->dest.x + hero_group->dest.width,
    //                     hero_group->dest.y + hero_group->dest.height,
    //                     (Color){51, 51, 51, 255});
    //        }
    //if (g->framecount % FRAMEINTERVAL == 0) {
    //    sprite_incrframe(hero_group->sprites[hero_group->current]);
    //}
    //    }

    EndMode2D();

    libgame_handlefade(g);

    // we will want to find a way to wrap animation management
    // this will get cumbersome as the # of sprites on-screen grows.
    // will also need a way to manage the existing sprites on screen
    // previously i used an unordered map in C++ to do this
    // i will need to find a way to do this in C
    //
    // we could rawdog an array of sprite pointers or something but we will deal with it
    // when we get there
}


void libgame_drawtitlescene(gamestate* g) {
    //BeginDrawing();
    const Color bgc = {0x66, 0x66, 0x66, 255};
    const Color fgc = WHITE;
    const Color fgc2 = BLACK;
    char b[128];
    char b2[128];
    char b3[128];
    snprintf(b, 128, "project");
    snprintf(b2, 128, "rpg");
    snprintf(b3, 128, "press space to continue");

    Vector2 m = MeasureTextEx(gfont, b, 40, 2);
    Vector2 m2 = MeasureTextEx(gfont, b2, 40, 2);
    Vector2 m3 = MeasureTextEx(gfont, b3, 16, 1);

    float tw2 = targetwidth / 2.0f;
    float th2 = targetheight / 2.0f;
    int offset = 100;

    int x = tw2 - m.x / 2.0f - offset;
    int y = th2 - m.y / 2.0f;
    int x2 = tw2 - m2.x / 2.0f + offset;
    //int y2 = th2 / 2.0f - m2.y / 2.0f;
    int x3 = tw2 - m3.x / 2.0f;
    int y3 = th2 + m3.y / 2.0f + 20;

    Vector2 pos = {x, y};
    Vector2 pos2 = {x2, y};
    Vector2 pos3 = {x3, y3};

    ClearBackground(bgc);

    //DrawRectangleLines(x - 5, y - 5, m.x + 20, m.y + 20, fgc);
    DrawTextEx(gfont, b, pos, 40, 4, fgc);

    //DrawRectangleLines(x2 - 5, y - 5, m2.x + 20, m2.y + 20, fgc2);
    DrawTextEx(gfont, b2, pos2, 40, 1, fgc2);

    // just below the 'project rpg' text
    DrawTextEx(gfont, b3, pos3, 16, 1, fgc);


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
    const Vector2 measure = MeasureTextEx(gfont, b, fontsize, spacing);
    const Vector2 measure2 = MeasureTextEx(gfont, b2, fontsize, spacing);

    if (g->framecount % interval >= 0 && g->framecount % interval < dur) {
        for (int i = 0; i < 10; i++) {
            shufflestrinplace(b);
            shufflestrinplace(b3);
        }
    }
    for (int i = 0; i < 10; i++) {
        shufflestrinplace(b2);
    }

    const Vector2 pos = {targetwidth / 2.0f - measure.x / 2.0f,
                         targetheight / 2.0f - measure.y / 2.0f};
    ClearBackground(bgc);
    DrawTextEx(gfont, b, pos, fontsize, 1, fgc);
    DrawTextEx(gfont, b2, pos, fontsize, 1, fgc);

    const Vector2 measure3 = MeasureTextEx(gfont, b3, 20, 1);
    const Vector2 pp = {targetwidth / 2.0f - measure3.x / 2.0f,
                        targetheight / 2.0f + measure.y / 2.0f + 20};

    DrawTextEx(gfont, b3, pp, 20, 1, fgc);

    libgame_handlefade(g);
}


void libgame_loadtexture(
    gamestate* g, int index, int contexts, int frames, bool dodither, const char* path) {
    if (dodither) {
        Image img = LoadImage(path);
        ImageDither(&img, 4, 4, 4, 4);

        Texture2D t = LoadTextureFromImage(img);


        txinfo[index].texture = t;
        UnloadImage(img);
    } else {
        Texture2D t = LoadTexture(path);

        txinfo[index].texture = t;
    }
    txinfo[index].num_frames = frames;
    txinfo[index].contexts = contexts;
    //SetTextureFilter(textures[index], TEXTURE_FILTER_POINT);
    //SetTextureFilter(txinfo[index].texture, TEXTURE_FILTER_POINT);
}


void libgame_loadtextures(gamestate* g) {
    libgame_loadtexturesfromfile(g, "textures.txt");
}


void libgame_loadtexturesfromfile(gamestate* g, const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) {
        mprint("could not open file");
        return;
    }

    char line[256];
    char txpath[256];
    int index = 0;
    int contexts = 0;
    int frames = 0;
    int dodither = 0;

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
    minfo("unloading texture");

    if (txinfo[index].texture.id > 0) {
        UnloadTexture(txinfo[index].texture);
    }
}


void libgame_unloadtextures(gamestate* g) {
    minfo("unloading textures");

    libgame_unloadtexture(g, TXHERO);
    libgame_unloadtexture(g, TXDIRT);
}


void libgame_reloadtextures(gamestate* g) {
    minfo("reloading textures");

    libgame_unloadtextures(g);
    libgame_loadtextures(g);
}


void libgame_init() {
    minfo("libgame_init");

    g = gamestateinitptr(windowwidth, windowheight, targetwidth, targetheight);

    libgame_initsharedsetup(g);
}


entityid libgame_createentity(gamestate* g, const char* name) {
    entity_t* e = entity_create(name);
    if (!e) {
        merror("could not create entity");
        return -1;
    }

    vectorentityid_pushback(&entityids, e->id);
    hashtable_entityid_entity_insert(entities, e->id, e);
    return e->id;
}


void libgame_createheroentity(gamestate* g) {
    entityid heroid = libgame_createentity(g, "hero");
    if (heroid != -1) {
        hero_id = heroid;
    }
}


void libgame_createherospritegroup(gamestate* g) {
    spritegroup_t* hero_group = spritegroup_create(20);

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
            &txinfo[keys[i]].texture, txinfo[keys[i]].contexts, txinfo[keys[i]].num_frames);
        if (!s) {
            merror("could not create sprite");
        }
        spritegroup_add(hero_group, s);
    }

    const float x = 0;
    const float y = 0;
    const float w = spritegroup_get(hero_group, 0)->width;
    const float h = spritegroup_get(hero_group, 0)->height;
    const float offset_x = -w / 2 + w / 8;
    const float offset_y = -h / 2 + h / 8;
    Rectangle dest = {x + offset_x, y + offset_y, w, h};
    hero_group->current = 0;
    hero_group->dest = dest;

    // add the spritegroup to the hashtable
    hashtable_entityid_spritegroup_insert(spritegroups, hero_id, hero_group);
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
    gfont = LoadFontEx(DEFAULT_FONT_PATH, fontsize, 0, codepointct);
}


void libgame_initdatastructures(gamestate* g) {
    entityids = vectorentityid_create(DEFAULT_VECTOR_ENTITYID_SIZE);
    entities = hashtable_entityid_entity_create(DEFAULT_HASHTABLE_ENTITYID_ENTITY_SIZE);
    spritegroups =
        hashtable_entityid_spritegroup_create(DEFAULT_HASHTABLE_ENTITYID_SPRITEGROUP_SIZE);
    dungeonfloor = create_dungeonfloor(8, 8, TILETYPE_DIRT);
    if (!dungeonfloor) {
        merror("could not create dungeonfloor");
        // we could use an 'emergency shutdown' in case an error causes us
        // to need to 'panic' and force game close properly
    }
}


void libgame_initsharedsetup(gamestate* g) {
    if (g) {
        libgame_initwindow();
        libgame_loadfont(g);
        libgame_loadtargettexture(g);
        libgame_loadtextures(g);
        libgame_initdatastructures(g);
        libgame_createheroentity(g);
        libgame_createherospritegroup(g);
        setdebugpaneltopleft(g);

    } else {
        merror("libgame_initsharedsetup: gamestate is NULL");
    }
}


void libgame_initwithstate(gamestate* state) {
    if (state == NULL) {
        merror("libgame_initwithstate: state is NULL");
        return;
    }
    minfo("libgame_initwithstate");
    g = state;
    libgame_initsharedsetup(g);
}


void libgame_closesavegamestate() {
    minfo("libgame_closesavegamestate");
    libgame_closeshared(g);
}


void libgame_close() {
    minfo("libgame_close");
    libgame_closeshared(g);
    gamestatefree(g);
}


void libgame_closeshared(gamestate* g) {
    // not right now, but when we add dungeonfloor to
    // the gamestate, we will be able to avoid freeing
    // it on every reload
    hashtable_entityid_entity_destroy(entities);

    hashtable_entityid_spritegroup_destroy(spritegroups);

    vectorentityid_destroy(&entityids);

    dungeonfloor_free(dungeonfloor);

    UnloadFont(gfont);

    libgame_unloadtextures(g);

    minfo("unloading render texture");
    UnloadRenderTexture(target);

    minfo("closing window");
    CloseWindow();
}


gamestate* libgame_getgamestate() {
    return g;
}
