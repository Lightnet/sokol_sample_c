//------------------------------------------------------------------------------
//  1-4-2-quad (indexed version – visible on both sides)
//------------------------------------------------------------------------------
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "triangle.glsl.h"   // unchanged

static struct {
    sg_pipeline pip;
    sg_bindings bind;          // now holds vertex + index buffer
    sg_pass_action pass_action;
} state;

static void init(void) {
    sg_setup(&(sg_desc){
        .environment = sglue_environment(),
    });

    sg_shader shd = sg_make_shader(simple_shader_desc(sg_query_backend()));

    /* 4 unique vertices */
    const float vertices[] = {
        -0.5f, -0.5f, 0.0f,   // 0 : bottom-left
         0.5f, -0.5f, 0.0f,   // 1 : bottom-right
         0.5f,  0.5f, 0.0f,   // 2 : top-right
        -0.5f,  0.5f, 0.0f    // 3 : top-left
    };

    /* 6 indices */
    const uint16_t indices[] = {
        0, 1, 2,   // first triangle (CCW)
        0, 2, 3    // second triangle (CCW)
    };

    /* Vertex buffer: immutable with initial data */
    state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
        .usage = {
            .vertex_buffer = true,  // explicit for clarity (default anyway)
            .immutable = true       // default
        },
        .data = SG_RANGE(vertices),  // implies .size = 0
        .label = "quad-vertices"
    });

    /* Index buffer: immutable with initial data */
    state.bind.index_buffer = sg_make_buffer(&(sg_buffer_desc){
        .usage = {
            .index_buffer = true,   // key flag
            .immutable = true
        },
        .data = SG_RANGE(indices),
        .label = "quad-indices"
    });

    /* Pipeline: declare index type for draw */
    state.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,
        .layout = {
            .attrs = {
                [ATTR_simple_position].format = SG_VERTEXFORMAT_FLOAT3
            }
        },
        .index_type = SG_INDEXTYPE_UINT16,  // for uint16_t indices
        .label = "quad-pipeline"
    });

    state.pass_action = (sg_pass_action){ 0 };  // or your clear color
}

void frame(void) {
    sg_begin_pass(&(sg_pass){
        .action = state.pass_action,
        .swapchain = sglue_swapchain()
    });
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);

    /* 6 indices → 2 triangles */
    sg_draw(0, 6, 1);          // start vertex is ignored for indexed draws

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
        .window_title = "Quad (indexed) visible"
    };
}