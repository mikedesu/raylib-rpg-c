#pragma once

#include <raylib.h>

typedef struct titlescene {
    Texture2D presents;
    //Texture2D title;
} titlescene;

#define TITLESCENESIZE (sizeof(Texture2D))


titlescene titlesceneinit();
titlescene* titlesceneinitptr();
void titlescenefree(titlescene* ts);
