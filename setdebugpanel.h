#pragma once

#include "gamestate.h"

void setdebugpanelxy(gamestate* g, int x, int y);
void setdebugpaneltopleft(gamestate* g);
void setdebugpanelbottomleft(gamestate* g, const int h);
void setdebugpanelbottomright(gamestate* g, const int w, const int h);
void setdebugpaneltopright(gamestate* g, const int w);
void setdebugpanelcenter(gamestate* g, const int w, const int h);
