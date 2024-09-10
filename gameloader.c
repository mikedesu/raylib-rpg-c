#include "display.h"
#include "gameloader.h"
#include "gamestate.h"
#include "mprint.h"
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

const int default_window_width = 1280;
const int default_window_height = 720;
const char* libname = "./libgame.so";
const char* lockfile = "./libgame.so.lockfile";
const char* templib = "./templibgame.so";

gamestate* g = NULL;
void* handle = NULL;

void (*myupdategamestate)(gamestate*) = NULL;
void (*myupdategamestateunsafe)(gamestate*) = NULL;

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


    mprint("check updategamestate");
    checksymbol(myupdategamestate, "updategamestate");

    mprint("check updategamestateunsafe");
    checksymbol(myupdategamestateunsafe, "updategamestateunsafe");


    mprint("end loadsymbols");
}


void initrendertexture(gamestate* g) {
    if (g) {
        g->d.target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        SetTextureFilter(g->d.target.texture, TEXTURE_FILTER_BILINEAR);
    }
}


void mygamestateinit() {
    g = gamestateinit();
    g->winwidth = default_window_width;
    g->winheight = default_window_height;
    g->cs = companysceneinitptr();

    g->d.font = LoadFontEx("fonts/hack.ttf", 20, 0, 250);
    SetTextureFilter(g->d.font.texture, TEXTURE_FILTER_BILINEAR);
}


void myinitwindow() {
    mprint("begin myinitwindow");
    // have to do inittitlescene after initwindow
    // cant load textures before initwindow
    InitWindow(default_window_width, default_window_height, "Game");
    SetTargetFPS(60);
    SetExitKey(KEY_Q);

    rlglInit(default_window_width, default_window_height);

    mygamestateinit();

    if (myupdategamestate == NULL) {
        fprintf(stderr, "dlsym failed or has not been loaded yet: %s\n", dlerror());
        loadsymbols();
    }
    myupdategamestate(g);

    mprint("end of myinitwindow");
}


void drawdebugpanel(gamestate* g) {
    if (g) {
        //const int fontsize = 20;
        const int pad = 5;
        Color bgc = (Color){g->dp.bgcolor.r, g->dp.bgcolor.g, g->dp.bgcolor.b, g->dp.bgcolor.a},
              fgc = (Color){g->dp.fgcolor.r, g->dp.fgcolor.g, g->dp.fgcolor.b, 255},
              borderc = (Color){g->dp.fgcolor.r, g->dp.fgcolor.g, g->dp.fgcolor.b, 255};
        int w = g->dp.w;
        int h = g->dp.h;
        int x = g->dp.x + pad;
        int y = g->dp.y + pad;
        DrawRectangle(x, y, w, h, bgc);
        DrawRectangleLines(x, y, w, h, borderc);
        x = g->dp.x + pad * 2;
        y = g->dp.y + pad * 2;
        //DrawTextEx(mydisplay.font, g->dp.bfr, (Vector2){x, y}, g->dp.fontsize, 0, fgc);
        DrawTextEx(g->d.font, g->dp.bfr, (Vector2){x, y}, g->dp.fontsize, 0, fgc);
    }
}


void drawcompanyscene(gamestate* g) {
    if (g) {
        companyscene* cs = g->cs;
        float w = cs->presents.width, h = cs->presents.height;
        float x = cs->x;
        float y = cs->y;
        float s = cs->scale;
        Rectangle src = {0, 0, w, h};
        Rectangle dst = {x, y, w * s, h * s};

        Vector3 cube = cs->cubepos;
        Color c = cs->cubecolor;
        Color c2 = cs->cubewirecolor;

        BeginMode3D(g->cs->cam3d);
        DrawGrid(10, 1.0f);

        DrawCube(cube, 1.0f, 1.0f, 1.0f, c);

        DrawCubeWires(cube, 1.0f, 1.0f, 1.0f, c2);


        //DrawCube(cube2, 1.0f, 1.0f, 1.0f, RED);
        src = (Rectangle){0, 0, g->cs->test.width / 4.0f, g->cs->test.height / 3.0f};

        //float ratio = src.height / src.width;
        float ratio = src.width / src.height;

        //Vector3 cube2 = {0.0f, 1.5f, 0.5f};
        //Vector3 cube2 = {0.0f, 1.51f, 0.5f};
        Vector3 cube2 = {cube.x, 1.51f, cube.z - 0.5f};

        //DrawCubeTextureRec(cs->test, src, cube2, ratio, 1.0f, 1.0f, WHITE);
        drawcubetexturerec(cs->test, src, cube2, ratio, 1.0f, 1.0f);
        // DrawCubeWires(cube2, ratio, 1.0f, 1.0f, WHITE);

        EndMode3D();
        BeginMode2D(g->cs->cam2d);

        // draw the "evildojo666 presents" texture
        if (g->cs->dodrawpresents) {
            src = (Rectangle){0, 0, w, h};
            DrawTexturePro(g->cs->presents, src, dst, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
        }

        //const float scale = 2.0f;
        //dst = (Rectangle){GetScreenWidth() / 2.0f - 20,
        //                  GetScreenHeight() / 2.0f - 90,
        //                  g->cs->test.width / 4.0f * scale,
        //                  g->cs->test.height / 3.0f * scale};
        // draw the test texture
        //if (g->cs->dodrawtest) {
        //    DrawTexturePro(g->cs->test, src, dst, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
        //}
    }
}


// Draw cube with texture piece applied to all faces
//void DrawCubeTextureRec(
void drawcubetexturerec(
    const Texture2D tex, const Rectangle src, Vector3 pos, float w, float h, float l) {
    const float x = pos.x, y = pos.y, z = pos.z, txw = tex.width, txh = tex.height,
                srcw = src.width, srch = src.height, srcx = src.x, srcy = src.y;
    const float rx = srcx + srcw, ry = srcy + srch, w2 = w / 2, h2 = h / 2, l2 = l / 2;
    const float xw0 = x - w2, xw1 = x + w2, yh0 = y - h2, yh1 = y + h2, z0 = z + l2;
    // precompute
    // vertices
    const Vector3 v[4] = {{xw0, yh0, z0}, {xw1, yh0, z0}, {xw1, yh1, z0}, {xw0, yh1, z0}};
    // texture coordinates
    const Vector2 t[4] = {{srcx / txw, ry / txh},
                          {rx / txw, ry / txh},
                          {rx / txw, srcy / txh},
                          {srcx / txw, srcy / txh}};
    // Set desired texture to be enabled while drawing following vertex data
    rlSetTexture(tex.id);

    // We calculate the normalized texture coordinates for the desired texture-source-rectangle
    // It means converting from (tex.width, tex.height) coordinates to [0.0f, 1.0f] equivalent
    rlBegin(RL_QUADS);
    //rlColor4ub(color.r, color.g, color.b, color.a);
    rlColor4ub(255, 255, 255, 255);
    // draw front face
    rlNormal3f(0.0f, 0.0f, 1.0f);
    for (int i = 0; i < 4; i++) {
        rlTexCoord2f(t[i].x, t[i].y);
        rlVertex3f(v[i].x, v[i].y, v[i].z);
    }

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

    rlEnd();

    rlSetTexture(0);
}


void drawfade(gamestate* s) {
    if (s) {
        drawfadeunsafe(s);
    }
}


void drawfadeunsafe(gamestate* g) {
    Color c = (Color){0, 0, 0, g->fadealpha};
    const int x = 0;
    const int y = 0;
    const int w = GetScreenWidth();
    const int h = GetScreenHeight();
    DrawRectangle(x, y, w, h, c);
    g->fadealpha += g->fadealphadir;
    if (g->fadealpha >= 255) {
        g->fadealphadir = -1;
    } else if (g->fadealpha <= 0) {
        g->fadealphadir = 1;
    }
}


void drawframe(gamestate* s) {
    if (s) {
        drawframeunsafe(s);
    }
}


void drawframeunsafe(gamestate* s) {
    //if (s) {
    const float dw = GetScreenWidth();
    const float dh = GetScreenHeight();
    const float rectw = 170;
    const float recth = 170;
    const float x = dw / 2 - rectw / 2;
    const float y = dh / 2 - recth / 2;
    float w = 200;
    float h = 200;

    BeginDrawing();
    BeginTextureMode(g->d.target);
    ClearBackground((Color){s->clearcolor.r, s->clearcolor.g, s->clearcolor.b, s->clearcolor.a});

    drawcompanyscene(s);

    EndMode2D();

    if (s->dodebugpanel) {
        drawdebugpanel(s);
    }


    if (s->dofps) {
        DrawFPS(dw - 100, 10);
    }

    EndTextureMode();

    w = g->d.target.texture.width;
    h = g->d.target.texture.height;
    Rectangle src = {0.0f, 0.0f, w, -h};
    Rectangle dst = {0.0f, 0.0f, dw, dh};

    DrawTexturePro(g->d.target.texture, src, dst, g->d.origin, 0.0f, WHITE);
    EndDrawing();
    g->framecount++;
}


void handleinput(gamestate* g) {
    if (g) {
        handleinputunsafe(g);
    }
}


void handleinputunsafe(gamestate* g) {
    if (IsKeyPressed(KEY_D)) {
        g->dodebugpanel = !g->dodebugpanel;
    }

    if (IsKeyPressed(KEY_SPACE)) {

        g->cs->cam3d.projection++;
        if (g->cs->cam3d.projection > CAMERA_ORTHOGRAPHIC) {
            g->cs->cam3d.projection = CAMERA_PERSPECTIVE;
        }

        //switch (g->cs->cam3d.projection) {
        //case CAMERA_PERSPECTIVE:
        //    g->cs->cam3d.projection = CAMERA_ORTHOGRAPHIC;
        //    break;
        //case CAMERA_ORTHOGRAPHIC:
        //    //g->cs->cam3d.projection = CAMERA_FREE;
        //    g->cs->cam3d.projection = CAMERA_ORBITAL;
        //    break;
        //case CAMERA_ORBITAL:
        //    g->cs->cam3d.projection = CAMERA_PERSPECTIVE;
        //    break;
        //default:
        //    g->cs->cam3d.projection = CAMERA_PERSPECTIVE;
        //    break;
        //}
    }

    if (IsKeyPressed(KEY_Z)) {
        // change camera mode
        g->cs->cameramode++;

        if (g->cs->cameramode > CAMERA_THIRD_PERSON) {
            g->cs->cameramode = CAMERA_CUSTOM;
        }
    }

    UpdateCamera(&g->cs->cam3d, g->cs->cameramode);

    // zoom in
    //if (IsKeyPressed(KEY_Z)) {
    //    g->cs->cam3d.fovy += 5;
    //    if (g->cs->cam3d.fovy > 90) {
    //        g->cs->cam3d.fovy = 90;
    //    }
    //}
}


void gameloop(gamestate* g) {
    if (g) {
        while (!WindowShouldClose()) {
            drawframeunsafe(g);
            myupdategamestateunsafe(g);
            handleinputunsafe(g);
            autoreload();
        }
    }
}


void autoreload() {
    if (getlastwritetime(libname) > last_write_time) {
        last_write_time = getlastwritetime(libname);
        while (FileExists(lockfile)) {
            printf("Library is locked\n");
            sleep(1);
        }
        // mprint("getting old gamestate");
        // gamestate* old_g = g;
        mprint("unloading library");
        dlclose(handle);
        mprint("opening handle");
        openhandle();
        mprint("loading symbols");
        loadsymbols();
    }
}


void unloaddisplay() {
    UnloadRenderTexture(g->d.target);
    UnloadFont(g->d.font);
}


void gamerun() {
    mprint("gamerun");
    mprint("opening handle");
    openhandle();
    mprint("loading symbols");
    loadsymbols();
    mprint("initing window");
    myinitwindow();
    mprint("initing rendertexture");
    initrendertexture(g);
    mprint("entering gameloop");
    gameloop(g);
    mprint("closing window");
    gamestatefree(g);
    unloaddisplay();

    //rlglClose();
    CloseWindow();
}
