#pragma once



#include <stdbool.h>
#include <time.h>



time_t getlastwritetime(const char* filename);
void gamerun();
void openhandle();
void checksymbol(void* symbol, const char* name);
void loadsymbols();
void autoreload();
