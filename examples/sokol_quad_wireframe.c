//------------------------------------------------------------------------------
//  1-4-3-quad-wireframe
//------------------------------------------------------------------------------
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"
#include "quad-wireframe.glsl.h"   // shader unchanged

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


typedef struct {
    sg_pipeline pip_solid;
    sg_pipeline pip_wire;
    sg_bindings bind;
    sg_pass_action pass_action;
    bool wireframe;
} app_state_t;

static app_state_t state = {0};

static sg_pipeline make_pipeline(sg_shader shd, bool wireframe) {
    return sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,
        .layout = {
            .attrs = {
                [ATTR_simple_position].format = SG_VERTEXFORMAT_FLOAT3
            }
        },
        // .primitive_type = SG_PRIMITIVETYPE_TRIANGLES, // fill
        .primitive_type = SG_PRIMITIVETYPE_LINES, // wireframe
        // .primitive_type = SG_PRIMITIVETYPE_LINE_STRIP, // wireframe
        // .primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP, // fill
        // .polygon_mode = wireframe ? SG_POLYGONMODE_LINE : SG_POLYGONMODE_FILL,
        .label = wireframe ? "quad-pipeline-wire" : "quad-pipeline-solid"
    });
}

static void init(void) {
    sg_setup(&(sg_desc){
        .environment = sglue_environment(),
    });

    sg_shader shd = sg_make_shader(simple_shader_desc(sg_query_backend()));

    /* 6 vertices – two CCW triangles */
    const float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
        .size = sizeof(vertices),
        .data = SG_RANGE(vertices),
        .label = "quad-vertices"
    });

    /* create both pipelines */
    state.pip_solid = make_pipeline(shd, false);
    state.pip_wire  = make_pipeline(shd, true);

    state.pass_action = (sg_pass_action){
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR,
                       .clear_value = {0.2f, 0.3f, 0.3f, 1.0f} }
    };

    state.wireframe = false;  // start in solid mode
}

void frame(void) {
    sg_begin_pass(&(sg_pass){
        .action = state.pass_action,
        .swapchain = sglue_swapchain()
    });

    sg_apply_pipeline(state.wireframe ? state.pip_wire : state.pip_solid);
    sg_apply_bindings(&state.bind);
    sg_draw(0, 6, 1);
    sg_end_pass();
    sg_commit();
}

void cleanup(void) {
    sg_destroy_pipeline(state.pip_solid);
    sg_destroy_pipeline(state.pip_wire);
    sg_shutdown();
}

void event(const sapp_event* ev) {
    if (ev->type == SAPP_EVENTTYPE_KEY_DOWN) {
        switch (ev->key_code) {
            case SAPP_KEYCODE_ESCAPE:
                sapp_request_quit();
                break;
            case SAPP_KEYCODE_W:
                LOG_INFO("input", "W");
                state.wireframe = !state.wireframe;
                break;
        }
    }
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .width = 800,
        .height = 600,
        .high_dpi = true,
        .window_title = "Quad Wireframe (W = toggle)",

        .logger.func = slog_func,
        .win32_console_utf8 = true,
        //.win32_console_create = true, // this create console terminal. this will not work when ide.
        .win32_console_attach = true, // this for ide for terminal.
    };
}