#include "dungeonfloor.h"
#include "mprint.h"

dungeonfloor_t* create_dungeonfloor(int len, int wid, tiletype_t basetype) {
    minfo("create_dungeonfloor");
    dungeonfloor_t* d = (dungeonfloor_t*)malloc(sizeof(dungeonfloor_t));
    if (!d) {
        merror("could not malloc dungeonfloor_t");
        return NULL;
    }

    d->len = len;
    d->wid = wid;

    // we can, instead of doing a double pointer, do a single big malloc
    // and manage the dungeon by using coordinate address translation
    d->grid = (tile_t*)malloc(sizeof(tile_t) * len * wid);
    if (!d->grid) {
        merror("could not malloc dungeonfloor grid tile*");
        return NULL;
    }

    dungeonfloor_set_all_tiles_to_type(d, basetype);
    dungeonfloor_init_all_tiles_entityids(d);

    return d;
}


void dungeonfloor_set_all_tiles_to_type(dungeonfloor_t* d, tiletype_t type) {
    minfo("dungeonfloor_set_all_tiles_to_type");
    for (int i = 0; i < d->len; i++) {
        for (int j = 0; j < d->wid; j++) {
            int index = i * d->wid + j;
            d->grid[index].type = type;
        }
    }
}


void dungeonfloor_init_all_tiles_entityids(dungeonfloor_t* d) {
    minfo("dungeonfloor_init_all_tiles_entityids");
    for (int i = 0; i < d->len; i++) {
        for (int j = 0; j < d->wid; j++) {
            int index = i * d->wid + j;
            d->grid[index].entityids = vectorentityid_new();
        }
    }
}



void dungeonfloor_free(dungeonfloor_t* d) {
    minfo("dungeonfloor_free");
    free(d->grid);
    free(d);
}




//tile_t* dungeonfloor_get_tile(dungeonfloor_t* d, Vector2 pos) {
tile_t* dungeonfloor_get_tile(dungeonfloor_t* d, int x, int y) {
    if (d) {

        if (x < d->len && y < d->wid) {
            int index = x * d->wid + y;
            return &d->grid[index];
        }

        merror("dungeonfloor_get_tile: pos out of bounds");
        fprintf(stderr, "pos: (%d, %d)\n", x, y);
        fprintf(stderr, "d->len: %d, d->wid: %d\n", d->len, d->wid);
        return NULL;
    }

    merror("dungeonfloor_get_tile: d is NULL");
    return NULL;
}




void dungeonfloor_set_tiletype_at(dungeonfloor_t* d, tiletype_t type, int x, int y) {
    if (d) {
        dungeonfloor_set_tiletype_at_unsafe(d, type, x, y);
    } else {
        merror("dungeonfloor_set_tiletype_at: d is NULL");
    }
}




void dungeonfloor_set_tiletype_at_unsafe(dungeonfloor_t* d, tiletype_t type, int x, int y) {
    if (x < d->len && y < d->wid) {
        int index = x * d->wid + y;
        d->grid[index].type = type;
    } else {
        merror("dungeonfloor_set_tiletype_at: pos out of bounds");
        fprintf(stderr, "pos: (%d, %d)\n", x, y);
        fprintf(stderr, "d->len: %d, d->wid: %d\n", d->len, d->wid);
    }
}
