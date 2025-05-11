#include "gameloader.h"
#include "gamestate.h"
#include "inputstate.h"
#include "massert.h"
#include "mprint.h"
#include <assert.h>
#include <dlfcn.h>
#include <raylib.h>
#include <stdbool.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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

void checksymbol(void* symbol, const char* name) { massert(symbol, "dlsym failed: %s", name); }

bool file_exists(const char* path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
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
    struct stat f_stat;
    if (stat(filename, &f_stat) == 0) return f_stat.st_mtime;
    return 0;
}

void load_draw_symbols() {
    draw_handle = dlopen(LIBDRAW_PATH, RTLD_LAZY);
    massert(draw_handle, "dlopen failed for %s: %s", LIBDRAW_PATH, dlerror());
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
    minfo("Loading liblogic.so...");
    //logic_handle = dlopen(LIBLOGIC_PATH, RTLD_LAZY);
    logic_handle = dlopen(LIBLOGIC_PATH, RTLD_NOW | RTLD_LOCAL); // No lazy binding
    massert(logic_handle, "dlopen failed for %s: %s", LIBLOGIC_PATH, dlerror());
    myliblogic_init = dlsym(logic_handle, "liblogic_init");
    checksymbol(myliblogic_init, "liblogic_init");
    myliblogic_tick = dlsym(logic_handle, "liblogic_tick");
    checksymbol(myliblogic_tick, "liblogic_tick");
    myliblogic_close = dlsym(logic_handle, "liblogic_close");
    checksymbol(myliblogic_close, "liblogic_close");
    msuccess("Loaded liblogic.so");
}

void reload_draw(const gamestate* const g) {
    massert(g, "reload_draw: gamestate is NULL");
    if (draw_handle) {
        mylibdraw_close(); // Direct call
        dlclose(draw_handle);
    }
    load_draw_symbols(); // Let it crash if fails
    mylibdraw_init(g);
    draw_last_write_time = getlastwritetime(LIBDRAW_PATH);
}

void reload_logic() {
    minfo("Reloading liblogic.so...");
    minfo("liblogic.so has changed, reloading...");
    massert(logic_handle, "reload_logic: logic_handle is NULL");
    minfo("Closing liblogic.so...");
    dlclose(logic_handle);
    load_logic_symbols();
    logic_last_write_time = getlastwritetime(LIBLOGIC_PATH);

    minfo("liblogic_tick address: %p", myliblogic_tick);

    msuccess("Reloaded liblogic.so");
}

void autoreload_every_n_sec(int n, gamestate* g) {
    static double last = 0;
    double now = GetTime();

    if (now - last >= n) {
        // Check draw lib

        if (file_changed(LIBDRAW_PATH, &draw_last_write_time)) {
            minfo("Checking libdraw.so...");

            while (file_exists(LIBDRAW_PATH ".lockfile")) usleep(1000);

            msuccess("Reloading libdraw.so...");
            reload_draw(g);
            last = now; // Reset timer immediately
        }

        // Check logic lib
        if (file_changed(LIBLOGIC_PATH, &logic_last_write_time)) {
            minfo("Checking liblogic.so...");

            while (file_exists(LIBLOGIC_PATH ".lockfile")) usleep(1000);

            msuccess("Reloading liblogic.so...");
            reload_logic();
            last = now; // Reset timer immediately
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

    myliblogic_init(g);
    mylibdraw_init(g);

    while (!mylibdraw_windowshouldclose()) {
        mylibdraw_update_input(&is);
        myliblogic_tick(&is, g);
        mylibdraw_update_sprites(g);
        mylibdraw_drawframe(g);
        autoreload_every_n_sec(5, g);
        //if (inputstate_is_pressed(&is, KEY_ESCAPE)) break;
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
