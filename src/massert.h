#pragma once

#ifdef DEBUG
#define massert(cond, msg)                                                                                             \
    do {                                                                                                               \
        if (!(cond)) {                                                                                                 \
            printf("☠️  \033[31;1mASSERT \033[0m %s:%d: %s\n", __FILE__, __LINE__, msg);                                \
            exit(EXIT_FAILURE);                                                                                        \
        }                                                                                                              \
    } while (0)
#else
#define massert(cond, msg) ((void)0)
#endif
