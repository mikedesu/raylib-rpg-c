#include "gameloader.h"
#include "gamestate.h"
#include "inputstate.h"
#include "mprint.h"
#include <assert.h>
#include <dlfcn.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define LIBDRAW_PATH "./libdraw.so"
#define LIBLOGIC_PATH "./liblogic.so"

void* draw_handle = NULL;
void (*mylibdraw_init)() = NULL;
void (*mylibdraw_close)() = NULL;
bool (*mylibdraw_windowshouldclose)() = NULL;
void (*mylibdraw_drawframe)(const gamestate* const) = NULL;
void (*mylibdraw_update_input)(inputstate* const) = NULL;

void* logic_handle = NULL;
void (*myliblogic_init)(gamestate* const) = NULL;
void (*myliblogic_tick)(const inputstate* const, gamestate* const) = NULL;

long draw_last_write_time = 0;
long logic_last_write_time = 0;
int frame_count = 0;

void checksymbol(void* symbol, const char* name) {
    if (symbol == NULL) {
        fprintf(stderr, "dlsym failed: %s\n", dlerror());
        exit(1);
    }
}

long getlastwritetime(const char* filename) {
    struct stat file_stat;
    if (stat(filename, &file_stat) == 0) {
        return file_stat.st_mtime;
    }
    return 0;
}

void load_draw_symbols() {
    draw_handle = dlopen(LIBDRAW_PATH, RTLD_LAZY);
    if (!draw_handle) {
        fprintf(stderr, "dlopen failed for %s: %s\n", LIBDRAW_PATH, dlerror());
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

void load_logic_symbols() {
    logic_handle = dlopen(LIBLOGIC_PATH, RTLD_LAZY);
    if (!logic_handle) {
        fprintf(stderr, "dlopen failed for %s: %s\n", LIBLOGIC_PATH, dlerror());
        exit(1);
    }
    myliblogic_init = dlsym(logic_handle, "liblogic_init");
    checksymbol(myliblogic_init, "liblogic_init");
    myliblogic_tick = dlsym(logic_handle, "liblogic_tick");
    checksymbol(myliblogic_tick, "liblogic_tick");
}

void reload_draw() {
    long new_time = getlastwritetime(LIBDRAW_PATH);
    if (new_time > draw_last_write_time) {
        if (draw_handle) {
            mylibdraw_close();
            dlclose(draw_handle);
        }
        load_draw_symbols();
        mylibdraw_init();
        draw_last_write_time = new_time;
        msuccess("Reloaded libdraw.so");
    }
}

void reload_logic() {
    long new_time = getlastwritetime(LIBLOGIC_PATH);
    if (new_time > logic_last_write_time) {
        if (logic_handle) dlclose(logic_handle);
        load_logic_symbols();
        logic_last_write_time = new_time;
        msuccess("Reloaded liblogic.so");
    }
}

void autoreload_every_n_sec(const int n) {
    assert(n > 0);
    frame_count++;
    if (frame_count % (n * 60) == 0) {
        reload_draw();
        reload_logic();
    }
}

void gamerun() {
    inputstate is = {0};
    gamestate* g = gamestateinitptr();
    draw_last_write_time = getlastwritetime(LIBDRAW_PATH);
    logic_last_write_time = getlastwritetime(LIBLOGIC_PATH);

    load_draw_symbols();
    load_logic_symbols();
    mylibdraw_init();
    myliblogic_init(g);

    while (!mylibdraw_windowshouldclose()) {
        mylibdraw_update_input(&is);
        myliblogic_tick(&is, g);
        mylibdraw_drawframe(g);
        autoreload_every_n_sec(4);
        if (inputstate_is_pressed(&is, KEY_ESCAPE)) break;
    }

    mylibdraw_close();
    dlclose(draw_handle);
    dlclose(logic_handle);
    gamestatefree(g);
}
