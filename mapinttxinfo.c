#include "mapinttxinfo.h"
#include <stdlib.h>

mapinttxinfonode* mapinttxinfonodecreate(txinfo info) {
    mapinttxinfonode* node = (mapinttxinfonode*)malloc(sizeof(mapinttxinfonode));
    node->info = info;
    node->next = NULL;
    return node;
}
