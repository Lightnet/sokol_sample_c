/* -------------------------------------------------------------
   custom_log.h
   -------------------------------------------------------------
   A tiny, variadic logging wrapper around sokol_log.
   Include this header wherever you want LOG_INFO/WARN/ERROR.
   ------------------------------------------------------------- */

#ifndef CUSTOM_LOG_H
#define CUSTOM_LOG_H

#include "sokol_log.h"
#include <stdint.h>

/* -----------------------------------------------------------------
   Public API – just include the header and use the macros
   ----------------------------------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* Internal helper – do NOT call directly */
void _custom_log(const char* tag,
                 uint32_t    level,      // 3=info, 2=warn, 1=error
                 const char* fmt,
                 uint32_t    line,
                 const char* file,
                 ...);

/* -----------------------------------------------------------------
   Macros – they automatically capture __LINE__ / __FILE__
   ----------------------------------------------------------------- */
#define LOG_INFO(tag, fmt, ...) \
    _custom_log(tag, 3, fmt, __LINE__, __FILE__, ##__VA_ARGS__)

#define LOG_WARN(tag, fmt, ...) \
    _custom_log(tag, 2, fmt, __LINE__, __FILE__, ##__VA_ARGS__)

#define LOG_ERROR(tag, fmt, ...) \
    _custom_log(tag, 1, fmt, __LINE__, __FILE__, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* CUSTOM_LOG_H */