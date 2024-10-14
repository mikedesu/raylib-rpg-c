#pragma once
#include <stdio.h>
#define mprint(s) (printf("%s:%d: %s\n", __FILE__, __LINE__, s))
#define merror(s) (fprintf(stderr, "☠️ \033[31;1mError\033[0m %s:%d: %s\n", __FILE__, __LINE__, s))
#define minfo(s) (fprintf(stderr, "📔 \033[35;1mInfo\033[0m %s:%d: %s\n", __FILE__, __LINE__, s))
