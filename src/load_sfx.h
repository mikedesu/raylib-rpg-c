/** @file load_sfx.h
 *  @brief Sound-effect loading helper.
 */

#pragma once

#include "gamestate.h"

/** @brief Load sound effects listed in `sfx.txt` into the active game state. */
static inline void libdraw_load_sfx(gamestate& g) {
    FILE* infile = fopen("sfx.txt", "r");
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), infile) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';
        string fullpath = "audio/sfx/" + string(buffer);
        Sound sound = LoadSound(fullpath.c_str());
        SetSoundVolume(sound, g.sfx_volume);
        g.sfx.push_back(sound);
    }
    fclose(infile);
}
