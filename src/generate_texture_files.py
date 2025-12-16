import os
from PIL import Image


def main():

    outfile_images_path = "textures.txt"
    outfile_header_path = "texture_ids.h"

    outfile_images = open(outfile_images_path, "w")
    outfile_header = open(outfile_header_path, "w")

    outfile_header.write("#pragma once\n")

    filecount = 0
    for i, _, k in os.walk("img"):
        subpath = i[4:]
        for file in k:
            path = f"{subpath}/{file}"
            fullpath = f"img/{path}"
            macro = path.replace("/", "_")
            macro = macro[:-4]
            macro = macro.upper()
            macro = f"TX_{macro}"

            # print(f"#define {macro} {filecount}")
            outfile_header.write(f"#define {macro} {filecount}\n")

            img = Image.open(fullpath)
            w = img.size[0]
            h = img.size[1]

            num_contexts = h // 32
            frame_count = w // 32

            # print(f"{filecount} {num_contexts} {frame_count} 0 {fullpath}")
            outfile_images.write(
                f"{filecount} {num_contexts} {frame_count} 0 {fullpath}\n"
            )

            filecount += 1

    outfile_images.close()
    outfile_header.close()


if __name__ == "__main__":
    main()
