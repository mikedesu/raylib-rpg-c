#pragma once

#include "gamestate.h"

#include <stdbool.h>
#include <time.h>

time_t getlastwritetime(const char* filename);
void myinitwindow();
void drawframe(gamestate* g);
void drawframeunsafe(gamestate* g);
void gameloop(gamestate* g);
void gamerun();
void updateframecountbuffer();
void openhandle();
void loadsymbols();
void autoreload();
void drawdebugpanel(gamestate* g);
void initrendertexture(gamestate* g);
void drawcompanyscene(gamestate* g);
void handleinput(gamestate* g);
void handleinputunsafe(gamestate* g);
void drawfade(gamestate* g);
void drawfadeunsafe(gamestate* g);
void unloaddisplay();
void mygamestateinit();

//void DrawCubeTextureRec(const Texture2D texture,
//                        const Rectangle source,
//                        const Vector3 position,
//                        const float width,
//                        const float height,
//                        const float length,
//                        const Color color);
//
void drawcubetexturerec(const Texture2D texture,
                        const Rectangle source,
                        const Vector3 position,
                        const float width,
                        const float height,
                        const float length);
