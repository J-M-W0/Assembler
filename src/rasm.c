#include "rasm.h"

void debug(char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n\n");
}
void info(char * fmt, ...) {
    time_t now = time(nil);
    if (now == (time_t) -1) {
        abortf("Error: Failed to get the current time!");
    }

    struct tm * localtm = localtime(&now);
    if (!localtm) {
        abortf("Error: Failed to convert current time to local time!");
    }

    char * timestr = asctime(localtm);
    if (!timestr) {
        abortf("Error: Failed to convert the time to a readable format!");
    }

    fprintf(stderr, "\t%s\t", timestr);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n\n");
}
void errorf(const char * filename, const int currline, char * fmt, ...) {
    time_t now = time(nil);
    if (now == (time_t) -1) {
        abortf("Error: Failed to get the current time!");
    }
    struct tm * localtm = localtime(&now);
    if (!localtm) {
        abortf("Error: Failed to convert current time to local time!");
    }
    char * timestr = asctime(localtm);
    if (!timestr) {
        abortf("Error: Failed to convert the time to a readable format!");
    }
    fprintf(stderr, "\t%s\t", timestr);
    fprintf(stderr, "\e[3m%s:%d\e[0m: ", filename, currline);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(EXIT_FAILURE);
}

