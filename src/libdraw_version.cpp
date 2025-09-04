#include "libdraw_version.h"
#include "libgame_defines.h"

void draw_version(const shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    const char* version = g->version.c_str();
    int font_size = 10;
    char buffer[1024] = {0};
    // also grab the current music track path
    snprintf(buffer, sizeof(buffer), "%s", version);
    Vector2 text_size = MeasureTextEx(GetFontDefault(), buffer, font_size, 1.0f);
    int w = DEFAULT_TARGET_WIDTH;
    int x = 20;
    int y = 20;
    DrawText(buffer, x, y, font_size, WHITE);
}
