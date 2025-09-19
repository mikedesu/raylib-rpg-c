#pragma once
#include <cstdio>
// Single-line macros that clang-format won't break
#ifdef DEBUG
#define minfo(...)    do { printf("📔 \033[35;1mInfo   \033[0m %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } while (0)
#define mwarning(...) do { printf("⚠️  \033[33;1mWarning\033[0m %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } while (0)
#define merror(...)   do { printf("☠️  \033[31;1mError  \033[0m %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } while (0)
#define msuccess(...) do { printf("✅ \033[32;1mOK     \033[0m %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } while (0)
#else
#define minfo(...)    ((void)0)
#define mwarning(...) ((void)0) 
#define merror(...)   ((void)0)
#define msuccess(...) ((void)0)
#endif

#ifdef DEBUG2
#define minfo2(...)    do { printf("📔 \033[35;1mInfo   \033[0m %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } while (0)
#define mwarning2(...) do { printf("⚠️  \033[33;1mWarning\033[0m %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } while (0)
#define merror2(...)   do { printf("☠️  \033[31;1mError  \033[0m %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } while (0)
#define msuccess2(...) do { printf("✅ \033[32;1mOK     \033[0m %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } while (0)
#else
#define minfo2(...)    ((void)0)
#define mwarning2(...) ((void)0) 
#define merror2(...)   ((void)0)
#define msuccess2(...) ((void)0)
#endif
