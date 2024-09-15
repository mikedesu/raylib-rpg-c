#include "fadestate.h"
#include "gamestate.h"
#include "mprint.h"
#include "utils.h"

#include <raylib.h>
#include <rlgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


//--------------------------------------------------------------------
// libgame global variables
//--------------------------------------------------------------------


#define TXHERO 0


char debugpanelbuffer[1024] = {0};

int activescene = 1;

fadestate fade = FADESTATENONE;

int fadealpha = 0;

gamestate* g = NULL;

Font gfont;

bool debugpanelon = true;

RenderTexture target;

//int windowwidth = 640;
//int windowheight = 360;
//int windowwidth = 1280;
//int windowheight = 720;
int windowwidth = 1920;
int windowheight = 1080;

int targetwidth = 640;
int targetheight = 360;


Texture textures[10];


//--------------------------------------------------------------------
// function declarations
//--------------------------------------------------------------------
bool gamewindowshouldclose();
void gameinitwindow();
void gameclosewindow();
void updategamestate();
void drawframe();
void libgameinit();
void libgameinitwithstate(void* state);
void libgameclose();

void drawdebugpanel();

void libgamehandleinput();

void drawcompanysceneframe();
void drawtitlesceneframe();
void drawgameplaysceneframe();

void setdebugpaneltopleft(gamestate* g);
void setdebugpanelbottomleft(gamestate* g);
void setdebugpanelbottomright(gamestate* g);
void setdebugpaneltopright(gamestate* g);
void gameinitframecount(unsigned int fc);
unsigned int saveframecount();
gamestate* libgamegetgamestate();
void handlefade();
void drawfade();
void libgameloadtextures();
void libgameunloadtextures();

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

    if (fade == FADESTATEOUT) {
        if (fadealpha < 255) {
            fadealpha += fadespeed;
        }
        if (fadealpha >= 255) {
            fadealpha = 255;
            fade = FADESTATEIN;
            activescene++;
            if (activescene > 2) {
                activescene = 0;
            }
        }
    } else if (fade == FADESTATEIN) {
        if (fadealpha > 0) {
            fadealpha -= fadespeed;
        }
        if (fadealpha <= 0) {
            fadealpha = 0;
            fade = FADESTATENONE;
        }
    }
    drawfade();
}


void libgamehandleinput() {
    if (IsKeyPressed(KEY_SPACE)) {
        mprint("key space pressed");
        if (fade == FADESTATENONE) {
            fade = FADESTATEOUT;
        }
        //activescene++;
        //if (activescene > 1) {
        //    activescene = 0;
        //}
    }

    if (IsKeyPressed(KEY_D)) {
        debugpanelon = !debugpanelon;
    }


    if (IsKeyPressed(KEY_F)) {
        ToggleFullscreen();
    }
}


bool gamewindowshouldclose() {
    return WindowShouldClose();
}


void gameinitwindow() {
    const char* title = "project rpg v0.000001";
    //const int w = 1920;
    //const int h = 1080;
    //const int w = 1920 / 2;
    //const int h = 1080 / 2;
    const int w = windowwidth;
    const int h = windowheight;
    mprint("begin gameinitwindow");
    // have to do inittitlescene after initwindow
    // cant load textures before initwindow
    InitWindow(w, h, title);
    while (!IsWindowReady())
        ;
    // this is hard-coded for now so we can auto-position the window
    // for easier config during streaming
    SetWindowMonitor(0);
    SetWindowPosition(1920, 0);
    //SetWindowPosition(0, 0);
    SetTargetFPS(60);
    SetExitKey(KEY_Q);
    mprint("end of gameinitwindow");
}


//void gameinitframecount(unsigned int fc) {
//    framecount = fc;
//}


void gameclosewindow() {
    //rlglClose();
    CloseWindow();
}


//void setdebugpaneltopleft(gamestate* g) {
//g->dp.x = 5;
//g->dp.y = 5;
//}


//void setdebugpanelbottomleft(gamestate* g) {
//g->dp.x = 5;
//g->dp.y = g->winheight - g->dp.h - 20;
//}


//void setdebugpanelbottomright(gamestate* g) {
//g->dp.x = g->winwidth - g->dp.w - 20;
//g->dp.y = g->winheight - g->dp.h - 20;
//}


//void setdebugpaneltopright(gamestate* g) {
//g->dp.x = g->winwidth - g->dp.w - 20;
//g->dp.y = 5;
//}


void updategamestate() {
    //    mprint("updategamestateunsafe");
    //now = time(NULL);
    //if (start == 0) {
    //    //start = now;
    //    start = g->starttime;
    //    tm2 = localtime(&start);
    //    memset(timebuf2, 0, 64);
    //    strftime(timebuf2, 64, "%Y-%m-%d %H:%M:%S", tm2);
    //}
    //tm = localtime(&now);
    //memset(timebuf, 0, 64);
    //memset(g->dp.bfr, 0, DEBUGPANELBUFSIZE);
    //strftime(timebuf, 64, "%Y-%m-%d %H:%M:%S", tm);
    //snprintf(g->dp.bfr,
    //         DEBUGPANELBUFSIZE,
    //         "framecount:   %d\n"
    //         "start date:   %s\n"
    //         "current date: %s\n"
    //         "camera3d.pos: %.2f %.2f %.2f\n"
    //         "camera3d.target:   %.2f %.2f %.2f\n"
    //         "camera3d.proj: %d\ncameramode: %d\n",
    //         g->framecount,
    //         timebuf2,
    //         timebuf,
    //         g->cs->cam3d.position.x,
    //         g->cs->cam3d.position.y,
    //         g->cs->cam3d.position.z,
    //         g->cs->cam3d.target.x,
    //         g->cs->cam3d.target.y,
    //         g->cs->cam3d.target.z,
    //         g->cs->cam3d.projection,
    //         g->cs->cameramode);
    //g->dp.w = 350;
    //g->dp.h = 200;
    // top left
    //setdebugpaneltopleft(g);
    // top right
    setdebugpaneltopright(g);
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
    //g->cs->cam3d.target = g->cs->cubepos;
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


void drawframe() {

    BeginDrawing();
    BeginTextureMode(target);

    if (activescene == 0) {
        drawcompanysceneframe();
    } else if (activescene == 1) {
        drawtitlesceneframe();
    } else if (activescene == 2) {
        drawgameplaysceneframe();
    }


    EndTextureMode();

    float w = target.texture.width;
    float h = target.texture.height;

    Rectangle source = {0, 0, w, -h};
    Rectangle dest = {0, 0, GetScreenWidth(), GetScreenHeight()};
    Vector2 origin = {0, 0};

    //DrawTextureRec(target.texture, source, dest, WHITE);
    DrawTexturePro(target.texture, source, dest, origin, 0.0f, WHITE);

    drawdebugpanel();

    EndDrawing();

    g->framecount++;
    gamestateupdatecurrenttime(g);

    g->dp.x = 10;
    g->dp.y = 10;
}


inline void drawdebugpanel() {
    if (debugpanelon) {
        const int fontsize = 14, spacing = 1;

        //Vector2 p = (Vector2){10, 10};
        Vector2 p = {g->dp.x, g->dp.y};

        bzero(debugpanelbuffer, 1024);
        snprintf(debugpanelbuffer,
                 1024,
                 "Framecount:   %d\n%s\n%s\nfade:    %d\nfadealpha:    %d\nscene:  %d\n",

                 g->framecount,
                 g->timebeganbuf,
                 g->currenttimebuf,
                 fade,
                 fadealpha,
                 activescene);
        Vector2 m = MeasureTextEx(gfont, debugpanelbuffer, fontsize, spacing);

        DrawRectangleLines(p.x - 5, p.y - 5, m.x + 12, m.y + 12, WHITE);
        DrawRectangle(p.x - 3, p.y - 3, m.x + 8, m.y + 8, (Color){0, 0, 0, 128});

        DrawTextEx(gfont, debugpanelbuffer, p, fontsize, spacing, WHITE);
    }
}


void drawgameplaysceneframe() {
    ClearBackground(BLACK);


    // lets text drawing the hero sprite
    DrawTexture(textures[TXHERO], targetwidth / 2, targetheight / 2, WHITE);


    DrawText("gameplay scene", targetwidth / 2, targetheight / 2, 20, WHITE);


    handlefade();
}


void drawtitlesceneframe() {
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


#define COMPANYNAME "@evildojo666"
#define COMPANYFILL "   x  x x   "
void drawcompanysceneframe() {
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
    const Vector2 m = MeasureTextEx(gfont, b, fontsize, spacing);
    const Vector2 m2 = MeasureTextEx(gfont, b2, fontsize, spacing);

    if (g->framecount % interval >= 0 && g->framecount % interval < dur) {
        for (int i = 0; i < 10; i++) {
            shufflestrinplace(b);
            shufflestrinplace(b3);
        }
    }
    for (int i = 0; i < 10; i++) {
        shufflestrinplace(b2);
    }

    //Vector2 p = {GetScreenWidth() / 2.0f - m.x / 2.0f, GetScreenHeight() / 2.0f - m.y / 2.0f};
    Vector2 p = {targetwidth / 2.0f - m.x / 2.0f, targetheight / 2.0f - m.y / 2.0f};
    ClearBackground(bgc);
    DrawTextEx(gfont, b, p, fontsize, 1, fgc);
    DrawTextEx(gfont, b2, p, fontsize, 1, fgc);

    const Vector2 mp = MeasureTextEx(gfont, b3, 20, 1);
    //const Vector2 pp = {GetScreenWidth() / 2.0f - mp.x / 2.0f,
    //                    GetScreenHeight() / 2.0f + m.y / 2.0f + 20};
    const Vector2 pp = {targetwidth / 2.0f - mp.x / 2.0f, targetheight / 2.0f + m.y / 2.0f + 20};


    DrawTextEx(gfont, b3, pp, 20, 1, fgc);

    handlefade();
}


void libgameloadtextures() {
    textures[TXHERO] = LoadTexture("img/darkknight2-sheet.png");
    SetTextureFilter(textures[TXHERO], TEXTURE_FILTER_POINT);
}

void libgameunloadtextures() {
    if (textures[TXHERO].id > 0)
        UnloadTexture(textures[TXHERO]);
}


void libgameinitsharedsetup();

void libgameinit() {
    mprint("libgameinit");
    libgameinitsharedsetup();
    libgameloadtextures();
    g = gamestateinitptr();
}


void libgameinitsharedsetup() {
    gameinitwindow();

    //gfont = LoadFont("fonts/hack.ttf");
    gfont = LoadFontEx("fonts/hack.ttf", 60, 0, 250);
    SetTextureFilter(gfont.texture, TEXTURE_FILTER_BILINEAR);

    target = LoadRenderTexture(targetwidth, targetheight);
    //SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    //SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
}


void libgameinitwithstate(void* state) {
    if (state == NULL) {
        mprint("libgameinitwithstate: state is NULL");
        return;
    }
    mprint("libgameinitwithstate");
    libgameinitsharedsetup();
    libgameloadtextures();
    g = (gamestate*)state;
}


void libgameclosesavegamestate() {
    mprint("libgameclosesavegamestate");
    UnloadFont(gfont);

    mprint("unloading textures");
    libgameunloadtextures();

    mprint("unloading render texture");
    UnloadRenderTexture(target);

    mprint("closing window");
    CloseWindow();
}


void libgameclose() {
    mprint("libgameclose");
    UnloadFont(gfont);
    gamestatefree(g);

    mprint("unloading textures");
    libgameunloadtextures();

    mprint("unloading render texture");
    UnloadRenderTexture(target);

    mprint("closing window");
    CloseWindow();
}


gamestate* libgamegetgamestate() {
    return g;
}
