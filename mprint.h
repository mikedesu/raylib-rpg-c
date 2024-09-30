#pragma once

//#include "gamestate.h"



//#define minfo(s, ...) printf(s, ##__VA_ARGS__)

#define minfo(s)                                                               \
    printf(                                                                    \
        stdout, "📔 \033[34;1mInfo\033[0m %s:%d: %s\n", __FILE__, __LINE__, s)

#define merror(s)                                                              \
    fprintf(stderr,                                                            \
            "☠️ \033[31;1mError\033[0m %s:%d: %s\n",                            \
            __FILE__,                                                          \
            __LINE__,                                                          \
            s);
// forward declaration
//typedef struct gamestate gamestate;
