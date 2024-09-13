#include "companyscene.h"
#include "mprint.h"

#include <raylib.h>
#include <stdlib.h>

companyscene companysceneinit() {
    mprint("companysceneinit");
    companyscene ts;
    //mprint("Loading textures");
    //mprint("Loading presents texture");
    //companysceneloadtextures(&ts);
    //companysceneinitvars(&ts);
    //companysceneptrinitcameras(&ts);
    //companysceneinitcubecolors(&ts);
    return ts;
}

companyscene* companysceneinitptr() {
    mprint("companysceneinitptr");
    companyscene* ts = (companyscene*)malloc(sizeof(companyscene));
    if (ts != NULL) {
        mprint("companysceneinit: malloc success");
        //mprint("Loading textures");
        //mprint("Loading presents texture");
        //        companysceneloadtextures(ts);
        //companysceneinitvars(ts);
        //companysceneptrinitcameras(ts);
        //companysceneinitcubecolors(ts);
    } else {
        mprint("companysceneinit: malloc failed");
    }
    return ts;
}


void companyscenefree(companyscene* ts) {
    if (ts != NULL) {
        //UnloadTexture(ts->presents);
        //UnloadTexture(ts->test);
        free(ts);
    }
}


//void companysceneptrinitcameras(companyscene* c) {
//    if (c != NULL) {
//        c->cam2d = (Camera2D){0};
//        c->cam2d.zoom = 1;
//        c->cam2d.rotation = 0;
//        c->cam2d.target = (Vector2){0};
//        c->cam2d.offset = (Vector2){0};
//
//        c->cam3d = (Camera3D){0};
//        c->cam3d.position = (Vector3){0, 5, 5};
//        c->cam3d.target = (Vector3){0, 0, 0};
//        c->cam3d.up = (Vector3){0, 1, 0};
//        c->cam3d.fovy = 45;
//        c->cam3d.projection = CAMERA_PERSPECTIVE;
//    }
//}


//void companyscenesetcubecolor(companyscene* c, Color color) {
//    if (c != NULL) {
//        c->cubecolor = color;
//    }
//}


//void companyscenesetcubewirecolor(companyscene* c, Color color) {
//    if (c != NULL) {
//        c->cubewirecolor = color;
//    }
//}


//void companysceneinitcubecolors(companyscene* c) {
//    if (c) {
//        c->cubecolor = (Color){0x33, 0x33, 0x33, 255};
//        c->cubewirecolor = (Color){0, 0, 0, 255};
//    }
//}


//void companysceneloadtextures(companyscene* c) {
//    if (c) {
//        mprint("Loading textures");
//        mprint("Loading presents texture");
//        c->presents = LoadTexture("img/evildojo666-presents.png");
//        if (c->presents.id == 0) {
//            mprint("Error loading presents texture");
//        }
//
//        mprint("Loading test texture");
//        c->test = LoadTexture("img/darkknight2-sheet.png");
//        if (c->test.id == 0) {
//            mprint("Error loading test texture");
//        }
//    }
//}


//void companysceneinitvars(companyscene* c) {
//    if (c) {
//        c->x = 0;
//        c->y = 0;
//        c->scale = 1;
//        c->cubepos = (Vector3){0.0f, 0.5f, 1.0f};
//        c->dodrawpresents = false;
//        c->dodrawtest = true;
//        c->cameramode = CAMERA_FREE;
//    }
//}
//
//
//void companyscenedrawframe(companyscene* c) { }
