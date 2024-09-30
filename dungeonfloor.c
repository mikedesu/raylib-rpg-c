#include "dungeonfloor.h"

dungeonfloor_t* create_dungeonfloor(int len, int wid, tiletype_t basetype) {
    dungeonfloor_t* d = (dungeonfloor_t*)malloc(sizeof(dungeonfloor_t));
    if (!d) {
        //merror("could not malloc dungeonfloor_t");
        return NULL;
    }

    d->len = len;
    d->wid = wid;

    // we can, instead of doing a double pointer, do a single big malloc
    // and manage the dungeon by using coordinate address translation
    d->grid = (tile_t*)malloc(sizeof(tile_t) * len * wid);
    if (!d->grid) {
        //merror("could not malloc dungeonfloor grid tile*");
        return NULL;
    }

    //for (int i = 0; i < len; i++) {
    //    for (int j = 0; j < wid; j++) {
    //        int index = i * wid + j;
    //        d->grid[index].type = basetype;
    //    }
    //}

    dungeonfloor_setalltiles(d, basetype);

    return d;
}


void dungeonfloor_setalltiles(dungeonfloor_t* d, tiletype_t type) {
    for (int i = 0; i < d->len; i++) {
        for (int j = 0; j < d->wid; j++) {
            int index = i * d->wid + j;
            d->grid[index].type = type;
        }
    }
}


void dungeonfloor_free(dungeonfloor_t* d) {
    free(d->grid);
    free(d);
}
