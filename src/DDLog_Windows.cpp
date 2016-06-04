#include "DDLog.h"
#include <stdio.h>
#include <stdarg.h>

void dsprintf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
