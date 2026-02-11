#include "gamestate.h"
#include "inputstate.h"
#include "libdraw.h"
#ifdef WEB
#include <emscripten/emscripten.h>
#endif

inputstate is;
gamestate g;

unordered_map<entityid, spritegroup*> spritegroups;
textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];
unordered_map<int, Shader> shaders;
RenderTexture2D title_target_texture;
RenderTexture2D char_creation_target_texture;
RenderTexture2D main_game_target_texture;
RenderTexture2D hud_target_texture;
RenderTexture2D target;
Rectangle target_src = {0, 0, DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT};
Rectangle target_dest = {0, 0, DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT};
Rectangle win_dest = {0, 0, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};
Music music;
int ANIM_SPEED = DEFAULT_ANIM_SPEED;

static inline void gameloop() {
    inputstate_update(is);
    g.tick(is);
    drawframe(g);
    if (g.do_restart) {
        msuccess("Restarting game...");
        libdraw_close();
        g.logic_close();
        g.reset();
        g.logic_init();
        libdraw_init(g);
        g.do_restart = false; // Reset restart flag
        g.restart_count++;
    }
}

int main() {
    g.logic_init();
    libdraw_init(g);
#ifndef WEB
    while (!libdraw_windowshouldclose(g)) {
        gameloop();
    }
#else
    emscripten_set_main_loop(gameloop, 0, 1);
#endif
    libdraw_close();
    g.logic_close();
    return 0;
}
