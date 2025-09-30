#include "libdraw_version.h"

void draw_version(const shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    const char* version = g->version.c_str();
    char buffer[1024] = {0};
    int x = 20, y = 20, font_size = 10;
    snprintf(buffer, sizeof(buffer), "%s", version);
    DrawText(buffer, x, y, font_size, WHITE);
}
