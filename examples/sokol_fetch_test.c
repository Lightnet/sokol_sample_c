/* -------------------------------------------------------------
   sokol_file_load.c
   ------------------------------------------------------------- */
#define SOKOL_IMPL
#define SOKOL_GLCORE          // or SOKOL_GLES3 / SOKOL_D3D11 etc.
#define SOKOL_VALIDATE_NON_FATAL

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_fetch.h"
#include "sokol_log.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

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

/* -------------------------------------------------------------
   Application state
   ------------------------------------------------------------- */
#define MAX_FILE_SIZE (1024*1024)
static uint8_t file_buffer[MAX_FILE_SIZE];

typedef struct {
    sg_pass_action pass_action;
} app_state_t;

static app_state_t state;

/* -------------------------------------------------------------
   sfetch response callback – called on the main thread
   ------------------------------------------------------------- */
static void file_loaded(const sfetch_response_t* r)
{
    if (r->failed) {
        LOG_ERROR("sfetch", "Failed to load '%s' (error %d)", r->path, (int)r->error_code);
        /* optional: change clear colour to red */
        state.pass_action = (sg_pass_action){
            .colors[0] = { .load_action = SG_LOADACTION_CLEAR,
                           .clear_value = { 1.0f, 0.0f, 0.0f, 1.0f }}
        };
        return;
    }

    if (r->fetched) {
        const uint8_t* data = (const uint8_t*)r->data.ptr;
        const size_t   size = (size_t)r->data.size;

        /* null-terminate for printf / LOG_* */
        char* txt = (char*)malloc(size + 1);
        if (txt) {
            memcpy(txt, data, size);
            txt[size] = '\0';

            printf("=== File '%s' (%zu bytes) ===\n%s\n", r->path, size, txt);
            LOG_INFO("sfetch", "Loaded %zu bytes from '%s'", size, r->path);

            free(txt);
        }
    }
}

/* -------------------------------------------------------------
   sokol-app callbacks
   ------------------------------------------------------------- */
static void init(void)
{
    LOG_INFO("app", "init");

    sg_setup(&(sg_desc){
        .environment = sglue_environment(),
        .logger.func = slog_func,
    });

    sfetch_setup(&(sfetch_desc_t){
        .max_requests = 4,
        .num_channels = 2,
        .num_lanes    = 2,
        .logger.func  = slog_func,
    });

    /* start loading the file */
    sfetch_send(&(sfetch_request_t){
        .path     = "resources/hello_world.txt",
        .callback = file_loaded,
        .buffer   = SFETCH_RANGE(file_buffer),
    });

    /* default clear colour */
    state.pass_action = (sg_pass_action){
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR,
                       .clear_value = { 0.1f, 0.1f, 0.1f, 1.0f }}
    };
}

static void frame(void)
{
    /* pump sfetch – must be called **every** frame */
    sfetch_dowork();

    /* simple render – just clear */
    sg_begin_pass(&(sg_pass){
        .action = state.pass_action,
        .swapchain = sglue_swapchain()
    });
    sg_end_pass();
    sg_commit();
}

static void cleanup(void)
{
    LOG_INFO("app", "cleanup");
    sfetch_shutdown();
    sg_shutdown();
}

/* -------------------------------------------------------------
   entry point
   ------------------------------------------------------------- */
sapp_desc sokol_main(int argc, char* argv[])
{
    (void)argc; (void)argv;

    /* This printf works **only** when a console is attached.
       On Windows the two flags below make it appear in the IDE. */
    printf("sokol_main() – starting application\n");

    return (sapp_desc){
        .init_cb      = init,
        .frame_cb     = frame,
        .cleanup_cb   = cleanup,
        .width        = 800,
        .height       = 600,
        .sample_count = 4,
        .window_title = "Sokol File-Load Demo",
        .icon.sokol_default = true,
        .logger.func  = slog_func,

        /* console on Windows */
        .win32_console_utf8   = true,
        .win32_console_attach = true,
    };
}