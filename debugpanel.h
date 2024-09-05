#pragma once


//typedef struct MyColor {
//	unsigned char r;
//	unsigned char g;
//	unsigned char b;
//	unsigned char a;
//} MyColor;

typedef struct debugpanel {
    int x;
    int y;
    int w;
    int h;
    //MyColor fgcolor;
    //MyColor bgcolor;
    char bfr[256];

} debugpanel;


#define DEBUGPANELSIZE (sizeof(int) * 4 + sizeof(char) * 256)
