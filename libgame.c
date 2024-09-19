#include "fadestate.h"
#include "gamestate.h"
#include "mprint.h"
#include "sprite.h"
#include "utils.h"

#include "dungeonfloor.h"
#include "libgame_defines.h"
#include "setdebugpanel.h"

#include "controlmode.h"

#include <raylib.h>
#include <rlgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

float scale = DEFAULT_SCALE;

Texture textures[10];

sprite* hero = NULL;

dungeonfloor_t* dungeonfloor = NULL;

//--------------------------------------------------------------------
// function declarations
//--------------------------------------------------------------------
bool libgame_windowshouldclose();

void gameinitwindow();
void gameclosewindow();

void libgameupdatedebugpanelbuffer();
void libgameupdategamestate();
void libgamedrawframe();
void libgameinit();
void libgameinitwithstate(void* state);
void libgameclose();
void libgamehandleinput();

void libgame_loadtexture(int index, const char* path, bool dodither);
void libgame_loadtextures();

void libgame_unloadtexture(int index);
void libgame_unloadtextures();

void libgame_initsharedsetup();

void libgame_closeshared();

void drawdebugpanel();
void drawcompanyscene();
void drawtitlescene();
void drawgameplayscene();
void handlefade();
void drawfade();

gamestate* libgame_getgamestate();
void libgame_drawframeend(gamestate* g);


void libgame_handleplayerinput(gamestate* g);
void libgame_handlecaminput(gamestate* g);
void libgame_handledebugpanelswitch(gamestate* g);
void libgame_handlemodeswitch(gamestate* g);


//--------------------------------------------------------------------
// definitions
//--------------------------------------------------------------------
void drawfade() {
    if (g->fadealpha > 0) {
        Color c = {0, 0, 0, g->fadealpha};
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), c);
    }
}


void handlefade() {
    const int fadespeed = 4;
    if (g->fadestate == FADESTATEOUT && g->fadealpha < 255) {
        g->fadealpha += fadespeed;
    } else if (g->fadestate == FADESTATEIN && g->fadealpha > 0) {
        g->fadealpha -= fadespeed;
    }

    if (g->fadealpha >= 255) {
        g->fadealpha = 255;
        g->fadestate = FADESTATEIN;
        activescene++;
        if (activescene > 2) {
            activescene = 0;
        }
    }

    if (g->fadealpha <= 0) {
        g->fadealpha = 0;
        g->fadestate = FADESTATENONE;
    }

    drawfade();
}


void libgamehandleinput() {
    if (IsKeyPressed(KEY_SPACE)) {
        mprint("key space pressed");
        if (g->fadestate == FADESTATENONE) {
            g->fadestate = FADESTATEOUT;
        }
    }

    libgame_handlemodeswitch(g);
    libgame_handledebugpanelswitch(g);
    libgame_handleplayerinput(g);
    libgame_handlecaminput(g);
}


void libgame_handlemodeswitch(gamestate* g) {
    if (IsKeyPressed(KEY_C)) {
        if (g->controlmode == CONTROLMODE_CAMERA) {
            g->controlmode = CONTROLMODE_PLAYER;
        } else {
            g->controlmode = CONTROLMODE_CAMERA;
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
            hero->dest.x += 20;
        } else if (IsKeyPressed(KEY_LEFT)) {
            hero->dest.x -= 20;
        }

        if (IsKeyPressed(KEY_DOWN)) {
            hero->dest.y += 20;
        } else if (IsKeyPressed(KEY_UP)) {
            hero->dest.y -= 20;
        }
    }
}


void libgame_handlecaminput(gamestate* g) {

    if (g->controlmode == CONTROLMODE_CAMERA) {

        const bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        const float zoom_incr = 0.01f;
        const float cam_move_incr = 1;

        if (IsKeyDown(KEY_Z) && shift) {
            g->cam2d.zoom -= zoom_incr;
            //} else if (IsKeyPressed(KEY_Z)) {
        } else if (IsKeyDown(KEY_Z)) {
            g->cam2d.zoom += zoom_incr;
        }

        if (IsKeyDown(KEY_UP)) {
            g->cam2d.target.y -= cam_move_incr;
        } else if (IsKeyDown(KEY_DOWN)) {
            g->cam2d.target.y += cam_move_incr;
        }

        if (IsKeyDown(KEY_LEFT)) {
            g->cam2d.target.x -= cam_move_incr;
        } else if (IsKeyDown(KEY_RIGHT)) {
            g->cam2d.target.x += cam_move_incr;
        }
    }
}


bool libgame_windowshouldclose() {
    return WindowShouldClose();
}


void gameinitwindow() {
    const char* title = DEFAULT_WINDOW_TITLE;
    mprint("begin gameinitwindow");
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
    mprint("end of gameinitwindow");
}


void gameclosewindow() {
    //rlglClose();
    CloseWindow();
}


void libgameupdatedebugpanelbuffer() {
    //bzero(debugpanelbuffer, 1024);
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

             g->controlmode);
}


void libgameupdategamestate() {
    libgameupdatedebugpanelbuffer();

    //setdebugpaneltopleft(g);
    //setdebugpaneltopright(g);
    //setdebugpanelbottomleft(g);
    //setdebugpanelbottomright(g);
    //setdebugpanelcenter(g);
}


void libgame_drawframeend(gamestate* g) {
    EndDrawing();
    g->framecount++;
    gamestateupdatecurrenttime(g);
}

void libgamedrawframe() {
    BeginDrawing();
    BeginTextureMode(target);

    if (activescene == 0) {
        drawcompanyscene();
    } else if (activescene == 1) {
        drawtitlescene();
    } else if (activescene == 2) {
        drawgameplayscene();
    }

    EndTextureMode();

    DrawTexturePro(target.texture, target_src, target_dest, target_origin, 0.0f, WHITE);
    drawdebugpanel();


    libgame_drawframeend(g);
}


inline void drawdebugpanel() {
    if (g->debugpanelon) {
        const int fontsize = 14;
        const int spacing = 1;

        Vector2 p = {g->dp.x, g->dp.y};
        Vector2 m = MeasureTextEx(gfont, debugpanelbuffer, fontsize, spacing);
        // update the debug panel width and height
        // this code will go elsewhere like an 'update debug panel' method
        // to separate the debug panel update from the draw
        // we store the root measurement because
        // the box is drawn relative to
        // the size of the text
        g->dp.w = m.x;
        g->dp.h = m.y;

        int boxoffsetxy = 10;
        int boxoffsetwh = 20;

        int bx = p.x - boxoffsetxy;
        int by = p.y - boxoffsetxy;
        int bw = m.x + boxoffsetwh;
        int bh = m.y + boxoffsetwh;
        DrawRectangle(bx, by, bw, bh, (Color){0x33, 0x33, 0x33, 128});

        int txoffsetxy = 3;
        int txoffsetwh = 8;
        int tx = p.x - txoffsetxy;
        int ty = p.y - txoffsetxy;
        int tw = m.x + txoffsetwh;
        int th = m.y + txoffsetwh;

        //DrawRectangle(tx, ty, tw, th, (Color){0x33, 0x33, 0x33, 128});
        DrawTextEx(gfont, debugpanelbuffer, p, fontsize, spacing, WHITE);
    }
}


void drawgameplayscene() {
    BeginMode2D(g->cam2d);
    ClearBackground(BLACK);

    // lets draw the sprite
    //DrawTextureEx(textures[TXHERO], (Vector2){hero->dest.x, hero->dest.y}, 0.0f, 1.0f, WHITE);

    const int w = textures[TXDIRT].width;
    const int h = textures[TXDIRT].height;

    Rectangle tile_src = {0, 0, w, h};
    Rectangle tile_dest = {0, 0, w, h};
    Vector2 origin = {0, 0};
    Color c = WHITE;
    Color border0 = RED;
    Color border1 = GREEN;
    float rotation = 0;

    //DrawTexturePro(textures[TXDIRT], tile_src, tile_dest, origin, rotation, c);

    // draw the dungeon floor

    for (int i = 0; i < dungeonfloor->len; i++) {
        for (int j = 0; j < dungeonfloor->wid; j++) {
            tile_dest.x = i * w;
            tile_dest.y = j * h;
            DrawTexturePro(textures[TXDIRT], tile_src, tile_dest, origin, rotation, c);
            // lets also draw a border around the tiles

            if (g->debugpanelon) {
                DrawRectangleLinesEx(tile_dest, 1, border0);
            }
        }
    }

    DrawTexturePro(textures[TXHERO], hero->src, hero->dest, origin, rotation, c);
    if (g->debugpanelon) {
        DrawRectangleLinesEx(hero->dest, 1, border1);
    }

    EndMode2D();

    handlefade();

    // we will want to find a way to wrap animation management
    // this will get cumbersome as the # of sprites on-screen grows.
    // will also need a way to manage the existing sprites on screen
    // previously i used an unordered map in C++ to do this
    // i will need to find a way to do this in C
    //
    // we could rawdog an array of sprite pointers or something but we will deal with it
    // when we get there
    if (g->framecount % 10 == 0) {
        sprite_incrframe(hero);
    }
}


void drawtitlescene() {
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


    handlefade();
}


void drawcompanyscene() {
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

    handlefade();
}


void libgame_loadtexture(int index, const char* path, bool dodither) {
    if (dodither) {
        Image img = LoadImage(path);
        ImageDither(&img, 16, 16, 16, 16);
        textures[index] = LoadTextureFromImage(img);
        UnloadImage(img);
    } else {
        textures[index] = LoadTexture(path);
    }
    SetTextureFilter(textures[index], TEXTURE_FILTER_POINT);
}


void libgame_loadtextures() {
    //textures[TXHERO] = LoadTexture("img/darkknight2-sheet.png");
    //SetTextureFilter(textures[TXHERO], TEXTURE_FILTER_POINT);

    libgame_loadtexture(TXHERO, "img/darkknight2-sheet.png", false);


    // do dithering on the dirt texture
    Image img = LoadImage("img/tile-dirt0.png");
    //ImageDither(&img, 32, 32, 32, 32);
    ImageDither(&img, 4, 4, 4, 4);
    textures[TXDIRT] = LoadTextureFromImage(img);
    UnloadImage(img);
}


void libgame_unloadtexture(int index) {
    if (textures[index].id > 0) {
        UnloadTexture(textures[index]);
    }
}


void libgame_unloadtextures() {
    libgame_unloadtexture(TXHERO);
    libgame_unloadtexture(TXDIRT);
}


void libgameinit() {
    mprint("libgameinit");

    g = gamestateinitptr();
    libgame_initsharedsetup();
}


void libgame_initsharedsetup() {
    gameinitwindow();

    //gfont = LoadFont("fonts/hack.ttf");
    gfont = LoadFontEx("fonts/hack.ttf", 60, 0, 250);
    //SetTextureFilter(gfont.texture, TEXTURE_FILTER_BILINEAR);
    target = LoadRenderTexture(targetwidth, targetheight);
    target_src = (Rectangle){0, 0, target.texture.width, -target.texture.height};
    target_dest = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};
    //SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
    libgame_loadtextures();
    // lets try initializing a sprite
    int numcontexts = 3;
    int numframes = 4;
    hero = sprite_create(&textures[TXHERO], numcontexts, numframes);
    // we need to set the destination
    // this is a function of how much we have scaled the target texture
    // we need to write code to manage this but we will hack something
    // together for right now
    float x = 0;
    float y = 0;
    float w = hero->width;
    float h = hero->height;

    // notice that the hero is a bit larger than the tile
    // we can solve this a number of ways
    //
    // 1. we could force all things to be the size of the tile, which would involve drawing a new hero
    //
    // 2. we could adjust the hero's destination by modifying the y position

    const float offset_y = -8;

    hero->dest = (Rectangle){x, y + offset_y, w, h};

    setdebugpaneltopleft(g);

    g->cam2d.offset = (Vector2){targetwidth / 2.0f, targetheight / 2.0f};

    // init dungeonfloor
    dungeonfloor = create_dungeonfloor(4, 4, TILETYPE_DIRT);
    if (!dungeonfloor) {
        mprint("could not create dungeonfloor");
        // we could use an 'emergency shutdown' in case an error causes us
        // to need to 'panic' and force game close properly
    }
}


void libgameinitwithstate(void* state) {
    if (state == NULL) {
        mprint("libgameinitwithstate: state is NULL");
        return;
    }
    mprint("libgameinitwithstate");
    g = (gamestate*)state;
    libgame_initsharedsetup();
}


void libgameclosesavegamestate() {
    mprint("libgameclosesavegamestate");
    libgame_closeshared();
}


void libgameclose() {
    mprint("libgameclose");
    gamestatefree(g);
    libgame_closeshared();
}


void libgame_closeshared() {
    sprite_destroy(hero);

    // not right now, but when we add dungeonfloor to
    // the gamestate, we will be able to avoid freeing
    // it on every reload
    dungeonfloor_free(dungeonfloor);

    UnloadFont(gfont);
    mprint("unloading textures");
    libgame_unloadtextures();
    mprint("unloading render texture");
    UnloadRenderTexture(target);
    mprint("closing window");
    CloseWindow();
}


gamestate* libgame_getgamestate() {
    return g;
}
