#include "gameloader.h"
#include "inputstate.h"
#include <dlfcn.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void* draw_handle = NULL;
void (*mylibdraw_init)() = NULL;
void (*mylibdraw_close)() = NULL;
bool (*mylibdraw_windowshouldclose)() = NULL;
void (*mylibdraw_drawframe)() = NULL;
void (*mylibdraw_update_input)(inputstate* const) = NULL;

void checksymbol(void* symbol, const char* name) {
    if (symbol == NULL) {
        fprintf(stderr, "dlsym failed: %s\n", dlerror());
        exit(1);
    }
}

void load_draw_symbols() {
    draw_handle = dlopen("./libdraw.so", RTLD_LAZY);
    if (!draw_handle) {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        exit(1);
    }
    mylibdraw_init = dlsym(draw_handle, "libdraw_init");
    checksymbol(mylibdraw_init, "libdraw_init");
    mylibdraw_close = dlsym(draw_handle, "libdraw_close");
    checksymbol(mylibdraw_close, "libdraw_close");
    mylibdraw_windowshouldclose = dlsym(draw_handle, "libdraw_windowshouldclose");
    checksymbol(mylibdraw_windowshouldclose, "libdraw_windowshouldclose");
    mylibdraw_drawframe = dlsym(draw_handle, "libdraw_drawframe");
    checksymbol(mylibdraw_drawframe, "libdraw_drawframe");
    mylibdraw_update_input = dlsym(draw_handle, "libdraw_update_input");
    checksymbol(mylibdraw_update_input, "libdraw_update_input");
}

void gamerun() {
    inputstate is = {0};
    load_draw_symbols();
    mylibdraw_init();
    while (!mylibdraw_windowshouldclose()) {
        mylibdraw_update_input(&is);
        mylibdraw_drawframe();
        if (inputstate_is_pressed(&is, KEY_ESCAPE)) break;
    }
    mylibdraw_close();
    dlclose(draw_handle);
}
