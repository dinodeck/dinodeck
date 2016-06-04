#include "../../DDLog.h"
#include <android/log.h>
#include <stdarg.h>
#include <stdio.h>

void dsprintf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_VERBOSE, "DSNDK", fmt, args);
    va_end(args);
}
