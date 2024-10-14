#pragma once

typedef struct mycolor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} mycolor;

#define MYCOLORSIZE (sizeof(unsigned char) * 4)
