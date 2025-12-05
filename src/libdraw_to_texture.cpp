#include "libdraw_draw_character_creation_screen.h"
#include "libdraw_frame_2d.h"
#include "libdraw_handle_debug_panel.h"
#include "libdraw_hud.h"
#include "libdraw_title_screen.h"
#include "libdraw_to_texture.h"

extern RenderTexture2D title_target_texture;
extern Rectangle target_src;
extern Rectangle target_dest;
extern RenderTexture2D char_creation_target_texture;
extern RenderTexture2D hud_target_texture;
extern RenderTexture2D main_game_target_texture;


void draw_hud_to_texture(shared_ptr<gamestate> g) {
    BeginTextureMode(hud_target_texture);
    draw_hud(g);
    EndTextureMode();
}

void draw_character_creation_screen_to_texture(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    BeginTextureMode(char_creation_target_texture);
    draw_character_creation_screen(g);
    handle_debug_panel(g);
    EndTextureMode();
}

void draw_title_screen_to_texture(shared_ptr<gamestate> g, bool show_menu) {
    massert(g, "gamestate is NULL");
    BeginTextureMode(title_target_texture);
    draw_title_screen(g, show_menu);
    handle_debug_panel(g);
    EndTextureMode();
}

void libdraw_drawframe_2d_to_texture(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    BeginTextureMode(main_game_target_texture);
    minfo("entering drawframe_2d");
    libdraw_drawframe_2d(g);
    minfo("exitting drawframe_2d");
    EndTextureMode();
}
