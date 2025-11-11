//------------------------------------------------------------------------------
//  1-4-2-quad (visible on both sides)
//------------------------------------------------------------------------------
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "triangle.glsl.h"   // shader unchanged

static struct {
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
} state;

static void init(void) {
    sg_setup(&(sg_desc){
        .environment = sglue_environment(),
    });

    sg_shader shd = sg_make_shader(simple_shader_desc(sg_query_backend()));

    /* 6 vertices – two CCW triangles that form a quad */
    const float vertices[] = {
        // 1st triangle (bottom-left → bottom-right → top-right)
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,

        // 2nd triangle (bottom-left → top-right → top-left)
        -0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
        .size = sizeof(vertices),
        .data = SG_RANGE(vertices),
        .label = "quad-vertices"
    });

    state.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,
        .layout = {
            .attrs = {
                [ATTR_simple_position].format = SG_VERTEXFORMAT_FLOAT3
            }
        },
        .label = "quad-pipeline"
    });

    state.pass_action = (sg_pass_action){
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR,
                       .clear_value = {0.2f, 0.3f, 0.3f, 1.0f} }
    };
}

void frame(void) {
    sg_begin_pass(&(sg_pass){
        .action = state.pass_action,
        .swapchain = sglue_swapchain()
    });
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);
    sg_draw(0, 6, 1);          // draw 6 vertices → 2 triangles
    sg_end_pass();
    sg_commit();
}

void cleanup(void) { sg_shutdown(); }

void event(const sapp_event* ev) {
    if (ev->type == SAPP_EVENTTYPE_KEY_DOWN &&
        ev->key_code == SAPP_KEYCODE_ESCAPE) {
        sapp_request_quit();
    }
}

sapp_desc sokol_main(int argc, char* argv[]) {
    return (sapp_desc){
        .init_cb    = init,
        .frame_cb   = frame,
        .cleanup_cb = cleanup,
        .event_cb   = event,
        .width      = 800,
        .height     = 600,
        .high_dpi   = true,
        .window_title = "Quad visible"
    };
}