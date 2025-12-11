#pragma once

#include "gamestate.h"
#include "textureinfo.h"


static inline bool libdraw_unload_texture(textureinfo* txinfo, int txkey) {
    massert(txinfo != NULL, "txinfo is NULL");
    massert(txkey >= 0, "txkey is invalid");
    if (txkey < 0 || txkey >= GAMESTATE_SIZEOFTEXINFOARRAY)
        return false;

    UnloadTexture(txinfo[txkey].texture);
    txinfo[txkey].texture = (Texture2D){0};
    txinfo[txkey].contexts = 0;
    return true;
}


static inline void libdraw_unload_textures(textureinfo* txinfo) {
    for (int i = 0; i < GAMESTATE_SIZEOFTEXINFOARRAY; i++)
        libdraw_unload_texture(txinfo, i);

    //UnloadRenderTexture(title_target_texture);
    //UnloadRenderTexture(char_creation_target_texture);
    //UnloadRenderTexture(main_game_target_texture);
    //UnloadRenderTexture(hud_target_texture);
    //UnloadRenderTexture(target);
}
