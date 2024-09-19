#pragma once

//#include "mycolor.h"

//#define DEBUGPANELBUFSIZE 256
#define DEBUGPANELBUFSIZE 496
// game begins black-screening at 497, not sure why
//#define DEBUGPANELBUFSIZE 497

typedef struct debugpanel {
    int x;
    int y;
    int w;
    int h;
    //int fontsize;
    //mycolor fgcolor;
    //mycolor bgcolor;
    //char bfr[DEBUGPANELBUFSIZE];
} debugpanel_t;
