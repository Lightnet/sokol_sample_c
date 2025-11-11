// sokol input

#define SOKOL_IMPL
#define SOKOL_GLCORE

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"

#include <stdio.h>
#include <stdarg.h>

/* -------------------------------------------------------------
   Logging helper – safe name, variadic, passes line/file
   ------------------------------------------------------------- */
static void slogf(const char* tag,
                     uint32_t level,
                     const char* fmt,
                     uint32_t line,
                     const char* file,
                     ...)               /* variadic starts here */
{
    char buf[1024];
    va_list ap;
    va_start(ap, file);                    // start after 'file'
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    slog_func(tag, level, 0, buf, line, file, NULL);
}

/* -----------------------------------------------------------------
   Helper macros – note the __LINE__/__FILE__ are now part of the macro
   ----------------------------------------------------------------- */
#define LOG_INFO(tag, fmt, ...) \
    slogf(tag, 3, fmt, __LINE__, __FILE__, ##__VA_ARGS__)

#define LOG_WARN(tag, fmt, ...) \
    slogf(tag, 2, fmt, __LINE__, __FILE__, ##__VA_ARGS__)

#define LOG_ERROR(tag, fmt, ...) \
    slogf(tag, 1, fmt, __LINE__, __FILE__, ##__VA_ARGS__)

static void init(void) {
    printf("init...\n");
}
static void frame(void) {}
static void cleanup(void) {}

void event(const sapp_event* ev) {
    if (ev->type == SAPP_EVENTTYPE_KEY_DOWN) {
        switch (ev->key_code) {
            case SAPP_KEYCODE_ESCAPE:
                sapp_request_quit();
                break;
            case SAPP_KEYCODE_W:
                LOG_INFO("input", "W");
                break;
            case SAPP_KEYCODE_A:
                LOG_INFO("input", "A");
                break;
            case SAPP_KEYCODE_S:
                LOG_INFO("input", "S");
                break;
            case SAPP_KEYCODE_D:
                LOG_INFO("input", "D");
                break;
        }
    }
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    printf("main...\n"); // does not work here.

    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .window_title = "Sokol Input Example",
        .width = 800,
        .height = 600,
        .icon.sokol_default = true,

        .logger.func = slog_func,
        .win32_console_utf8 = true,
        // .win32_console_create = true,
        .win32_console_attach = true,
    };
}
