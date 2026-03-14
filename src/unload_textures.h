/** @file unload_textures.h
 *  @brief Texture teardown helpers for the generated texture table.
 */

#pragma once

#include "gamestate.h"
#include "textureinfo.h"

/** @brief Unload one texture-table entry and clear its cached metadata. */
constexpr static inline bool libdraw_unload_texture(textureinfo* txinfo, int txkey) {
    massert(txinfo != NULL, "txinfo is NULL");
    massert(txkey >= 0, "txkey is invalid");
    if (txkey < 0 || txkey >= GAMESTATE_SIZEOFTEXINFOARRAY) {
        return false;
    }
    UnloadTexture(txinfo[txkey].texture);
    txinfo[txkey].texture = (Texture2D){0};
    txinfo[txkey].contexts = 0;
    return true;
}

/** @brief Unload every texture currently stored in the texture table. */
static inline void libdraw_unload_textures(textureinfo* txinfo) {
    for (int i = 0; i < GAMESTATE_SIZEOFTEXINFOARRAY; i++) {
        if (!libdraw_unload_texture(txinfo, i)) {
            merror("Failed to unload texture %d", i);
        }
    }
}
