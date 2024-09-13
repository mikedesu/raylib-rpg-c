#pragma once

#include <stdbool.h>

typedef struct companyscene {
    int x;
    int y;
    int scale;

    bool dodrawpresents;
    bool dodrawtest;

    //Texture2D presents;
    //Texture2D test;

    //Camera2D cam2d;

    //Camera3D cam3d;
    //CameraMode cameramode;

    // cube color
    //Color cubecolor;
    //Color cubewirecolor;

    //Vector3 cubepos;

} companyscene;


companyscene companysceneinit();
companyscene* companysceneinitptr();
void companyscenefree(companyscene* ts);
//void companysceneptrinitcameras(companyscene* c);
//void companyscenesetcubecolor(companyscene* c, Color color);
//void companyscenesetcubewirecolor(companyscene* c, Color color);
//void companysceneinitcubecolors(companyscene* c);
//void companysceneloadtextures(companyscene* c);
//void companysceneinitvars(companyscene* c);
//void companyscenedrawframe(companyscene* c);
