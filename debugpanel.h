#pragma once

#include "mycolor.h"

#define DEBUGPANELBUFSIZE 256

typedef struct debugpanel {
    int x;
    int y;
    int w;
    int h;
    int fontsize;
    mycolor fgcolor;
    mycolor bgcolor;
    char bfr[DEBUGPANELBUFSIZE];
} debugpanel;

#define DEBUGPANELSIZE (sizeof(debugpanel))
