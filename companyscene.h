#pragma once

#include <raylib.h>

typedef struct companyscene {
    int x;
    int y;
    int scale;
    Texture2D presents;
    //Texture2D title;

    Camera2D cam2d;
    Camera3D cam3d;


    // cube color
    Color cubecolor;

} companyscene;

//#define COMPANYSCENESIZE (sizeof(int) * 3 + sizeof(Texture2D))
//#define COMPANYSCENESIZE (sizeof(int) * 3 + sizeof(Texture2D) + sizeof(Camera2D))
#define COMPANYSCENESIZE                                                                           \
    (sizeof(int) * 3 + sizeof(Texture2D) + sizeof(Camera2D) + sizeof(Camera3D) + sizeof(Color))


companyscene companysceneinit();
companyscene* companysceneinitptr();
void companyscenefree(companyscene* ts);
void companysceneptrinitcameras(companyscene* c);
