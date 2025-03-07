#pragma once

long getlastwritetime(const char* filename);
void gamerun();
//void openhandle();
void open_libdraw_handle();
void checksymbol(void* symbol, const char* name);
void loadsymbols();
void autoreload();
void autoreload_every_n_sec(const int n);
