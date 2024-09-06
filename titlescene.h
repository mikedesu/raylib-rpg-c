#pragma once

#include <raylib.h>

typedef struct titlescene {
    int x;
    int y;
    int scale;
    Texture2D presents;
    //Texture2D title;
} titlescene;

#define TITLESCENESIZE (sizeof(int) * 3 + sizeof(Texture2D))


titlescene titlesceneinit();
titlescene* titlesceneinitptr();
void titlescenefree(titlescene* ts);
