#include "fadestate.h"
#include "gamestate.h"
#include "mprint.h"
#include "sprite.h"
#include "utils.h"

#include "setdebugpanel.h"

#include <raylib.h>
#include <rlgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//--------------------------------------------------------------------
// libgame global variables
//--------------------------------------------------------------------
#define DEFAULT_TARGET_WIDTH 640
#define DEFAULT_TARGET_HEIGHT 360
#define DEFAULT_SCALE 2

#define DEFAULT_WINDOW_POS_X (1920 + 635)
#define DEFAULT_WINDOW_POS_Y 25

#define DEFAULT_WINDOW_WIDTH (DEFAULT_TARGET_WIDTH * DEFAULT_SCALE)
#define DEFAULT_WINDOW_HEIGHT (DEFAULT_TARGET_HEIGHT * DEFAULT_SCALE)

#define TXHERO 0
#define TXDIRT 1

#define COMPANYSCENE 0
#define TITLESCENE 1
#define GAMEPLAYSCENE 2

#define COMPANYNAME "@evildojo666"
#define COMPANYFILL "   x  x x   "

char debugpanelbuffer[1024] = {0};

//int activescene = COMPANYSCENE;
//int activescene = TITLESCENE;
int activescene = GAMEPLAYSCENE;

fadestate fade = FADESTATENONE;

int fadealpha = 0;

gamestate* g = NULL;

Font gfont;

RenderTexture target;

int targetwidth = DEFAULT_TARGET_WIDTH;
int targetheight = DEFAULT_TARGET_HEIGHT;

float scale = DEFAULT_SCALE;

int windowwidth = DEFAULT_WINDOW_WIDTH;
int windowheight = DEFAULT_WINDOW_HEIGHT;

Texture textures[10];

sprite* hero = NULL;


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
void drawdebugpanel();
void libgamehandleinput();
void drawcompanyscene();
void drawtitlescene();
void drawgameplayscene();
void handlefade();
void drawfade();
void libgameloadtexture(int index, const char* path, bool dodither);
void libgameloadtextures();
void libgameunloadtexture(int index);
void libgameunloadtextures();
void libgameinitsharedsetup();
void libgamecloseshared();

gamestate* libgame_getgamestate();

void libgame_drawframeend(gamestate* g);

//--------------------------------------------------------------------
// definitions
//--------------------------------------------------------------------
void drawfade() {
    if (fadealpha > 0) {
        Color c = {0, 0, 0, fadealpha};
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), c);
    }
}


void handlefade() {
    const int fadespeed = 4;
    if (fade == FADESTATEOUT && fadealpha < 255) {
        fadealpha += fadespeed;
    } else if (fade == FADESTATEIN && fadealpha > 0) {
        fadealpha -= fadespeed;
    }

    if (fadealpha >= 255) {
        fadealpha = 255;
        fade = FADESTATEIN;
        activescene++;
        if (activescene > 2) {
            activescene = 0;
        }
    }

    if (fadealpha <= 0) {
        fadealpha = 0;
        fade = FADESTATENONE;
    }

    drawfade();
}


void libgamehandleinput() {
    if (IsKeyPressed(KEY_SPACE)) {
        mprint("key space pressed");
        if (fade == FADESTATENONE) {
            fade = FADESTATEOUT;
        }
    }

    if (IsKeyPressed(KEY_D)) {
        g->debugpanelon = !g->debugpanelon;
    }

    if (IsKeyPressed(KEY_Z)) {
        g->cam2d.zoom += 0.1f;
    }
}


bool libgame_windowshouldclose() {
    return WindowShouldClose();
}


void gameinitwindow() {
    const char* title = "project rpg v0.000001";
    mprint("begin gameinitwindow");
    // have to do inittitlescene after initwindow
    // cant load textures before initwindow
    InitWindow(windowwidth, windowheight, title);
    while (!IsWindowReady())
        ;
    // this is hard-coded for now so we can auto-position the window
    // for easier config during streaming
    SetWindowMonitor(0);
    //SetWindowPosition(1920, 0);
    //const int pad = 900;
    //const int x = 1920 + pad;
    const int x = DEFAULT_WINDOW_POS_X;
    const int y = DEFAULT_WINDOW_POS_Y;
    SetWindowPosition(x, y);

    SetTargetFPS(60);
    SetExitKey(KEY_Q);
    mprint("end of gameinitwindow");
}


void gameclosewindow() {
    //rlglClose();
    CloseWindow();
}


void libgameupdatedebugpanelbuffer() {
    bzero(debugpanelbuffer, 1024);
    snprintf(debugpanelbuffer,
             1024,
             "Framecount:    %d\n"
             "%s\n"
             "%s\n"
             "Cam.target:  %.2f,%.2f\n"
             "Cam.offset:    %.2f,%.2f\n"
             "Active scene:  %d\n",
             g->framecount,

             g->timebeganbuf,
             g->currenttimebuf,

             g->cam2d.target.x,
             g->cam2d.target.y,
             g->cam2d.offset.x,
             g->cam2d.offset.y,

             activescene);
}


void libgameupdategamestate() {

    libgameupdatedebugpanelbuffer();
    //g->cam3d.target.x = 0;
    //g->cam3d.target.y = -1;
    //g->cam3d.target.z = 0;
    //g->cubepos.x += 0.001f;
    // trying to get the camera to properly follow...
    //g->cameramode = CAMERA_FREE;

    //g->cubepos = (Vector3){0.0f, -1.0f, 0.0f};

    //g->cam3d.position.x = 0.000f;
    //g->cam3d.position.y = 0.000f;
    //g->cam3d.position.z = 2.000f;

    //g->cam3d.target.x = 0.000f;
    //g->cam3d.target.y = 0.000f;
    //g->cam3d.target.z = -0.500f;

    //g->cam3d.position.x -= 0.001f;
    //g->cam3d.position.y -= 0.004f;

    //static float xdir = -0.01f;
    //static float ydir = -0.01f;
    //static float zdir = -0.01f;

    //g->cam3d.position.x += xdir;
    //g->cam3d.position.y += ydir;
    //g->cam3d.position.z += zdir;

    //if (g->cam3d.position.x <= -1.0f) {
    //    xdir = -xdir;
    //} else if (g->cam3d.position.x >= 1.0f) {
    //    xdir = -xdir;
    //}

    //if (g->cam3d.position.y <= 1.0f) {
    //    ydir = -ydir;
    //} else if (g->cam3d.position.y >= 8.0f) {
    //    ydir = -ydir;
    //}

    //if (g->cam3d.position.z <= -1.0f) {
    //    zdir = -zdir;
    //} else if (g->cam3d.position.z >= 1.0f) {
    //    zdir = -zdir;
    //}

    //g->cam3d.target = g->cubepos;

    //UpdateCamera(&g->cam3d, g->cameramode);


    //setdebugpaneltopleft(g);
    //setdebugpaneltopright(g);
    //setdebugpanelbottomleft(g);
    //setdebugpanelbottomright(g);
    //setdebugpanelcenter(g);
    //g->dp.w = 350;
    //g->dp.h = 200;
    // top left
    //setdebugpaneltopleft(g);
    // top right
    //setdebugpaneltopright(g);
    // bottom left
    //setdebugpanelbottomleft(g);
    // bottom right
    //setdebugpanelbottomright(g);
    //g->dp.fontsize = 20;
    //g->dp.fgcolor.r = 255;
    //g->dp.fgcolor.g = 255;
    //g->dp.fgcolor.b = 255;
    //g->dp.bgcolor.r = 0x33;
    //g->dp.bgcolor.g = 0x33;
    //g->dp.bgcolor.b = 0x33;
    //g->dp.bgcolor.a = 255;
    //g->clearcolor.r = 0;
    //g->clearcolor.g = 0;
    //g->clearcolor.b = 0;
    //g->cs->cubecolor = (Color){0x33, 0x33, 0x33, 255};
    //g->cs->cubecolor = (Color){0x66, 0x66, 0x66, 255};
    //g->cs->cubewirecolor = (Color){0x33, 0x33, 0x33, 255};
    //g->cs->cubecolor = (Color){255, 255, 255, 255};
    //g->cs->cubepos = (Vector3){-1.5f, 0.5f, 4.5f};
    //if (g->framecount % 120 == 0) {
    //    g->cs->cubepos =
    //        (Vector3){GetRandomValue(-4, 4) + 0.5f, 0.5f, GetRandomValue(-4, 4) + 0.5f};
    //    g->cs->cubecolor =
    //        (Color){GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255};
    //}
    //g->cs->cubepos = (Vector3){0.0f, 0.5f, 1.0f};
    //static float cubedirx = 0.01f;
    //static Vector3 cubev = {0.05f, 0.00f, 0.01f};
    //g->cs->cubepos.x = 0.05f;
    //g->cs->cubepos.y = 0.0f;
    //g->cs->cubepos.z = 0.01f;
    //g->cs->cubepos = (Vector3){0.0f, 0.5f, 1.0f};
    //g->cs->cubepos.x += cubev.x;
    //g->cs->cubepos.y += cubev.y;
    //g->cs->cubepos.z += cubev.z;
    //if (g->cs->cubepos.x > 4.5f) {
    //    cubev.x = -cubev.x;
    //} else if (g->cs->cubepos.x < -4.5f) {
    //    cubev.x = -cubev.x;
    //}
    //if (g->cs->cubepos.y > 5.0f) {
    //    cubev.y = -cubev.y;
    //} else if (g->cs->cubepos.y < -5.0f) {
    //    cubev.y = -cubev.y;
    //}
    //if (g->cs->cubepos.z > 4.5f) {
    //    cubev.z = -cubev.z;
    //} else if (g->cs->cubepos.z < -4.5f) {
    //    cubev.z = -cubev.z;
    //}
    //static float dpx = 0.00f;
    //static float dtx = 0.00f;
    //static float dpy = 0.01f;
    //static float dty = 0.01f;
    //static float dpz = 0.00f;
    //static float dtz = 0.00f;
    //const float ymax = 10.0f;
    //const float ymin = 0.0f;
    //if (g->cs->cam3d.position.y >= 0.0f || g->cs->cam3d.position.y < 2.0f) {
    //    if (dty < 0) {
    //        dty = -0.005f;
    //    } else if (dty > 0) {
    //        dty = 0.005f;
    //    }
    //}
    // g->cs->cam3d.position = (Vector3){10.0f, 10.0f, -10.0f};
    //g->cs->cam3d.position = (Vector3){-10.0f, 5.0f, 9.0f};
    //g->cs->cam3d.position = (Vector3){-3.0f, 5.0f, 5.0f};
    //g->cs->cam3d.target = (Vector3){0.0f, 0.0f, 0.0f};
    //g->cam3d.target = g->cubepos;
    //g->cs->cam3d.target = (Vector3){0.0f, 0.0f, 0.0f};
    //g->cs->cam3d.position.x = 0.0f;
    //g->cs->cam3d.target.x = 0.0f;
    //g->cs->cam3d.position.y = 10.0f;
    //g->cs->cam3d.target.y = 0.0f;
    //g->cs->cam3d.position.z = 5.0f;
    //g->cs->cam3d.target.z = 0.0f;
    //g->cs->cam3d.position.y += dpy;
    //g->cs->cam3d.target.y += dty;
    //g->cs->cam3d.position.z += dz;
    //g->cs->cam3d.target.z += dz;
    //if (g->cs->cam3d.position.y > ymax || g->cs->cam3d.position.y < ymin) {
    //    dpy = -dpy;
    //    dty = -dty;
    //}
    //if (g->cs->cam3d.target.y > ymax || g->cs->cam3d.target.y < ymin) {
    //    dty = -dty;
    //}
    //g->cs->scale = 4;
    //const int scale = g->cs->scale;
    //g->cs->x = g->winwidth / 2 - g->cs->presents.width * scale / 2;
    //g->cs->y = g->winheight / 2 - g->cs->presents.height * scale / 2;
    //g->cs->dodrawpresents = false;
    //g->cs->dodrawpresents = true;
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

    float w = target.texture.width;
    float h = target.texture.height;

    Rectangle source = {0, 0, w, -h};
    Rectangle dest = {0, 0, GetScreenWidth(), GetScreenHeight()};
    Vector2 origin = {0, 0};

    DrawTexturePro(target.texture, source, dest, origin, 0.0f, WHITE);

    drawdebugpanel();

    libgame_drawframeend(g);

    //EndDrawing();
    //g->framecount++;
    //gamestateupdatecurrenttime(g);

    //g->dp.x = 10;
    //g->dp.y = 10;
}


inline void drawdebugpanel() {
    if (g->debugpanelon) {
        const int fontsize = 18;
        const int spacing = 1;

        Vector2 p = {g->dp.x, g->dp.y};

        // this content update needs to be separated from the draw
        //bzero(debugpanelbuffer, 1024);
        //snprintf(debugpanelbuffer,
        //         1024,
        //         "Framecount:   %d\n%s\n%s\nfade:         %d\nfadealpha:    %d\nscene:        %d\n",
        //         g->framecount,
        //         g->timebeganbuf,
        //         g->currenttimebuf,
        //         fade,
        //         fadealpha,
        //         activescene);


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
        DrawRectangleLines(bx, by, bw, bh, WHITE);

        int txoffsetxy = 3;
        int txoffsetwh = 8;
        int tx = p.x - txoffsetxy;
        int ty = p.y - txoffsetxy;
        int tw = m.x + txoffsetwh;
        int th = m.y + txoffsetwh;

        DrawRectangle(tx, ty, tw, th, (Color){0, 0, 0, 128});
        DrawTextEx(gfont, debugpanelbuffer, p, fontsize, spacing, WHITE);
    }
}


void drawgameplayscene() {
    BeginMode2D(g->cam2d);
    ClearBackground(BLACK);

    // lets draw the sprite
    DrawTextureRec(textures[TXHERO], hero->src, (Vector2){0, 0}, WHITE);

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


void libgameloadtexture(int index, const char* path, bool dodither) {
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


void libgameloadtextures() {
    //textures[TXHERO] = LoadTexture("img/darkknight2-sheet.png");
    //SetTextureFilter(textures[TXHERO], TEXTURE_FILTER_POINT);

    libgameloadtexture(TXHERO, "img/darkknight2-sheet.png", false);

    //libgameloadtexture(TXDIRT, "img/tile-dirt0.png", true);

    // do dithering on the dirt texture
    Image img = LoadImage("img/tile-dirt0.png");
    ImageDither(&img, 32, 32, 32, 32);
    //ImageDither(&img, 16, 16, 16, 16);
    textures[TXDIRT] = LoadTextureFromImage(img);
    UnloadImage(img);
}


void libgameunloadtexture(int index) {
    if (textures[index].id > 0) {
        UnloadTexture(textures[index]);
    }
}


void libgameunloadtextures() {
    libgameunloadtexture(TXHERO);
    libgameunloadtexture(TXDIRT);
}


void libgameinit() {
    mprint("libgameinit");

    g = gamestateinitptr();
    libgameinitsharedsetup();
}


void libgameinitsharedsetup() {
    gameinitwindow();

    //gfont = LoadFont("fonts/hack.ttf");
    gfont = LoadFontEx("fonts/hack.ttf", 60, 0, 250);
    //SetTextureFilter(gfont.texture, TEXTURE_FILTER_BILINEAR);

    target = LoadRenderTexture(targetwidth, targetheight);
    //SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    libgameloadtextures();

    // lets try initializing a sprite
    int numcontexts = 3;
    int numframes = 4;
    hero = sprite_create(&textures[TXHERO], numcontexts, numframes);

    // we need to set the destination
    // this is a function of how much we have scaled the target texture
    // we need to write code to manage this but we will hack something
    // together for right now
    const int scale = 2;
    float x = targetwidth / 2.0f;
    float y = targetheight / 2.0f;
    float w = hero->width * scale;
    float h = hero->height * scale;

    hero->dest = (Rectangle){x, y, w, h};

    setdebugpaneltopleft(g);


    g->cam2d.offset = (Vector2){targetwidth / 2.0f, targetheight / 2.0f};
}


void libgameinitwithstate(void* state) {
    if (state == NULL) {
        mprint("libgameinitwithstate: state is NULL");
        return;
    }
    mprint("libgameinitwithstate");
    g = (gamestate*)state;
    libgameinitsharedsetup();
}


void libgameclosesavegamestate() {
    mprint("libgameclosesavegamestate");
    libgamecloseshared();
}


void libgameclose() {
    mprint("libgameclose");
    gamestatefree(g);
    libgamecloseshared();
}


void libgamecloseshared() {
    sprite_destroy(hero);

    UnloadFont(gfont);
    mprint("unloading textures");
    libgameunloadtextures();
    mprint("unloading render texture");
    UnloadRenderTexture(target);
    mprint("closing window");
    CloseWindow();
}


gamestate* libgame_getgamestate() {
    return g;
}


void drawcubetexturerecfrontface(
    const Texture2D tex, const Rectangle src, Vector3 pos, float w, float h, float l) {
    const float rx = src.x + src.width;
    const float ry = src.y + src.height;
    const float a = pos.x - w / 2;
    const float b = pos.x + w / 2;
    const float c = pos.y - h / 2;
    const float d = pos.y + h / 2;
    const float e = pos.z + l / 2;
    const float f = pos.z - l / 2;
    // precompute
    // vertices
    const Vector3 v[4] = {{a, c, e}, {b, c, e}, {b, d, e}, {a, d, e}};
    // texture coordinates
    const Vector2 t[4] = {{src.x / tex.width, ry / tex.height},
                          {rx / tex.width, ry / tex.height},
                          {rx / tex.width, src.y / tex.height},
                          {src.x / tex.width, src.y / tex.height}};
    // Set desired texture to be enabled while drawintex.height tex.widthollowintex.height vertex data
    rlSetTexture(tex.id);
    // We calculate the normalized texture coordinates for the desired texture-source-rectangle
    // It means converting from (tex.width, tex.height) coordinates to [0.0f, 1.0f] equivalent
    rlBegin(RL_QUADS);
    rlColor4ub(255, 255, 255, 255);
    // draw front face
    rlNormal3f(0.0f, 0.0f, 1.0f);
    for (int i = 0; i < 4; i++) {
        rlTexCoord2f(t[i].x, t[i].y);
        rlVertex3f(v[i].x, v[i].y, v[i].z);
    }
    rlEnd();
    rlSetTexture(0);
}


void drawcubetexturerecallfaces(
    const Texture2D tex, const Rectangle src, Vector3 pos, float w, float h, float l) {
    const float rx = src.x + src.width;
    const float ry = src.y + src.height;
    const float a = pos.x - w / 2;
    const float b = pos.x + w / 2;
    const float c = pos.y - h / 2;
    const float d = pos.y + h / 2;
    const float e = pos.z + l / 2;
    const float f = pos.z - l / 2;
    // precompute
    // vertices
    const Vector3 v[4] = {{a, c, e}, {b, c, e}, {b, d, e}, {a, d, e}};
    // texture coordinates
    const Vector2 t[4] = {{src.x / tex.width, ry / tex.height},
                          {rx / tex.width, ry / tex.height},
                          {rx / tex.width, src.y / tex.height},
                          {src.x / tex.width, src.y / tex.height}};
    // Set desired texture to be enabled while drawintex.height tex.widthollowintex.height vertex data
    rlSetTexture(tex.id);
    // We calculate the normalized texture coordinates for the desired texture-source-rectangle
    // It means converting from (tex.width, tex.height) coordinates to [0.0f, 1.0f] equivalent
    rlBegin(RL_QUADS);
    rlColor4ub(255, 255, 255, 255);
    // draw front face
    rlNormal3f(0.0f, 0.0f, 1.0f);
    for (int i = 0; i < 4; i++) {
        rlTexCoord2f(t[i].x, t[i].y);
        rlVertex3f(v[i].x, v[i].y, v[i].z);
    }
    /////////////////////////////////////////////////////////
    // Back face
    rlNormal3f(0.0f, 0.0f, -1.0f);
    rlTexCoord2f((src.x + src.width) / tex.width, (src.y + src.height) / tex.height);
    rlVertex3f(pos.x - w / 2, pos.y - h / 2, pos.z - l / 2);
    rlTexCoord2f((src.x + src.width) / tex.width, src.y / tex.height);
    rlVertex3f(pos.x - w / 2, pos.y + h / 2, pos.z - l / 2);
    rlTexCoord2f(src.x / tex.width, src.y / tex.height);
    rlVertex3f(pos.x + w / 2, pos.y + h / 2, pos.z - l / 2);
    rlTexCoord2f(src.x / tex.width, (src.y + src.height) / tex.height);
    rlVertex3f(pos.x + w / 2, pos.y - h / 2, pos.z - l / 2);
    // Top face
    rlNormal3f(0.0f, 1.0f, 0.0f);
    rlTexCoord2f(src.x / tex.width, src.y / tex.height);
    rlVertex3f(pos.x - w / 2, pos.y + h / 2, pos.z - l / 2);
    rlTexCoord2f(src.x / tex.width, (src.y + src.height) / tex.height);
    rlVertex3f(pos.x - w / 2, pos.y + h / 2, pos.z + l / 2);
    rlTexCoord2f((src.x + src.width) / tex.width, (src.y + src.height) / tex.height);
    rlVertex3f(pos.x + w / 2, pos.y + h / 2, pos.z + l / 2);
    rlTexCoord2f((src.x + src.width) / tex.width, src.y / tex.height);
    rlVertex3f(pos.x + w / 2, pos.y + h / 2, pos.z - l / 2);
    // Bottom face
    rlNormal3f(0.0f, -1.0f, 0.0f);
    rlTexCoord2f((src.x + src.width) / tex.width, src.y / tex.height);
    rlVertex3f(pos.x - w / 2, pos.y - h / 2, pos.z - l / 2);
    rlTexCoord2f(src.x / tex.width, src.y / tex.height);
    rlVertex3f(pos.x + w / 2, pos.y - h / 2, pos.z - l / 2);
    rlTexCoord2f(src.x / tex.width, (src.y + src.height) / tex.height);
    rlVertex3f(pos.x + w / 2, pos.y - h / 2, pos.z + l / 2);
    rlTexCoord2f((src.x + src.width) / tex.width, (src.y + src.height) / tex.height);
    rlVertex3f(pos.x - w / 2, pos.y - h / 2, pos.z + l / 2);
    // Right face
    rlNormal3f(1.0f, 0.0f, 0.0f);
    rlTexCoord2f((src.x + src.width) / tex.width, (src.y + src.height) / tex.height);
    rlVertex3f(pos.x + w / 2, pos.y - h / 2, pos.z - l / 2);
    rlTexCoord2f((src.x + src.width) / tex.width, src.y / tex.height);
    rlVertex3f(pos.x + w / 2, pos.y + h / 2, pos.z - l / 2);
    rlTexCoord2f(src.x / tex.width, src.y / tex.height);
    rlVertex3f(pos.x + w / 2, pos.y + h / 2, pos.z + l / 2);
    rlTexCoord2f(src.x / tex.width, (src.y + src.height) / tex.height);
    rlVertex3f(pos.x + w / 2, pos.y - h / 2, pos.z + l / 2);
    // Left face
    rlNormal3f(-1.0f, 0.0f, 0.0f);
    rlTexCoord2f(src.x / tex.width, (src.y + src.height) / tex.height);
    rlVertex3f(pos.x - w / 2, pos.y - h / 2, pos.z - l / 2);
    rlTexCoord2f((src.x + src.width) / tex.width, (src.y + src.height) / tex.height);
    rlVertex3f(pos.x - w / 2, pos.y - h / 2, pos.z + l / 2);
    rlTexCoord2f((src.x + src.width) / tex.width, src.y / tex.height);
    rlVertex3f(pos.x - w / 2, pos.y + h / 2, pos.z + l / 2);
    rlTexCoord2f(src.x / tex.width, src.y / tex.height);
    rlVertex3f(pos.x - w / 2, pos.y + h / 2, pos.z - l / 2);
    rlEnd();
    rlSetTexture(0);
}
