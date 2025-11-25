#include "libdraw_load_music.h"

void libdraw_load_music(shared_ptr<gamestate> g) {
    minfo("BEGIN load_music");
    // load the music stream from the selected path
    // randomly select a music path
    size_t index = GetRandomValue(0, g->music_file_paths->size() - 1);
    const char* music_path = g->music_file_paths->at(index).c_str();
    minfo("Music path: %s", music_path);
    music = LoadMusicStream(music_path);
    SetMasterVolume(1.0f);
    SetMusicVolume(music, 0.25f); // Set initial music volume
    PlayMusicStream(music);
    minfo("END load_music");
}
