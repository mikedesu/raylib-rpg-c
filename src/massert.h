#pragma once
#include <stdio.h>
#include <stdlib.h>

// Cross-platform debug break
#if defined(_WIN32)
#include <windows.h>
#define DEBUG_BREAK() DebugBreak()
#elif defined(__linux__) || defined(__APPLE__)
#include <signal.h>
#define DEBUG_BREAK() raise(SIGTRAP)
#else
#define DEBUG_BREAK() ((void)0)
#endif

// Assert macro with printf formatting
#ifdef DEBUG_ASSERT
#define massert(cond, ...) \
    do { \
        if (!(cond)) { \
            printf("☠️  \033[31;1mASSERT\033[0m %s:%d: ", __FILE__, __LINE__); \
            printf(__VA_ARGS__); \
            printf("\n"); \
            DEBUG_BREAK(); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#else
#define massert(cond, ...) ((void)0)
#endif
