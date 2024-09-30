//#include "mprint.h"
#include <stdarg.h>
#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <time.h>



void mprint(const char* format, ...) {
    va_list args;
    char buffer[1024];
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    fprintf(stdout, "%s:%d: %s\n", __FILE__, __LINE__, buffer);
}


void merror(const char* format, ...) {
    va_list args;
    char buffer[1024];
    //char datetimebuf[128];
    //time_t now = time(NULL);
    //struct tm* tm = localtime(&now);
    //strftime(datetimebuf, sizeof(datetimebuf), "%Y-%m-%d %H:%M:%S", tm);
    //if (g) {
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    fprintf(stderr, "☠️ \033[31;1mError\033[0m %s:%d: %s\n", __FILE__, __LINE__, buffer);
    //}
}




void minfo(const char* format, ...) {
    va_list args;

    char buffer[1024];
    //char datetimebuf[128];

    //time_t now = time(NULL);
    //struct tm* tm = localtime(&now);
    //strftime(datetimebuf, sizeof(datetimebuf), "%Y-%m-%d %H:%M:%S", tm);


    //if (g) {

    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    fprintf(stdout, "📔 \033[34;1mInfo\033[0m %s:%d: %s\n", __FILE__, __LINE__, buffer);
    //}
}
