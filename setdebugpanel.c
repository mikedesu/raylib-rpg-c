#include "setdebugpanel.h"

void setdebugpanelxy(gamestate* g, int x, int y) {
    if (g != NULL) {
        g->debugpanel.x = x;
        g->debugpanel.y = y;
    }
}


void setdebugpaneltopleft(gamestate* g) {
    setdebugpanelxy(g, 20, 20);
}


void setdebugpanelbottomleft(gamestate* g, const int h) {
    setdebugpanelxy(g, 20, h - g->debugpanel.h - 40);
}


void setdebugpanelbottomright(gamestate* g, const int w, const int h) {
    setdebugpanelxy(g, w - g->debugpanel.w - 20, h - g->debugpanel.h - 40);
}


void setdebugpaneltopright(gamestate* g, const int w) {
    setdebugpanelxy(g, w - g->debugpanel.w - 20, 20);
}


void setdebugpanelcenter(gamestate* g, const int w, const int h) {
    setdebugpanelxy(g, w / 2 - g->debugpanel.w / 2, h / 2 - g->debugpanel.h / 2);
}
