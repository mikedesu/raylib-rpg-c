#pragma once

#include "gamestate.h"
#include "libgame_defines.h"
#include <raylib.h>

/**
 * @brief Capture the frame start timestamp in debug builds.
 *
 * @return The current time in seconds when debug timing is enabled, otherwise `0.0`.
 */
static inline double libdraw_frame_begin_time() {
#ifdef DEBUG
    return GetTime();
#else
    return 0.0;
#endif
}

/**
 * @brief Finalize per-frame timing statistics in debug builds.
 *
 * @param g Active game state receiving timing statistics.
 * @param start_time Timestamp returned by `libdraw_frame_begin_time()`.
 */
static inline void libdraw_finish_frame_stats(gamestate& g, double start_time) {
#ifdef DEBUG
    g.last_frame_time = GetTime() - start_time;
    g.last_frame_times[g.last_frame_times_current] = g.last_frame_time;
    g.last_frame_times_current++;
    if (g.last_frame_times_current >= LAST_FRAME_TIMES_MAX) {
        g.last_frame_times_current = 0;
    }
    g.framecount++;

    if (g.last_frame_time > g.max_frame_time) {
        g.max_frame_time = g.last_frame_time;
    }
#else
    (void)g;
    (void)start_time;
#endif
}
