#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

    // for (int i = 0; i < argc; i++) {
    //   printf("argv[%d]: %s\n", i, argv[i]);
    // }

    int do_dither = 0;

    if (argc != 3 && argc != 4) {
        printf("Usage: %s <input_file> <output_file> [do_dither]\n", argv[0]);
        return 1;
    }

    const char* infile = argv[1];
    const char* outfile = argv[2];

    if (argc == 4) {
        do_dither = atoi(argv[3]);
    }

    Image image = LoadImage(infile);
    if (image.data == NULL) {
        printf("Failed to load image: %s\n", infile);
        return 1;
    }

    if (do_dither) {
        ImageDither(&image, 4.0f, 4.0f, 4.0f, 4.0f);
    }

    const bool retval = ExportImageAsCode(image, outfile);
    if (!retval) {
        printf("Failed to export image as code: %s\n", outfile);
        return 1;
    }

    UnloadImage(image);
    return 0;
}
