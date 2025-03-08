#include "libdraw.h"
#include "gamestate.h"
#include "hashtable_entityid_spritegroup.h"
#include "mprint.h"
#include "textureinfo.h"

hashtable_entityid_spritegroup_t* spritegroups = NULL;
textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

gamestate* g = NULL;

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 480


void libdraw_init(const gamestate* const g) {
    if (g) {
        const char* title = "evildojo666";
        const int window_width = DEFAULT_WINDOW_WIDTH;
        const int window_height = DEFAULT_WINDOW_HEIGHT;
        InitWindow(window_width, window_height, title);
        SetTargetFPS(60);
        libdraw_load_textures();
        msuccess("libdraw_init");
    } else {
        merror("libdraw_init g is NULL");
    }
}


void libdraw_update_input(inputstate* const is) {
    inputstate_update(is);
}


void libdraw_drawframe(const gamestate* const g) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
}


void libdraw_close() {
    g = NULL;
    libdraw_unload_textures();
    CloseWindow();
}


bool libdraw_windowshouldclose() {
    return WindowShouldClose();
}


void libdraw_load_texture(
    const int txkey, const int contexts, const int frames, const bool do_dither, const char* path) {
    if (txkey < 0 || txkey >= GAMESTATE_SIZEOFTEXINFOARRAY) {
        merror("libdraw_load_texture: txkey out of bounds");
    } else if (contexts < 0) {
        merror("libdraw_load_texture: contexts out of bounds");
    } else if (frames < 0) {
        merror("libdraw_load_texture: frames out of bounds");
    } else if (txinfo[txkey].texture.id > 0) {
        merror("libdraw_load_texture: texture already loaded");
    } else if (strlen(path) == 0) {
        merror("libdraw_load_texture: path is empty");
    } else if (strcmp(path, "\n") == 0) {
        merror("libdraw_load_texture: path is newline");
    } else {
        Image image = LoadImage(path);
        if (do_dither) {
            ImageDither(&image, 8, 8, 8, 8);
        }
        Texture2D texture = LoadTextureFromImage(image);
        UnloadImage(image);
        txinfo[txkey].texture = texture;
        txinfo[txkey].contexts = contexts;
        msuccess("libdraw_load_texture: texture loaded successfully");
    }
}


void libdraw_unload_texture(const int txkey) {
    if (txkey < 0 || txkey >= GAMESTATE_SIZEOFTEXINFOARRAY) {
        merror("libdraw_unload_texture: txkey out of bounds");
    } else {
        UnloadTexture(txinfo[txkey].texture);
        txinfo[txkey].texture = (Texture2D){0};
        txinfo[txkey].contexts = 0;
        msuccess("libdraw_unload_texture: texture unloaded successfully");
    }
}


void libdraw_load_textures() {
    const char* textures_file = "textures.txt";
    FILE* file = fopen(textures_file, "r");
    if (file) {
        char line[1024] = {0};
        while (fgets(line, sizeof(line), file)) {
            int txkey = -1;
            int contexts = -1;
            int frames = -1;
            int do_dither = 0;
            char path[512] = {0};
            // check if the line begins with a #
            if (line[0] == '#') {
                minfo("libdraw_load_textures: skipping comment line");
                continue;
            }

            sscanf(line, "%d %d %d %d %s", &txkey, &contexts, &frames, &do_dither, path);
            if (txkey < 0 || contexts < 0 || frames < 0) {
                merror("libdraw_load_textures: invalid line in textures.txt");
                continue;
            }
            libdraw_load_texture(txkey, contexts, frames, do_dither, path);
        }
        fclose(file);
    } else {
        merror("libdraw_load_textures: could not open textures.txt");
    }
}


void libdraw_unload_textures() {
    for (int i = 0; i < GAMESTATE_SIZEOFTEXINFOARRAY; i++)
        libdraw_unload_texture(i);
}
