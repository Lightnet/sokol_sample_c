// #define SOKOL_LOG_IMPL
#define SOKOL_IMPL
#define SOKOL_GLCORE

#include "sokol_app.h"
#include "sokol_log.h"

static void init(void) {
    printf("init...\n");
}
static void frame(void) {}
static void cleanup(void) {}
static void event(const sapp_event* ev) {}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    printf("main...\n");

    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .window_title = "Sokol Window Example",
        .width = 800,
        .height = 600,
        .icon.sokol_default = true,

        .logger.func = slog_func,
        .win32_console_utf8 = true,
        .win32_console_create = true,
    };
}