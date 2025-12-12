#pragma once

#include "massert.h"
#include "mprint.h"
#include "textureinfo.h"


static inline bool load_texture(textureinfo* txinfo, int txkey, int ctxs, int frames, bool do_dither, char* path) {
    massert(txinfo != NULL, "txinfo is NULL");
    massert(path, "path is NULL");
    massert(txkey >= 0, "txkey is invalid");
    massert(ctxs >= 0, "contexts is invalid");
    massert(frames >= 0, "frames is invalid");
    massert(txkey < GAMESTATE_SIZEOFTEXINFOARRAY, "txkey is out of bounds: %d", txkey);
    massert(txinfo[txkey].texture.id == 0, "txinfo[%d].texture.id is not 0", txkey);
    massert(strlen(path) > 0, "load_texture: path is empty");
    massert(strcmp(path, "\n") != 0, "load_texture: path is newline");
    Image image = LoadImage(path);
    // crash if there is a problem loading the image
    massert(image.data != NULL, "load_texture: image data is NULL for path: %s", path);
    if (do_dither)
        ImageDither(&image, 4, 4, 4, 4);
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    txinfo[txkey].texture = texture;
    txinfo[txkey].contexts = ctxs;
    txinfo[txkey].num_frames = frames;
    return true;
}


static inline bool load_textures(textureinfo* txinfo) {
    massert(txinfo != NULL, "txinfo is NULL");
    const char* textures_file = "textures.txt";
    FILE* file = fopen(textures_file, "r");
    massert(file, "textures.txt file is NULL");
    if (!file) {
        merror("Failed to open file");
        return false;
    }
    char line[1024] = {0};
    while (fgets(line, sizeof(line), file)) {
        int txkey = -1;
        int contexts = -1;
        int frames = -1;
        int do_dither = 0;
        char path[512] = {0};
        // check if the line begins with a #
        if (line[0] == '#')
            continue;
        sscanf(line, "%d %d %d %d %s", &txkey, &contexts, &frames, &do_dither, path);
        //minfo("Path: %s", path);
        massert(txkey >= 0, "txkey is invalid");
        massert(contexts >= 0, "contexts is invalid");
        massert(frames >= 0, "frames is invalid");
        if (txkey < 0 || contexts < 0 || frames < 0)
            continue;
        bool tx_loaded = load_texture(txinfo, txkey, contexts, frames, do_dither, path);
        if (!tx_loaded) {
            merror("Failed to load %s, hard-crashing!", path);
            exit(-1);
        }
    }
    fclose(file);
    return true;
}
