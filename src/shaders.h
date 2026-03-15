/** @file shaders.h
 *  @brief Shader loading and teardown helpers.
 */

#pragma once

#include "libdraw_context.h"
#include <raylib.h>
#include <unordered_map>

using std::unordered_map;

/** @brief Load the project's runtime shaders into the shared shader map. */
static inline void load_shaders() {
#ifdef WEB
    libdraw_ctx.shaders[1] = LoadShader(0, "shaders/web/green-glow.frag");
#else
    libdraw_ctx.shaders[1] = LoadShader(0, "shaders/desktop/green-glow.frag");
#endif

    //shader_grayscale = LoadShader(0, "grayscale.frag"); // No vertex shader needed
    //shader_glow = LoadShader(0, "glow.frag");
    //shader_red_glow = LoadShader(0, "red-glow.frag");
    //shader_color_noise = LoadShader(0, "colornoise.frag");
    //shader_psychedelic_0 = LoadShader(0, "psychedelic-0.frag");
    //shader_tile_glow = LoadShader(0, "psychedelic_ripple.frag");
}

/** @brief Unload the shaders currently stored in the shared shader map. */
static inline void unload_shaders() {
    UnloadShader(libdraw_ctx.shaders[1]);

    //UnloadShader(shader_grayscale);
    //UnloadShader(shader_glow);
    //UnloadShader(shader_red_glow);
    //UnloadShader(shader_color_noise);
    //UnloadShader(shader_psychedelic_0);
}
