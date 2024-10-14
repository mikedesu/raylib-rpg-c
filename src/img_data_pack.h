#pragma once

typedef struct img_data_pack_t {
    int width;
    int height;
    int format;
    int index;
    int contexts;
    int frames;
    unsigned char* data;
} img_data_pack_t;
