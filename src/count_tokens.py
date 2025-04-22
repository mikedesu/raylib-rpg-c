# count_tokens.py

import os
import tiktoken


def main():
    # lets look at all the .c files and .h files in the current directory

    total_count = 0
    for filename in os.listdir():
        if filename.endswith(".c") or filename.endswith(".h"):
            with open(filename, "r") as f:
                content = f.read()
                encoding = tiktoken.encoding_for_model("gpt-3.5-turbo")
                tokens = len(encoding.encode(content))
                total_count += tokens
                # print(f"{filename}: {tokens} tokens")

    # print(f"Total tokens: {total_count} tokens")
    print(f"{total_count}")


if __name__ == "__main__":
    main()
