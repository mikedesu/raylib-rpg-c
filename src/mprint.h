#pragma once
#include <stdio.h>

#ifdef DEBUG
#define merror(s) (printf("☠️  \033[31;1mError  \033[0m %s:%d: %s\n", __FILE__, __LINE__, s))
#define merrorint(s, x) (printf("☠️  \033[31;1mError  \033[0m %s:%d: %s: %d\n", __FILE__, __LINE__, s, x))
#define merrorint2(s, x, y) (printf("☠️  \033[31;1mError  \033[0m %s:%d: %s: %d %d\n", __FILE__, __LINE__, s, x, y))
#define merrorint4(s, x, y, z, a)                                                                                      \
    (printf("☠️  \033[31;1mError  \033[0m %s:%d: %s: %d %d %d %d\n", __FILE__, __LINE__, s, x, y, z, a))

#define mwarning(s) (printf("⚠️  \033[31;1mWarning\033[0m %s:%d: %s\n", __FILE__, __LINE__, s))
#define mwaringint(s, x) (printf("⚠️  \033[31;1mWarning\033[0m %s:%d: %s: %d\n", __FILE__, __LINE__, s, x))
#define mwarningint2(s, x, y) (printf("⚠️  \033[31;1mWarning\033[0m %s:%d: %s: %d %d\n", __FILE__, __LINE__, s, x, y))
#define mwarningint4(s, x, y, z, a)                                                                                    \
    (printf("⚠️  \033[31;1mWarning\033[0m %s:%d: %s: %d %d %d %d\n", __FILE__, __LINE__, s, x, y, z, a))

#define minfo(s) (printf("📔 \033[35;1mInfo   \033[0m %s:%d: %s\n", __FILE__, __LINE__, s))
#define minfoint(s, x) (printf("📔 \033[35;1mInfo   \033[0m %s:%d: %s: %d\n", __FILE__, __LINE__, s, x))
#define minfoint2(s, x, y) (printf("📔 \033[35;1mInfo   \033[0m %s:%d: %s: %d %d\n", __FILE__, __LINE__, s, x, y))
#define minfoint4(s, x, y, z, a)                                                                                       \
    (printf("📔 \033[35;1mInfo   \033[0m %s:%d: %s: %d %d %d %d\n", __FILE__, __LINE__, s, x, y, z, a))

#define msuccess(s) (printf("✅ \033[32;1mOK     \033[0m %s:%d: %s\n", __FILE__, __LINE__, s))
#define msuccessint(s, x) (printf("✅ \033[32;1mOK     \033[0m %s:%d: %s: %d\n", __FILE__, __LINE__, s, x))
#define msuccessint2(s, x, y) (printf("✅ \033[32;1mOK     \033[0m %s:%d: %s: %d %d\n", __FILE__, __LINE__, s, x, y))
#define msuccessint4(s, x, y, z, a)                                                                                    \
    (printf("✅ \033[32;1mOK     \033[0m %s:%d: %s: %d %d %d %d\n", __FILE__, __LINE__, s, x, y, z, a))
#else
#define merror(s)
#define mwarning(s)
#define minfo(s)
#define msuccess(s)
#define merrorint(s, x)
#define mwarningint(s, x)
#define minfoint(s, x)
#define msuccessint(s, x)
#define merrorint2(s, x, y)
#define mwarningint2(s, x, y)
#define minfoint2(s, x, y)
#define msuccessint2(s, x, y)
#define merrorint4(s, x, y, z, a)
#define mwarningint4(s, x, y, z, a)
#define minfoint4(s, x, y, z, a)
#define msuccessint4(s, x, y, z, a)
#endif
