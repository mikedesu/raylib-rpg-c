#pragma once

#include "gamestate.h"

#include <stdbool.h>
#include <time.h>

//bool mywindowshouldclose();
//bool myiskeypressed(int key);
//unsigned int getframecount();
time_t getlastwritetime(const char* filename);
void myinitwindow();
void drawframe(gamestate* g);
void drawframeunsafe(gamestate* g);
void gameloop();
void gamerun();
void updateframecountbuffer();
void openhandle();
void loadsymbols();
void autoreload();
void drawdebugpanel();
void initrendertexture();
void drawcompanyscene();
void handleinput();
void drawfade(gamestate* g);
void drawfadeunsafe(gamestate* g);
