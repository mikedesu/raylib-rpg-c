#include "mprint.h"
#include "titlescene.h"

#include <stdlib.h>


titlescene titlesceneinit() {
    mprint("titlesceneinit");
    titlescene ts;
    mprint("Loading textures");
    mprint("Loading presents texture");
    ts.presents = LoadTexture("img/evildojo666-presents.png");
    if(ts.presents.id == 0) {
        mprint("Error loading presents texture");
    }
    return ts;
}

titlescene* titlesceneinitptr() {
    mprint("titlesceneinitptr");
    //titlescene* ts = (titlescene*)malloc(sizeof(titlescene));
    titlescene* ts = (titlescene*)malloc(TITLESCENESIZE);
    if(ts != NULL) {
        mprint("titlesceneinit: malloc success");
        mprint("Loading textures");
        mprint("Loading presents texture");
        ts->presents = LoadTexture("img/evildojo666-presents.png");
        if(ts->presents.id == 0) {
            mprint("Error loading presents texture");
        }

        //mprint("Loading title texture");
        //ts->title = LoadTexture("img/title2.png");
        //if(ts->title.id == 0) {
        //    mprint("Error loading title texture");
        //}
    }
    return ts;
}


void titlescenefree(titlescene* ts) {
    if(ts != NULL) {
        UnloadTexture(ts->presents);
        //UnloadTexture(ts->title);
        free(ts);
    }
}
