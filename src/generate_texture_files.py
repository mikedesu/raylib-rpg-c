import os


def main():
    for i, j, k in os.walk("img"):
        subpath = i[4:]
        for file in k:
            path = f"{subpath}/{file}"
            macro = path.replace("/", "_")
            macro = macro[:-4]
            macro = macro.upper()
            print(f"{macro}")


if __name__ == "__main__":
    main()
