#include "libdraw_from_texture.h"

extern RenderTexture2D title_target_texture;
extern Rectangle target_src;
extern Rectangle target_dest;
extern RenderTexture2D char_creation_target_texture;
extern RenderTexture2D hud_target_texture;

void draw_title_screen_from_texture(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    DrawTexturePro(title_target_texture.texture, target_src, target_dest, (Vector2){0, 0}, 0.0f, WHITE);
}


void draw_hud_from_texture(shared_ptr<gamestate> g) { DrawTexturePro(hud_target_texture.texture, target_src, target_dest, (Vector2){0, 0}, 0.0f, WHITE); }


void draw_character_creation_screen_from_texture(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    DrawTexturePro(char_creation_target_texture.texture, target_src, target_dest, (Vector2){0, 0}, 0.0f, WHITE);
}
