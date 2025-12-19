#pragma once

#include "gamestate.h"

static inline void libdraw_load_sfx(shared_ptr<gamestate> g) {
    FILE* infile = fopen("sfx.txt", "r");
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), infile) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';
        string fullpath = "audio/sfx/" + string(buffer);
        Sound sound = LoadSound(fullpath.c_str());
        g->sfx.push_back(sound);
    }
    fclose(infile);
}
