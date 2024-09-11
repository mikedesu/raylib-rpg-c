#include "display.h"
#include "gameloader.h"
#include "gamestate.h"
#include "mprint.h"
#include "rcamera.h"
#include <dlfcn.h>
#include <raylib.h>
#include <rlgl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

long last_write_time = 0;

const int default_window_width = 960;
const int default_window_height = 540;
//const int default_window_width = 1280;
//const int default_window_height = 720;

const char* libname = "./libgame.so";
const char* lockfile = "./libgame.so.lockfile";
const char* templib = "./templibgame.so";

//gamestate* g = NULL;
void* handle = NULL;

bool (*mywindowshouldclose)(void) = NULL;
void (*myupdategamestate)(gamestate*) = NULL;
void (*myupdategamestateunsafe)(gamestate*) = NULL;

void (*myinitwindow)() = NULL;
void (*myclosewindow)() = NULL;
void (*mygameloop)(gamestate*) = NULL;

// get the last write time of a file
time_t getlastwritetime(const char* filename) {
    struct stat file_stat;
    time_t retval = 0;
    if (stat(filename, &file_stat) == 0) {
        retval = file_stat.st_mtime;
    }
    return retval;
}


void openhandle() {
    handle = dlopen(libname, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        exit(1);
    }
}


void checksymbol(void* symbol, const char* name) {
    //mprint("begin check symbol");
    if (symbol == NULL) {
        fprintf(stderr, "dlsym failed: %s\n", dlerror());
        exit(1);
    }
    //mprint("end check symbol");
}


void loadsymbols() {
    mprint("begin loadsymbols");


    mprint("updategamestate");
    myupdategamestate = (void (*)(gamestate*))dlsym(handle, "updategamestate");


    mprint("updategamestateunsafe");
    myupdategamestateunsafe = (void (*)(gamestate*))dlsym(handle, "updategamestateunsafe");


    mprint("mywindowshouldclose");
    mywindowshouldclose = (bool (*)(void))dlsym(handle, "gamewindowshouldclose");


    mprint("myinitwindow");
    myinitwindow = (void (*)())dlsym(handle, "gameinitwindow");


    mprint("myclosewindow");
    myclosewindow = (void (*)())dlsym(handle, "gameclosewindow");


    mprint("mygameloop");
    mygameloop = (void (*)(gamestate*))dlsym(handle, "gameloop");


    mprint("check updategamestate");
    checksymbol(myupdategamestate, "updategamestate");

    mprint("check updategamestateunsafe");
    checksymbol(myupdategamestateunsafe, "updategamestateunsafe");

    mprint("check mywindowshouldclose");
    checksymbol(mywindowshouldclose, "gamewindowshouldclose");

    mprint("check myinitwindow");
    checksymbol(myinitwindow, "gameinitwindow");

    mprint("check myclosewindow");
    checksymbol(myclosewindow, "gameclosewindow");


    mprint("check mygameloop");
    checksymbol(mygameloop, "gameloop");


    mprint("end loadsymbols");
}


//void initrendertexture(gamestate* g) {
//    if (g) {
//        g->d.target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
//        SetTextureFilter(g->d.target.texture, TEXTURE_FILTER_BILINEAR);
//    }
//}


//void mygamestateinit() {
//    g = gamestateinit();
//    g->winwidth = default_window_width;
//    g->winheight = default_window_height;
//    g->cs = companysceneinitptr();
//    g->d.font = LoadFontEx("fonts/hack.ttf", 20, 0, 250);
//    SetTextureFilter(g->d.font.texture, TEXTURE_FILTER_BILINEAR);
//}


//void myinitwindow() {
//    mprint("begin myinitwindow");
//    // have to do inittitlescene after initwindow
//    // cant load textures before initwindow
//    InitWindow(default_window_width, default_window_height, "Game");
//    SetTargetFPS(60);
//    SetExitKey(KEY_Q);
//    rlglInit(default_window_width, default_window_height);
//    mprint("end of myinitwindow");
//}


//void drawdebugpanel(gamestate* g) {
//    if (g) {
//        const int pad = 5;
//        const Color bgc = {g->dp.bgcolor.r, g->dp.bgcolor.g, g->dp.bgcolor.b, g->dp.bgcolor.a};
//        const Color fgc = {g->dp.fgcolor.r, g->dp.fgcolor.g, g->dp.fgcolor.b, 255};
//        const Color borderc = {g->dp.fgcolor.r, g->dp.fgcolor.g, g->dp.fgcolor.b, 255};
//        const int x = g->dp.x + pad * 2;
//        const int y = g->dp.y + pad * 2;
//        const Vector2 v = {x, y};
//        DrawRectangle(g->dp.x + pad, g->dp.y + pad, g->dp.w, g->dp.h, bgc);
//        DrawRectangleLines(g->dp.x, g->dp.y, g->dp.w, g->dp.h, borderc);
//        DrawTextEx(g->d.font, g->dp.bfr, v, g->dp.fontsize, 0, fgc);
//    }
//}


//void drawcompanyscene(gamestate* g) {
//    if (g) {
//        companyscene* cs = g->cs;
//        const float w = cs->presents.width, h = cs->presents.height;
//        const float x = cs->x;
//        const float y = cs->y;
//        const float s = cs->scale;
//        Rectangle src = {0, 0, w, h};
//        Rectangle dst = {x, y, w * s, h * s};
//        const Vector3 cube = cs->cubepos;
//        const Color c = cs->cubecolor;
//        const Color c2 = cs->cubewirecolor;
//        BeginMode3D(g->cs->cam3d);
//        DrawGrid(10, 1.0f);
//        DrawCube(cube, 1.0f, 1.0f, 1.0f, c);
//        DrawCubeWires(cube, 1.0f, 1.0f, 1.0f, c2);
//        src = (Rectangle){0, 0, g->cs->test.width / 4.0f, g->cs->test.height / 3.0f};
//        const float ratio = src.width / src.height;
//        const Vector3 cube2 = {cube.x, 1.51f, cube.z - 0.5f};
//        drawcubetexturerec(cs->test, src, cube2, ratio, 1.0f, 1.0f);
//        EndMode3D();
//        BeginMode2D(g->cs->cam2d);
//        // draw the "evildojo666 presents" texture
//        if (g->cs->dodrawpresents) {
//            src = (Rectangle){0, 0, w, h};
//            DrawTexturePro(g->cs->presents, src, dst, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
//        }
//    }
//}


// Draw cube with texture piece applied to all faces
//void drawcubetexturerec(
//    const Texture2D tex, const Rectangle src, Vector3 pos, float w, float h, float l) {
//const float rx = src.x + src.width;
//const float ry = src.y + src.height;
//const float a = pos.x - w / 2;
//const float b = pos.x + w / 2;
//const float c = pos.y - h / 2;
//const float d = pos.y + h / 2;
//const float e = pos.z + l / 2;
//const float f = pos.z - l / 2;
// precompute
// vertices
//const Vector3 v[4] = {{a, c, e}, {b, c, e}, {b, d, e}, {a, d, e}};
// texture coordinates
//const Vector2 t[4] = {{src.x / tex.width, ry / tex.height},
//                      {rx / tex.width, ry / tex.height},
//                      {rx / tex.width, src.y / tex.height},
//                      {src.x / tex.width, src.y / tex.height}};
// Set desired texture to be enabled while drawintex.height tex.widthollowintex.height vertex data
//rlSetTexture(tex.id);
// We calculate the normalized texture coordinates for the desired texture-source-rectangle
// It means converting from (tex.width, tex.height) coordinates to [0.0f, 1.0f] equivalent
//rlBegin(RL_QUADS);
//rlColor4ub(255, 255, 255, 255);
// draw front face
//rlNormal3f(0.0f, 0.0f, 1.0f);
//for (int i = 0; i < 4; i++) {
//    rlTexCoord2f(t[i].x, t[i].y);
//    rlVertex3f(v[i].x, v[i].y, v[i].z);
//}

/////////////////////////////////////////////////////////

// Back face
//rlNormal3f(0.0f, 0.0f, -1.0f);
//rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
//rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
//rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
//rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
//rlTexCoord2f(source.x / texWidth, source.y / texHeight);
//rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
//rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
//rlVertex3f(x + width / 2, y - height / 2, z - length / 2);

// Top face
//rlNormal3f(0.0f, 1.0f, 0.0f);
//rlTexCoord2f(source.x / texWidth, source.y / texHeight);
//rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
//rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
//rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
//rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
//rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
//rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
//rlVertex3f(x + width / 2, y + height / 2, z - length / 2);

// Bottom face
//rlNormal3f(0.0f, -1.0f, 0.0f);
//rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
//rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
//rlTexCoord2f(source.x / texWidth, source.y / texHeight);
//rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
//rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
//rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
//rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
//rlVertex3f(x - width / 2, y - height / 2, z + length / 2);

// Right face
//rlNormal3f(1.0f, 0.0f, 0.0f);
//rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
//rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
//rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
//rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
//rlTexCoord2f(source.x / texWidth, source.y / texHeight);
//rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
//rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
//rlVertex3f(x + width / 2, y - height / 2, z + length / 2);

// Left face
//rlNormal3f(-1.0f, 0.0f, 0.0f);
//rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
//rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
//rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
//rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
//rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
//rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
//rlTexCoord2f(source.x / texWidth, source.y / texHeight);
//rlVertex3f(x - width / 2, y + height / 2, z - length / 2);

//rlEnd();

//rlSetTexture(0);
//}


//void drawfade(gamestate* s) {
//    if (s) {
//        drawfadeunsafe(s);
//    }
//}


//void drawfadeunsafe(gamestate* g) {
//const Color c = (Color){0, 0, 0, g->fadealpha};
//const int x = 0;
//const int y = 0;
//const int w = GetScreenWidth();
//const int h = GetScreenHeight();
//DrawRectangle(x, y, w, h, c);
//g->fadealpha += g->fadealphadir;
//if (g->fadealpha >= 255) {
//    g->fadealphadir = -1;
//} else if (g->fadealpha <= 0) {
//    g->fadealphadir = 1;
//}
//}


//void drawframe(gamestate* s) {
//    if (s) {
//        drawframeunsafe(s);
//    }
//}


//void drawframeunsafe(gamestate* s) {
//if (s) {
//const float dw = GetScreenWidth();
//const float dh = GetScreenHeight();
//const float rectw = 170;
//const float recth = 170;
//const float x = dw / 2 - rectw / 2;
//const float y = dh / 2 - recth / 2;
//const float w = 200;
//const float h = 200;

//BeginDrawing();
//BeginTextureMode(g->d.target);
//ClearBackground((Color){s->clearcolor.r, s->clearcolor.g, s->clearcolor.b, s->clearcolor.a});

//drawcompanyscene(s);

//EndMode2D();

//if (s->dodebugpanel) {
//    drawdebugpanel(s);
//}

//if (s->dofps) {
//    DrawFPS(dw - 100, 10);
//}

//EndTextureMode();

// we've finished drawing to the target texture
// now we can draw the target texture to the window
//const float w2 = g->d.target.texture.width;
//const float h2 = g->d.target.texture.height;
//const Rectangle src = {0.0f, 0.0f, w2, -h2};
//const Rectangle dst = {0.0f, 0.0f, dw, dh};

//DrawTexturePro(g->d.target.texture, src, dst, g->d.origin, 0.0f, WHITE);
//EndDrawing();
//g->framecount++;
//}


//void handleinput(gamestate* g) {
//    if (g) {
//        handleinputunsafe(g);
//    }
//}


//void handleinputunsafe(gamestate* g) {
//if (IsKeyPressed(KEY_D)) {
//    g->dodebugpanel = !g->dodebugpanel;
//}
//
//if (IsKeyPressed(KEY_ONE)) {
//    g->cs->cameramode = CAMERA_FREE;
//    g->cs->cam3d.up = (Vector3){0.0f, 1.0f, 0.0f}; // Reset roll
//}

//if (IsKeyPressed(KEY_TWO)) {
//    g->cs->cameramode = CAMERA_FIRST_PERSON;
//    g->cs->cam3d.up = (Vector3){0.0f, 1.0f, 0.0f}; // Reset roll
//}

//if (IsKeyPressed(KEY_THREE)) {
//    g->cs->cameramode = CAMERA_THIRD_PERSON;
//    g->cs->cam3d.up = (Vector3){0.0f, 1.0f, 0.0f}; // Reset roll
//}

//if (IsKeyPressed(KEY_FOUR)) {
//    g->cs->cameramode = CAMERA_ORBITAL;
//    g->cs->cam3d.up = (Vector3){0.0f, 1.0f, 0.0f}; // Reset roll
//}

// Switch camera projection
//if (IsKeyPressed(KEY_P)) {
//    if (g->cs->cam3d.projection == CAMERA_PERSPECTIVE) {
//        // Create isometric view
//        g->cs->cameramode = CAMERA_THIRD_PERSON;
//        // Note: The target distance is related to the render distance in the orthographic projection
//        g->cs->cam3d.position = (Vector3){0.0f, 2.0f, -100.0f};
//        g->cs->cam3d.target = (Vector3){0.0f, 2.0f, 0.0f};
//        g->cs->cam3d.up = (Vector3){0.0f, 1.0f, 0.0f};
//        g->cs->cam3d.projection = CAMERA_ORTHOGRAPHIC;
//        g->cs->cam3d.fovy = 20.0f; // near plane width in CAMERA_ORTHOGRAPHIC
//        CameraYaw(&g->cs->cam3d, -135 * DEG2RAD, true);
//        CameraPitch(&g->cs->cam3d, -45 * DEG2RAD, true, true, false);
//    } else if (g->cs->cam3d.projection == CAMERA_ORTHOGRAPHIC) {
//        // Reset to default view
//        g->cs->cameramode = CAMERA_THIRD_PERSON;
//        g->cs->cam3d.position = (Vector3){0.0f, 2.0f, 10.0f};
//        g->cs->cam3d.target = (Vector3){0.0f, 2.0f, 0.0f};
//        g->cs->cam3d.up = (Vector3){0.0f, 1.0f, 0.0f};
//        g->cs->cam3d.projection = CAMERA_PERSPECTIVE;
//        g->cs->cam3d.fovy = 60.0f;
//    }
//}

//UpdateCamera(&g->cs->cam3d, g->cs->cameramode);
//}


//void gameloop(gamestate* g) {
//if (g) {
//while (!WindowShouldClose()) {
//    while (!mywindowshouldclose()) {
//drawframeunsafe(g);
//myupdategamestateunsafe(g);
//handleinputunsafe(g);

//autoreload();
//    }
//}
//}


//void autoreload() {
//    if (getlastwritetime(libname) > last_write_time) {
//        last_write_time = getlastwritetime(libname);
//        while (FileExists(lockfile)) {
//            printf("Library is locked\n");
//            sleep(1);
//        }
//        // mprint("getting old gamestate");
//        // gamestate* old_g = g;
//        mprint("unloading library");
//        dlclose(handle);
//        mprint("opening handle");
//        openhandle();
//        mprint("loading symbols");
//        loadsymbols();
//    }
//}


//void unloaddisplay() {
//UnloadRenderTexture(g->d.target);
//UnloadFont(g->d.font);
//}


void gamerun() {
    mprint("gamerun");
    mprint("opening handle");
    openhandle();
    mprint("loading symbols");
    loadsymbols();
    mprint("initing window");

    //myinitwindow();

    //g = gamestateinit();
    //g->winwidth = default_window_width;
    //g->winheight = default_window_height;
    //g->cs = companysceneinitptr();
    //g->d.font = LoadFontEx("fonts/hack.ttf", 20, 0, 250);
    //SetTextureFilter(g->d.font.texture, TEXTURE_FILTER_BILINEAR);
    //if (myupdategamestate == NULL) {
    //    fprintf(stderr, "dlsym failed or has not been loaded yet: %s\n", dlerror());
    //    loadsymbols();
    //}
    //myupdategamestate(g);

    //mprint("initing rendertexture");
    //initrendertexture(g);

    mprint("entering gameloop");

    //gameloop(NULL);
    mygameloop(NULL);

    mprint("closing window");
    myclosewindow();
    //gamestatefree(g);

    //unloaddisplay();

    //rlglClose();
    //CloseWindow();
}
