#pragma once

#ifdef DEBUG
#define massert(cond, ...)                                                                                             \
    do {                                                                                                               \
        if (!(cond)) {                                                                                                 \
            printf("☠️  \033[31;1mASSERT \033[0m %s:%d: ", __FILE__, __LINE__);                                         \
            printf(__VA_ARGS__);                                                                                       \
            printf("\n");                                                                                              \
            exit(EXIT_FAILURE);                                                                                        \
        }                                                                                                              \
    } while (0)
#else
#define massert(cond, ...) ((void)0)
#endif
