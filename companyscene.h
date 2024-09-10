#pragma once

#include "sizemacros.h"
#include <raylib.h>

typedef struct companyscene {
    int x;
    int y;
    int scale;

    bool dodrawpresents;
    bool dodrawtest;

    Texture2D presents;
    Texture2D test;

    Camera2D cam2d;

    Camera3D cam3d;
    CameraMode cameramode;


    // cube color
    Color cubecolor;
    Color cubewirecolor;

    Vector3 cubepos;


} companyscene;


//#define COMPANYSCENESIZE                                                                           \
//    (SIZEOFINT(3) + SIZEOFT2D(2) + SIZEOFCAMERAS + SIZEOFCOLOR(2) + SIZEOFVEC3(1) + SIZEOFBOOL(1))
//(SIZEOFINT(3) + sizeof(Texture2D) + sizeof(Camera2D) + sizeof(Camera3D) + sizeof(Color))
//(sizeof(int) * 3 + sizeof(Texture2D) + sizeof(Camera2D) + sizeof(Camera3D) + sizeof(Color))


companyscene companysceneinit();
companyscene* companysceneinitptr();
void companyscenefree(companyscene* ts);
void companysceneptrinitcameras(companyscene* c);
void companyscenesetcubecolor(companyscene* c, Color color);
void companyscenesetcubewirecolor(companyscene* c, Color color);
void companysceneinitcubecolors(companyscene* c);
void companysceneloadtextures(companyscene* c);
void companysceneinitvars(companyscene* c);
