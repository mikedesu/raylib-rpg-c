/** @file load_music.h
 *  @brief Background music loading and stream-rotation helpers.
 */

#pragma once

#include "gamestate.h"
#include "libdraw_context.h"
#include "libgame_defines.h"
#include <random>

/** @brief Load one random music track from the configured music path list. */
static inline void load_random_music(gamestate& g) {
    minfo2("BEGIN load_random_music");
    // load the music stream from the selected path
    // randomly select a music path
    uniform_int_distribution<int> gen(0, g.music_file_paths.size() - 1);
    const size_t index = gen(g.mt);
    const char* music_path = g.music_file_paths[index].c_str();
    minfo("Music path: %s", music_path);
    libdraw_ctx.music = LoadMusicStream(music_path);
    libdraw_ctx.music.looping = false;
    minfo2("END load_random_music");
}

/** @brief Advance the active music stream and rotate tracks when playback ends. */
static inline void handle_music_stream(gamestate& g) {
    UpdateMusicStream(libdraw_ctx.music);
    // handle load next music track
    if (!IsMusicStreamPlaying(libdraw_ctx.music)) {
        StopMusicStream(libdraw_ctx.music);
        UnloadMusicStream(libdraw_ctx.music);
        load_random_music(g);

        //#ifdef MUSIC_OFF
        //        SetMusicVolume(music, 0.0f); // Set initial music volume
        //#else
        //        SetMusicVolume(music, DEFAULT_MUSIC_VOLUME); // Set initial music volume
        //#endif
        //        PlayMusicStream(music);


        // if MUSIC_OFF is undefined, or MUSIC_ON is defined
        //#ifndef MUSIC_OFF
        //        SetMusicVolume(music, music_volume); // Set initial music volume
        //#endif
        //#ifdef MUSIC_ON
        //        SetMusicVolume(music, music_volume); // Set initial music volume
        //#endif

#ifdef MUSIC_VOLUME
        libdraw_ctx.music_volume = MUSIC_VOLUME;
#endif


// if MUSIC_OFF is defined
#ifdef MUSIC_OFF
        libdraw_ctx.music_volume = 0.0f;
        //SetMusicVolume(libdraw_ctx.music, libdraw_ctx.music_volume); // Set initial music volume
#endif

        SetMusicVolume(libdraw_ctx.music, libdraw_ctx.music_volume); // Set initial music volume
        g.music_volume = libdraw_ctx.music_volume;
        PlayMusicStream(libdraw_ctx.music);
    }
}

/** @brief Initialize music playback, master volume, and starting track state. */
static inline void libdraw_load_music(gamestate& g) {
    minfo2("BEGIN load_music");
    load_random_music(g);

#ifndef MASTER_VOLUME
    libdraw_ctx.master_volume = DEFAULT_MASTER_VOLUME;
#else
    libdraw_ctx.master_volume = MASTER_VOLUME;
#endif


#ifdef MUSIC_OFF
    libdraw_ctx.music_volume = 0.0f;
#else
#ifdef MUSIC_VOLUME
    libdraw_ctx.music_volume = MUSIC_VOLUME;
#else
    libdraw_ctx.music_volume = DEFAULT_MUSIC_VOLUME;
#endif
#endif

    //SetMusicVolume(libdraw_ctx.music, DEFAULT_MUSIC_VOLUME); // Set initial music volume

    g.master_volume = libdraw_ctx.master_volume;
    SetMasterVolume(libdraw_ctx.master_volume);
    SetMusicVolume(libdraw_ctx.music, libdraw_ctx.music_volume); // Set initial music volume
    g.music_volume = libdraw_ctx.music_volume;

    PlayMusicStream(libdraw_ctx.music);
    minfo2("END load_music");
}
