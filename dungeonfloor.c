#include "dungeonfloor.h"

dungeonfloor_t* create_dungeonfloor(int len, int wid, tiletype_t basetype) {
    dungeonfloor_t* d = (dungeonfloor_t*)malloc(sizeof(dungeonfloor_t));
    if (!d) {
        mprint("could not malloc dungeonfloor_t");
        return NULL;
    }

    d->len = len;
    d->wid = wid;

    d->grid = (tile_t**)malloc(sizeof(tile_t*) * len);
    if (!d->grid) {
        mprint("could not malloc dungeonfloor grid tile**");
        return NULL;
    }

    for (int i = 0; i < len; i++) {
        d->grid[i] = (tile_t*)malloc(sizeof(tile_t) * wid);
        if (!d->grid[i]) {
            mprint("could not malloc d->grid[i]");
            return NULL;
        }

        for (int j = 0; j < wid; j++) {
            d->grid[i][j].type = basetype;
        }
    }

    return d;
}


void dungeonfloor_free(dungeonfloor_t* d) {
    for (int i = 0; i < d->len; i++) {
        free(d->grid[i]);
    }
    free(d->grid);
    free(d);
}
