#include "gameloader.h"
#include "gamestate.h"
#include "inputstate.h"
#include "massert.h"
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
void (*mylibdraw_init)(const gamestate* const) = NULL;
void (*mylibdraw_close)() = NULL;
bool (*mylibdraw_windowshouldclose)() = NULL;
void (*mylibdraw_drawframe)(const gamestate* const) = NULL;
void (*mylibdraw_update_sprites)(gamestate* const) = NULL;
void (*mylibdraw_update_input)(inputstate* const) = NULL;

void* logic_handle = NULL;
void (*myliblogic_init)(gamestate* const) = NULL;
void (*myliblogic_tick)(const inputstate* const, gamestate* const) = NULL;
void (*myliblogic_close)(gamestate* const) = NULL;

long draw_last_write_time = 0;
long logic_last_write_time = 0;
int frame_count = 0;

void checksymbol(void* symbol, const char* name) {
    massert(symbol, "dlsym failed: %s", name);
    massert(name, "dlsym failed: %s", name);
    massert(strlen(name) > 0, "dlsym failed: %s", name);

    //if (symbol == NULL) {
    //    fprintf(stderr, "dlsym failed: %s\n", dlerror());
    //    exit(1);
    //}
}

bool file_changed(const char* path, long* last_time) {
    long t = getlastwritetime(path);
    if (t > *last_time) {
        *last_time = t;
        return true;
    }
    return false;
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
    mylibdraw_update_sprites = dlsym(draw_handle, "libdraw_update_sprites");
    checksymbol(mylibdraw_update_sprites, "libdraw_update_sprites");
}

void load_logic_symbols() {
    logic_handle = dlopen(LIBLOGIC_PATH, RTLD_LAZY);
    massert(logic_handle, "dlopen failed for %s: %s", LIBLOGIC_PATH, dlerror());
    //if (!logic_handle) {
    //    fprintf(stderr, "dlopen failed for %s: %s\n", LIBLOGIC_PATH, dlerror());
    //    exit(1);
    //}
    myliblogic_init = dlsym(logic_handle, "liblogic_init");
    checksymbol(myliblogic_init, "liblogic_init");
    myliblogic_tick = dlsym(logic_handle, "liblogic_tick");
    checksymbol(myliblogic_tick, "liblogic_tick");
    myliblogic_close = dlsym(logic_handle, "liblogic_close");
    checksymbol(myliblogic_close, "liblogic_close");
}

//void reload_draw(const gamestate* const g) {
//    long new_time = getlastwritetime(LIBDRAW_PATH);
//    if (new_time > draw_last_write_time) {
//        if (draw_handle) {
//            mylibdraw_close();
//            dlclose(draw_handle);
//        }
//        load_draw_symbols();
//        mylibdraw_init(g);
//        draw_last_write_time = new_time;
//        msuccess("Reloaded libdraw.so");
//    }
//}

//void reload_draw(const gamestate* const g) {
//    if (draw_handle) {
//        void (*temp_close)() = mylibdraw_close; // Save close fn
//        if (temp_close) {
//            temp_close();
//        }
//        dlclose(draw_handle);
//    }
//
//    load_draw_symbols();
//    mylibdraw_init(g);
//}

void reload_draw(const gamestate* const g) {
    if (draw_handle) {
        mylibdraw_close(); // Direct call
        dlclose(draw_handle);
    }

    load_draw_symbols(); // Let it crash if fails
    mylibdraw_init(g);
    draw_last_write_time = getlastwritetime(LIBDRAW_PATH);
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

//void reload_logic() {
//    long new_time = getlastwritetime(LIBLOGIC_PATH);
//    if (new_time > logic_last_write_time) {
//        if (logic_handle) dlclose(logic_handle);
//        load_logic_symbols();
//        logic_last_write_time = new_time;
//        msuccess("Reloaded liblogic.so");
//    }
//}

//void autoreload_every_n_sec(const int n, const gamestate* const g) {
//    massert(n > 0, "autoreload_every_n_sec: n must be greater than 0");
//    frame_count++;
//    if (frame_count % (n * 60) == 0) {
//        reload_logic();
//        reload_draw(g);
//    }
//}

void autoreload_every_n_sec(const int n, const gamestate* const g) {
    frame_count++;
    if (frame_count % (n * 60) == 0) {
        if (file_changed(LIBDRAW_PATH, &draw_last_write_time)) {
            reload_draw(g);
        }
        if (file_changed(LIBLOGIC_PATH, &logic_last_write_time)) {
            reload_logic();
        }
    }
}

void gamerun() {
    inputstate is = {0};
    gamestate* g = gamestateinitptr();
    draw_last_write_time = getlastwritetime(LIBDRAW_PATH);
    logic_last_write_time = getlastwritetime(LIBLOGIC_PATH);
    load_draw_symbols();
    load_logic_symbols();
    // i think liblogic will need init before libdraw because
    // we will want to create entries in the entitymap AFTER it is init'd
    myliblogic_init(g);
    mylibdraw_init(g);
    while (!mylibdraw_windowshouldclose()) {
        mylibdraw_update_input(&is);
        myliblogic_tick(&is, g);
        mylibdraw_update_sprites(g);
        mylibdraw_drawframe(g);
        autoreload_every_n_sec(4, g);
        if (inputstate_is_pressed(&is, KEY_ESCAPE)) break;
    }
    mylibdraw_close();
    msuccess("libdraw closed");
    myliblogic_close(g);
    msuccess("liblogic closed");
    dlclose(draw_handle);
    msuccess("dlclose libdraw");
    dlclose(logic_handle);
    msuccess("dlclose liblogic");
    gamestatefree(g);
    msuccess("gamestate freed");
}
