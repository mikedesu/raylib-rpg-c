#pragma once

#ifdef DEBUG
#define massert(cond, ...)                                                                                             \
    do {                                                                                                               \
        if (!(cond)) {                                                                                                 \
            fprintf(stderr, "☠️  \033[31;1mASSERT \033[0m %s:%d: ", __FILE__, __LINE__);                                \
            fprintf(stderr, __VA_ARGS__);                                                                              \
            fprintf(stderr, "\n");                                                                                     \
            exit(EXIT_FAILURE);                                                                                        \
        }                                                                                                              \
    } while (0)
#else
#define massert(cond, ...) ((void)0)
#endif
