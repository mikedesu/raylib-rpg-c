#pragma once

#include "txinfo.h"

// forward declaration

typedef struct mapinttxinfonode mapinttxinfonode;

typedef struct mapinttxinfonode {
    txinfo info;
    mapinttxinfonode* next;
} mapinttxinfonode;

#define MAPINTTEXTURENODESIZE sizeof(mapinttexturenode)

typedef struct mapinttexture {
    mapinttxinfonode* map[256];
} mapinttxinfo;


mapinttxinfonode* mapinttexturenodecreate(txinfo info);
