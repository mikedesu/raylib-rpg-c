#include "utils.h"

char* shufflestr(const char* str) {
    char* s = strdup(str);
    size_t len = strlen(s);
    for (size_t i = 0; i < len - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (len - i) + 1);
        char c = s[j];
        s[j] = s[i];
        s[i] = c;
    }
    return s;
}


void shufflestrinplace(char* str) {
    // for now lets try swapping 2 random characters
    int i = rand() % strlen(str);
    int j = rand() % strlen(str);
    while (i == j) {
        j = rand() % strlen(str);
    }

    char c = str[i];
    str[i] = str[j];
    str[j] = c;
}
