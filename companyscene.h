#pragma once

#include <raylib.h>

typedef struct companyscene {
    int x;
    int y;
    int scale;
    Texture2D presents;
    //Texture2D title;
} companyscene;

#define COMPANYSCENESIZE (sizeof(int) * 3 + sizeof(Texture2D))


companyscene companysceneinit();
companyscene* companysceneinitptr();
void companyscenefree(companyscene* ts);
