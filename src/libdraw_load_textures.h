#pragma once

#include "textureinfo.h"

bool load_texture(textureinfo* txinfo, int txkey, int ctxs, int frames, bool do_dither, char* path);
bool load_textures(textureinfo* txinfo);
