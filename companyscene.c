#include "companyscene.h"
#include "mprint.h"

#include <stdlib.h>


companyscene companysceneinit() {
    mprint("companysceneinit");
    companyscene ts;
    mprint("Loading textures");
    mprint("Loading presents texture");
    ts.presents = LoadTexture("img/evildojo666-presents.png");
    if(ts.presents.id == 0) {
        mprint("Error loading presents texture");
    }
    ts.x = 0;
    ts.y = 0;
    ts.scale = 1;
    return ts;
}

companyscene* companysceneinitptr() {
    mprint("companysceneinitptr");
    //companyscene* ts = (companyscene*)malloc(sizeof(companyscene));
    companyscene* ts = (companyscene*)malloc(COMPANYSCENESIZE);
    if(ts != NULL) {
        mprint("companysceneinit: malloc success");
        mprint("Loading textures");
        mprint("Loading presents texture");
        ts->presents = LoadTexture("img/evildojo666-presents.png");
        if(ts->presents.id == 0) {
            mprint("Error loading presents texture");
        }
        ts->x = 0;
        ts->y = 0;
        ts->scale = 1;

        //mprint("Loading company texture");
        //ts->company = LoadTexture("img/company2.png");
        //if(ts->company.id == 0) {
        //    mprint("Error loading company texture");
        //}
    }
    return ts;
}


void companyscenefree(companyscene* ts) {
    if(ts != NULL) {
        UnloadTexture(ts->presents);
        //UnloadTexture(ts->company);
        free(ts);
    }
}
