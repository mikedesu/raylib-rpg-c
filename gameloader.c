#include "gameloader.h"
#include "gamestate.h"
#include "mprint.h"
//#include "display.h"
//#include "rcamera.h"

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

unsigned int old_framecount = 0;

const int default_window_width = 960;
const int default_window_height = 540;

const char* libname = "./libgame.so";
const char* lockfile = "./libgame.so.lockfile";
const char* templib = "./templibgame.so";

void* handle = NULL;

gamestate* g = NULL;

bool (*mywindowshouldclose)(void) = NULL;
void (*myinitwindow)() = NULL;
void (*myclosewindow)() = NULL;
void (*mylibgamedrawframe)() = NULL;
void (*mylibgameinit)() = NULL;
void (*mylibgameinitwithstate)(void*) = NULL;
void (*mylibgameclose)() = NULL;
void (*mylibgameclosesavegamestate)() = NULL;
void (*mylibgamehandleinput)() = NULL;
void (*mylibgameinitframecount)(unsigned int) = NULL;

void (*mylibgameupdategamestate)(gamestate*) = NULL;

gamestate* (*mylibgamegetgamestate)() = NULL;


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
    if (symbol == NULL) {
        fprintf(stderr, "dlsym failed: %s\n", dlerror());
        exit(1);
    }
}


void loadsymbols() {
    mprint("begin loadsymbols");
    //mprint("updategamestate");
    //myupdategamestate = (void (*)(gamestate*))dlsym(handle, "updategamestate");
    //mprint("updategamestateunsafe");
    //myupdategamestateunsafe = (void (*)(gamestate*))dlsym(handle, "updategamestateunsafe");
    // casting these to indicate return types and possible args

    mprint("windowshouldclose");
    mywindowshouldclose = (bool (*)(void))dlsym(handle, "gamewindowshouldclose");
    checksymbol(mywindowshouldclose, "gamewindowshouldclose");

    mprint("initwindow");
    myinitwindow = dlsym(handle, "gameinitwindow");
    checksymbol(myinitwindow, "gameinitwindow");

    mprint("closewindow");
    myclosewindow = dlsym(handle, "gameclosewindow");
    checksymbol(myclosewindow, "gameclosewindow");

    mprint("libgamedrawframe");
    mylibgamedrawframe = dlsym(handle, "libgamedrawframe");
    checksymbol(mylibgamedrawframe, "libgamedrawframe");

    mprint("libgameinit");
    mylibgameinit = dlsym(handle, "libgameinit");
    checksymbol(mylibgameinit, "libgameinit");

    mprint("libgameclose");
    mylibgameclose = dlsym(handle, "libgameclose");
    checksymbol(mylibgameclose, "libgameclose");

    mprint("libgamehandleinput");
    mylibgamehandleinput = dlsym(handle, "libgamehandleinput");
    checksymbol(mylibgamehandleinput, "libgamehandleinput");

    mprint("libgamegetgamestate");
    mylibgamegetgamestate = dlsym(handle, "libgamegetgamestate");
    checksymbol(mylibgamegetgamestate, "libgamegetgamestate");

    mprint("libgameclosesavegamestate");
    mylibgameclosesavegamestate = dlsym(handle, "libgameclosesavegamestate");
    checksymbol(mylibgameclosesavegamestate, "libgameclosesavegamestate");

    mprint("libgameinitwithstate");
    mylibgameinitwithstate = (void (*)(void*))dlsym(handle, "libgameinitwithstate");
    checksymbol(mylibgameinitwithstate, "libgameinitwithstate");


    mprint("libgameupdategamestate");
    mylibgameupdategamestate = (void (*)(gamestate*))dlsym(handle, "libgameupdategamestate");
    checksymbol(mylibgameupdategamestate, "libgameupdategamestate");

    //////////////////////////////////////

    mprint("end loadsymbols");
}


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

void autoreload() {
    if (getlastwritetime(libname) > last_write_time) {
        last_write_time = getlastwritetime(libname);
        //while (FileExists(lockfile)) {
        while (access(lockfile, F_OK) == 0) {
            printf("Library is locked\n");
            sleep(1);
        }
        mprint("getting old gamestate");
        g = mylibgamegetgamestate();
        // this time, we have to shut down the game and close the window
        // before we can reload and restart everything
        mprint("closing libgame");
        mylibgameclosesavegamestate(); // closes window
        mprint("unloading library");
        dlclose(handle);
        mprint("opening handle");
        openhandle();
        mprint("loading symbols");
        loadsymbols();
        mylibgameinitwithstate(g);
    }
}


void gamerun() {
    mprint("gamerun");
    mprint("opening handle");
    openhandle();
    mprint("loading symbols");
    loadsymbols();
    mprint("initing window");
    last_write_time = getlastwritetime(libname);
    mylibgameinit();
    mprint("entering gameloop");
    while (!mywindowshouldclose()) {

        mylibgamedrawframe();

        mylibgamehandleinput();

        mylibgameupdategamestate(g);

        autoreload();
    }
    mprint("closing libgame");
    mylibgameclose();
}
