#pragma once

#define setdebugpaneltopleft(g) (g->debugpanel.x = 20, g->debugpanel.y = 20)

#define setdebugpaneltopright(g)                                               \
    (g->debugpanel.x = g->display.windowwidth - g->debugpanel.w - 20,          \
     g->debugpanel.y = 20)

#define setdebugpanelbottomleft(g)                                             \
    (g->debugpanel.x = 20,                                                     \
     g->debugpanel.y = g->display.windowheight - g->debugpanel.h - 20)

#define setdebugpanelbottomright(g)                                            \
    (g->debugpanel.x = g->display.windowwidth - g->debugpanel.w - 20,          \
     g->debugpanel.y = g->display.windowheight - g->debugpanel.h - 20)

#define setdebugpanelcenter(g)                                                 \
    (g->debugpanel.x = g->display.windowwidth / 2 - g->debugpanel.w / 2,       \
     g->debugpanel.y = g->display.windowheight / 2 - g->debugpanel.h / 2)
