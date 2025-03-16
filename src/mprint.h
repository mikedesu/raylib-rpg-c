#pragma once
#include <stdio.h>

#ifdef DEBUG
#define merror(s) (printf("☠️  \033[31;1mError  \033[0m %s:%d: %s\n", __FILE__, __LINE__, s))
#define merrorint(s, x) (printf("☠️  \033[31;1mError  \033[0m %s:%d: %s: %d\n", __FILE__, __LINE__, s, x))
#define merrorint2(s, x, y) (printf("☠️  \033[31;1mError  \033[0m %s:%d: %s: %d %d\n", __FILE__, __LINE__, s, x, y))
#define merrorint3(s, x, y, z)                                                                                         \
    (printf("☠️  \033[31;1mError  \033[0m %s:%d: %s: %d %d %d\n", __FILE__, __LINE__, s, x, y, z))
#define merrorint4(s, x, y, z, a)                                                                                      \
    (printf("☠️  \033[31;1mError  \033[0m %s:%d: %s: %d %d %d %d\n", __FILE__, __LINE__, s, x, y, z, a))
#define merrorstr(s, x) (printf("☠️  \033[31;1mError  \033[0m %s:%d: %s: %s\n", __FILE__, __LINE__, s, x))


#define mwarning(s) (printf("⚠️  \033[31;1mWarning\033[0m %s:%d: %s\n", __FILE__, __LINE__, s))
#define mwaringint(s, x) (printf("⚠️  \033[31;1mWarning\033[0m %s:%d: %s: %d\n", __FILE__, __LINE__, s, x))
#define mwarningint2(s, x, y) (printf("⚠️  \033[31;1mWarning\033[0m %s:%d: %s: %d %d\n", __FILE__, __LINE__, s, x, y))
#define mwarningint3(s, x, y, z)                                                                                       \
    (printf("⚠️  \033[31;1mWarning\033[0m %s:%d: %s: %d %d %d\n", __FILE__, __LINE__, s, x, y, z))
#define mwarningint4(s, x, y, z, a)                                                                                    \
    (printf("⚠️  \033[31;1mWarning\033[0m %s:%d: %s: %d %d %d %d\n", __FILE__, __LINE__, s, x, y, z, a))
#define mwarningstr(s, x) (printf("⚠️  \033[31;1mWarning\033[0m %s:%d: %s: %s\n", __FILE__, __LINE__, s, x))

#define minfo(s) (printf("📔 \033[35;1mInfo   \033[0m %s:%d: %s\n", __FILE__, __LINE__, s))
#define minfoint(s, x) (printf("📔 \033[35;1mInfo   \033[0m %s:%d: %s: %d\n", __FILE__, __LINE__, s, x))
#define minfoint2(s, x, y) (printf("📔 \033[35;1mInfo   \033[0m %s:%d: %s: %d %d\n", __FILE__, __LINE__, s, x, y))
#define minfoint3(s, x, y, z)                                                                                          \
    (printf("📔 \033[35;1mInfo   \033[0m %s:%d: %s: %d %d %d\n", __FILE__, __LINE__, s, x, y, z))
#define minfoint4(s, x, y, z, a)                                                                                       \
    (printf("📔 \033[35;1mInfo   \033[0m %s:%d: %s: %d %d %d %d\n", __FILE__, __LINE__, s, x, y, z, a))
#define minfostr(s, x) (printf("📔 \033[35;1mInfo   \033[0m %s:%d: %s: %s\n", __FILE__, __LINE__, s, x))

#define msuccess(s) (printf("✅ \033[32;1mOK     \033[0m %s:%d: %s\n", __FILE__, __LINE__, s))
#define msuccessint(s, x) (printf("✅ \033[32;1mOK     \033[0m %s:%d: %s: %d\n", __FILE__, __LINE__, s, x))
#define msuccessint2(s, x, y) (printf("✅ \033[32;1mOK     \033[0m %s:%d: %s: %d %d\n", __FILE__, __LINE__, s, x, y))
#define msuccessint3(s, x, y, z)                                                                                       \
    (printf("✅ \033[32;1mOK     \033[0m %s:%d: %s: %d %d %d\n", __FILE__, __LINE__, s, x, y, z))
#define msuccessint4(s, x, y, z, a)                                                                                    \
    (printf("✅ \033[32;1mOK     \033[0m %s:%d: %s: %d %d %d %d\n", __FILE__, __LINE__, s, x, y, z, a))
#define msuccessstr(s, x) (printf("✅ \033[32;1mOK     \033[0m %s:%d: %s: %s\n", __FILE__, __LINE__, s, x))
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
#define merrorint3(s, x, y, z)
#define mwarningint3(s, x, y, z)
#define minfoint3(s, x, y, z)
#define msuccessint3(s, x, y, z)
#define merrorint4(s, x, y, z, a)
#define mwarningint4(s, x, y, z, a)
#define minfoint4(s, x, y, z, a)
#define msuccessint4(s, x, y, z, a)
#define merrorstr(s, x)
#define mwarningstr(s, x)
#define minfostr(s, x)
#define msuccessstr(s, x)
#endif
