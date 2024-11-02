#pragma once

#define setdebugpaneltopleft(g) (g->debugpanel.x = 20, g->debugpanel.y = 20)
#define setdebugpaneltopright(g) (g->debugpanel.x = g->windowwidth - g->debugpanel.w - 20, g->debugpanel.y = 20)
#define setdebugpanelbottomleft(g) (g->debugpanel.x = 20, g->debugpanel.y = g->windowheight - g->debugpanel.h - 20)
#define setdebugpanelbottomright(g) (g->debugpanel.x = g->windowwidth - g->debugpanel.w - 20, g->debugpanel.y = g->windowheight - g->debugpanel.h - 20)
#define setdebugpanelcenter(g) (g->debugpanel.x = g->windowwidth / 2 - g->debugpanel.w / 2, g->debugpanel.y = g->windowheight / 2 - g->debugpanel.h / 2)
