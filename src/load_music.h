#pragma once

#include "gamestate.h"

extern Music music;

static inline void libdraw_load_music(gamestate& g) {
    minfo("BEGIN load_music");
    // load the music stream from the selected path
    // randomly select a music path
    //size_t index = GetRandomValue(0, g.music_file_paths->size() - 1);
    size_t index = GetRandomValue(0, g.music_file_paths.size() - 1);
    //const char* music_path = g.music_file_paths->at(index).c_str();
    const char* music_path = g.music_file_paths.at(index).c_str();
    minfo("Music path: %s", music_path);
    music = LoadMusicStream(music_path);
    SetMasterVolume(1.0f);
#ifdef MUSIC_OFF
    SetMusicVolume(music, 0.0f); // Set initial music volume
#else
    SetMusicVolume(music, 0.50f); // Set initial music volume
#endif
    PlayMusicStream(music);
    minfo("END load_music");
}
