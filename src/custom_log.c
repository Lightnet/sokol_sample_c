/* -------------------------------------------------------------
   custom_log.c
   -------------------------------------------------------------
   Implementation of the logging helper declared in custom_log.h
   ------------------------------------------------------------- */

#include "custom_log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void _custom_log(const char* tag,
                 uint32_t    level,
                 const char* fmt,
                 uint32_t    line,
                 const char* file,
                 ...)
{
    char buffer[1024];

    va_list ap;
    va_start(ap, file);                     // start after 'file'
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    /* Forward to sokol's logger – the last argument is reserved for future use */
    slog_func(tag, level, 0, buffer, line, file, NULL);
}