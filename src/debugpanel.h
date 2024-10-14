#pragma once

#include <raylib.h>

//#define DEBUGPANELBUFSIZE 256
#define DEBUGPANELBUFSIZE 496
// game begins black-screening at 497, not sure why
//#define DEBUGPANELBUFSIZE 497

typedef struct debugpanel {
    int x;
    int y;
    int w;
    int h;
    Vector2 pos;
    Color fg_color;
    Color bg_color;
    //Rectangle textbox;
    Rectangle box;
    char buffer[DEBUGPANELBUFSIZE];
} debugpanel_t;
