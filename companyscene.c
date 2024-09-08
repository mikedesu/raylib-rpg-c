#include "companyscene.h"
#include "mprint.h"

#include <raylib.h>
#include <stdlib.h>


void companysceneptrinitcameras(companyscene* c) {
    if (c != NULL) {
        c->cam2d = (Camera2D){0};
        c->cam2d.zoom = 1;
        c->cam2d.rotation = 0;
        c->cam2d.target = (Vector2){0};
        c->cam2d.offset = (Vector2){0};

        c->cam3d = (Camera3D){0};
        c->cam3d.position = (Vector3){0, 5, 5};
        c->cam3d.target = (Vector3){0, 0, 0};
        //c->cam3d.target = (Vector3){0, 0, 0};
        c->cam3d.up = (Vector3){0, 1, 0};
        c->cam3d.fovy = 45;

        c->cam3d.projection = CAMERA_PERSPECTIVE;
        //c->cam3d.projection = CAMERA_ORTHOGRAPHIC;
        //c->cam3d.projection = CAMERA_FREE;
        //c->cam3d.projection = CAMERA_CUSTOM;
        //c->cam3d.projection = CAMERA_ORBITAL;
    }
}


companyscene companysceneinit() {
    mprint("companysceneinit");
    companyscene ts;
    mprint("Loading textures");
    mprint("Loading presents texture");
    ts.presents = LoadTexture("img/evildojo666-presents.png");
    if (ts.presents.id == 0) {
        mprint("Error loading presents texture");
    }
    ts.x = 0;
    ts.y = 0;
    ts.scale = 1;

    companysceneptrinitcameras(&ts);

    return ts;
}

companyscene* companysceneinitptr() {
    mprint("companysceneinitptr");
    //companyscene* ts = (companyscene*)malloc(sizeof(companyscene));
    companyscene* ts = (companyscene*)malloc(COMPANYSCENESIZE);
    if (ts != NULL) {
        mprint("companysceneinit: malloc success");
        mprint("Loading textures");
        mprint("Loading presents texture");
        ts->presents = LoadTexture("img/evildojo666-presents.png");
        if (ts->presents.id == 0) {
            mprint("Error loading presents texture");
        }
        ts->x = 0;
        ts->y = 0;
        ts->scale = 1;

        companysceneptrinitcameras(ts);
    }
    return ts;
}


void companyscenefree(companyscene* ts) {
    if (ts != NULL) {
        UnloadTexture(ts->presents);
        //UnloadTexture(ts->company);
        free(ts);
    }
}
