#pragma once

int getlastwritetime(const char* filename);
void gamerun();
//void openhandle();
void open_libdraw_handle();
void checksymbol(void* symbol, const char* name);
void loadsymbols();
void autoreload();
