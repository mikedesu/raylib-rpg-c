#pragma once
#include <stdio.h>
#define merror(s) (printf("☠️  \033[31;1mError  \033[0m %s:%d: %s\n", __FILE__, __LINE__, s))
#define mwarning(s) (printf("⚠️  \033[31;1mWarning\033[0m %s:%d: %s\n", __FILE__, __LINE__, s))
#define minfo(s) (printf("📔 \033[35;1mInfo   \033[0m %s:%d: %s\n", __FILE__, __LINE__, s))
#define msuccess(s) (printf("✅ \033[32;1mOK     \033[0m %s:%d: %s\n", __FILE__, __LINE__, s))
