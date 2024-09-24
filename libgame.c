#include "controlmode.h"
#include "dungeonfloor.h"
#include "entity.h"
#include "fadestate.h"
#include "gamestate.h"
#include "hashtable_entityid_entity.h"
#include "hashtable_entityid_spritegroup.h"
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

#define BASE_SIZE 8
#define TEST_SIZE 0.2
#define FRAMEINTERVAL 10

//--------------------------------------------------------------------
// libgame global variables
//--------------------------------------------------------------------

gamestate* g = NULL;

RenderTexture target;
Rectangle target_src;
Rectangle target_dest;
Vector2 target_origin = (Vector2){0, 0};

int activescene = GAMEPLAYSCENE;
int targetwidth = DEFAULT_TARGET_WIDTH;
int targetheight = DEFAULT_TARGET_HEIGHT;
int windowwidth = DEFAULT_WINDOW_WIDTH;
int windowheight = DEFAULT_WINDOW_HEIGHT;

vectorentityid_t entityids;

hashtable_entityid_entity_t* entities = NULL;

hashtable_entityid_spritegroup_t* spritegroups = NULL;

//textureinfo txinfo[20];

dungeonfloor_t* dungeonfloor = NULL;

//--------------------------------------------------------------------
// function declarations
//--------------------------------------------------------------------
bool libgame_windowshouldclose();
gamestate* libgame_getgamestate();
void libgame_initwindow();
void libgame_closewindow();
void libgame_init();

void libgame_updatedebugpanelbuffer(gamestate* g);
void libgame_updategamestate(gamestate* g);
void libgame_close(gamestate* g);
void libgame_drawframe(gamestate* g);
void libgame_handleinput(gamestate* g);
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
void libgame_updateherospritegroup_right(gamestate* g);
void libgame_updateherospritegroup_left(gamestate* g);
void libgame_updateherospritegroup_up(gamestate* g);
void libgame_updateherospritegroup_down(gamestate* g);

entityid libgame_createentity(gamestate* g, const char* name);


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
    //minfo("begin libgame_handlereloadtextures");
    if (IsKeyPressed(KEY_R)) {
        libgame_reloadtextures(g);
    }
}


void libgame_handleinput(gamestate* g) {
    if (IsKeyPressed(KEY_SPACE)) {
        minfo("key space pressed");
        //if (g->fadestate == FADESTATENONE) {
        //    g->fadestate = FADESTATEOUT;
        //}
    }

    if (IsKeyPressed(KEY_A)) {
        // increment the 'current' of the hero group
        spritegroup_t* hero_group = hashtable_entityid_spritegroup_search(spritegroups, g->hero_id);
        spritegroup_incr(hero_group);
    }

    libgame_handlereloadtextures(g);
    libgame_handlemodeswitch(g);
    libgame_handledebugpanelswitch(g);
    libgame_handleplayerinput(g);
    libgame_handlecaminput(g);
}


void libgame_handlemodeswitch(gamestate* g) {
    //minfo("begin libgame_handlemodeswitch");
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
    //minfo("begin libgame_handledebugpanelswitch");
    if (IsKeyPressed(KEY_D)) {
        g->debugpanelon = !g->debugpanelon;
    }
}


void libgame_updateherospritegroup_right(gamestate* g) {
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_search(spritegroups, g->hero_id);
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
        hero_group->move = (Vector2){8, 0};
        hero_group->current = SPRITEGROUP_ANIM_WALK;
    }
}


void libgame_updateherospritegroup_left(gamestate* g) {
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_search(spritegroups, g->hero_id);
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
        hero_group->move = (Vector2){-8, 0};
        hero_group->current = SPRITEGROUP_ANIM_WALK;
    }
}


void libgame_updateherospritegroup_up(gamestate* g) {
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_search(spritegroups, g->hero_id);
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
        //int cc = hero_group->sprites[hero_group->current]->currentcontext;
        //int context = cc == SG_CTX_R_D   ? SG_CTX_R_U
        //              : cc == SG_CTX_L_D ? SG_CTX_L_U
        //              : cc == SG_CTX_R_U ? SG_CTX_R_U
        //                                 : SG_CTX_L_U;
        spritegroup_setcontexts(hero_group, ctx);
        hero_group->move = (Vector2){0, -8};
        hero_group->current = SPRITEGROUP_ANIM_WALK;
    }
}


void libgame_updateherospritegroup_down(gamestate* g) {
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_search(spritegroups, g->hero_id);
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

        //int cc = hero_group->sprites[hero_group->current]->currentcontext;
        //int context = cc == SG_CTX_R_D   ? SG_CTX_R_D
        //              : cc == SG_CTX_L_D ? SG_CTX_L_D
        //              : cc == SG_CTX_R_U ? SG_CTX_R_D
        //                                 : SG_CTX_L_D;
        spritegroup_setcontexts(hero_group, ctx);
        hero_group->move = (Vector2){0, 8};
        hero_group->current = SPRITEGROUP_ANIM_WALK;
    }
}

void libgame_handleplayerinput(gamestate* g) {
    //minfo("begin libgame_handleplayerinput");
    if (g->controlmode == CONTROLMODE_PLAYER) {
        const bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        // this is just a test
        // the real setup will involve managing the player's dungeon position
        // and then translating that into a destination on screen
        if (IsKeyPressed(KEY_RIGHT)) {
            libgame_updateherospritegroup_right(g);

        } else if (IsKeyPressed(KEY_LEFT)) {
            libgame_updateherospritegroup_left(g);
        }

        if (IsKeyPressed(KEY_DOWN)) {
            libgame_updateherospritegroup_down(g);

        } else if (IsKeyPressed(KEY_UP)) {
            libgame_updateherospritegroup_up(g);
        }


        if (IsKeyPressed(KEY_PERIOD)) {
            spritegroup_t* hero_group =
                hashtable_entityid_spritegroup_search(spritegroups, g->hero_id);
            if (hero_group) {
                hero_group->current = 0; //standing/idle
            }
        }
    }
}


void libgame_handlecaminput(gamestate* g) {
    //minfo("begin libgame_handlecaminput");
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
    minfo("end of libgame_initwindow");
}


void libgame_closewindow() {
    //rlglClose();
    CloseWindow();
}


void libgame_updatedebugpanelbuffer(gamestate* g) {
    //spritegroup_t* hero_group = hashtable_entityid_spritegroup_search(spritegroups, hero_id);

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


void libgame_updategamestate(gamestate* g) {
    libgame_updatedebugpanelbuffer(g);
    //setdebugpanelcenter(g);

    // smooth movement:
    // lets try using the  move vector on our spritegroup
    // to move the spritegroup dest rectangle
    // so that we can begin translating
    // player input
    // into real moves
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_search(spritegroups, g->hero_id);
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
    libgame_drawdebugpanel(g);
    libgame_drawframeend(g);
}


inline void libgame_drawdebugpanel(gamestate* g) {
    if (g->debugpanelon) {
        const int fontsize = 14;
        const int spacing = 1;
        const int xy = 10;
        const int wh = 20;
        const Vector2 p = {g->debugpanel.x, g->debugpanel.y};
        const Vector2 m = MeasureTextEx(g->font, g->debugpanel.buffer, fontsize, spacing);
        const Rectangle box = {p.x - xy, p.y - xy, m.x + wh, m.y + wh};
        // update the debug panel width and height
        // we store the root measurement because
        // the box is drawn relative to
        // the size of the text
        g->debugpanel.w = m.x;
        g->debugpanel.h = m.y;
        DrawRectanglePro(box, (Vector2){0.0f, 0.0f}, 0.0f, (Color){0x33, 0x33, 0x33, 128});
        DrawTextEx(g->font, g->debugpanel.buffer, p, fontsize, spacing, WHITE);
    }
}


void libgame_drawgrid(gamestate* g) {
    const int w = g->txinfo[TXDIRT].texture.width;
    const int h = g->txinfo[TXDIRT].texture.height;
    for (int i = 0; i <= dungeonfloor->len; i++)
        DrawLine(i * w, 0, i * w, dungeonfloor->wid * h, GREEN);
    for (int i = 0; i <= dungeonfloor->wid; i++)
        DrawLine(0, i * h, dungeonfloor->len * w, i * h, GREEN);
}


void libgame_drawdungeonfloor(gamestate* g) {
    const int w = g->txinfo[TXDIRT].texture.width;
    const int h = g->txinfo[TXDIRT].texture.height;
    Rectangle tile_src = {0, 0, w, h};
    Rectangle tile_dest = {0, 0, w, h};
    Color c = WHITE;
    float rotation = 0;
    Vector2 origin = {0, 0};
    for (int i = 0; i < dungeonfloor->len; i++) {
        for (int j = 0; j < dungeonfloor->wid; j++) {
            tile_dest.x = i * w;
            tile_dest.y = j * h;
            DrawTexturePro(g->txinfo[TXDIRT].texture, tile_src, tile_dest, origin, rotation, c);
        }
    }
}


void libgame_drawherogrouphitbox(gamestate* g) {
    spritegroup_t* sg = hashtable_entityid_spritegroup_search(spritegroups, g->hero_id);
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
    spritegroup_t* hero_group = hashtable_entityid_spritegroup_search(spritegroups, g->hero_id);
    if (hero_group) {
        // draw hero and its shadow
        DrawTexturePro(*hero_group->sprites[hero_group->current]->texture,
                       hero_group->sprites[hero_group->current]->src,
                       hero_group->dest,
                       (Vector2){0, 0},
                       0.0f,
                       WHITE);

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

    libgame_drawdungeonfloor(g);

    if (g->debugpanelon) {
        libgame_drawgrid(g);
    }

    libgame_drawherogroup(g);
    libgame_drawherogrouphitbox(g);
    libgame_handlefade(g);

    EndMode2D();
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
    const Vector2 measure2 = MeasureTextEx(g->font, b2, fontsize, spacing);

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
    DrawTextEx(g->font, b, pos, fontsize, 1, fgc);
    DrawTextEx(g->font, b2, pos, fontsize, 1, fgc);

    const Vector2 measure3 = MeasureTextEx(g->font, b3, 20, 1);
    const Vector2 pp = {targetwidth / 2.0f - measure3.x / 2.0f,
                        targetheight / 2.0f + measure.y / 2.0f + 20};

    DrawTextEx(g->font, b3, pp, 20, 1, fgc);
    libgame_handlefade(g);
}


void libgame_loadtexture(
    gamestate* g, int index, int contexts, int frames, bool dodither, const char* path) {
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
        mprint("could not open file");
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
    minfo("unloading texture");
    if (g->txinfo[index].texture.id > 0) {
        UnloadTexture(g->txinfo[index].texture);
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
        g->hero_id = heroid;
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
        sprite* s = sprite_create(&g->txinfo[keys[i]].texture,
                                  g->txinfo[keys[i]].contexts,
                                  g->txinfo[keys[i]].num_frames);
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
    hashtable_entityid_spritegroup_insert(spritegroups, g->hero_id, hero_group);
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


void libgame_close(gamestate* g) {
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

    UnloadFont(g->font);

    libgame_unloadtextures(g);

    minfo("unloading render texture");
    UnloadRenderTexture(target);

    minfo("closing window");
    CloseWindow();
}


gamestate* libgame_getgamestate() {
    return g;
}
