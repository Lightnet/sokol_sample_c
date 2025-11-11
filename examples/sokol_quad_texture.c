//------------------------------------------------------------------------------
//  1-4-2-quad-textured (indexed version – visible on both sides)
//------------------------------------------------------------------------------
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"
#include "quad_textured.glsl.h"  // Ensure binding=0 in shader
#include <stdio.h>

#define SLOT_tex 0

static struct {
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
    sg_image tex;
    sg_sampler smp;
    sg_view tex_view;
} state;

static void init(void) {
    sg_setup(&(sg_desc){
        .environment = sglue_environment(),
        .logger.func = slog_func,  // Enable Sokol logging for errors
    });

    sg_shader shd = sg_make_shader(quad_textured_shader_desc(sg_query_backend()));

    /* Vertices: pos3 + uv2 (matches texcube-sapp.c interleaved style) */
    const float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 0.0f
    };

    const uint16_t indices[] = { 0, 1, 2, 0, 2, 3 };

    state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
        .usage = { .vertex_buffer = true, .immutable = true },
        .data = SG_RANGE(vertices),
        .label = "quad-vertices"
    });

    state.bind.index_buffer = sg_make_buffer(&(sg_buffer_desc){
        .usage = { .index_buffer = true, .immutable = true },
        .data = SG_RANGE(indices),
        .label = "quad-indices"
    });

    /* Load PNG (matches texcube-sapp.c pattern: load → validate → image) */
    int width, height, num_comps;
    stbi_uc* pixels = stbi_load("grass16x16.png", &width, &height, &num_comps, 4);
    if (!pixels || width <= 0 || height <= 0) {
        fprintf(stderr, "Load failed: %s (dims: %dx%d)\n",
                pixels ? "invalid size" : stbi_failure_reason(), width, height);
        if (pixels) stbi_image_free(pixels);

        /* Fallback (like texcube-sapp.c static data, but 1x1 white) */
        uint8_t fallback[4] = { 255, 255, 255, 255 };
        state.tex = sg_make_image(&(sg_image_desc){
            .type = SG_IMAGETYPE_2D,
            .width = 1, .height = 1,
            .pixel_format = SG_PIXELFORMAT_RGBA8,
            .usage = { .immutable = true },
            .data = { .mip_levels[0] = SG_RANGE(fallback) },  // Braced init (key fix)
            .label = "fallback-tex"
        });
    } else {
        /* Validate pitch (Sokol expects row-aligned RGBA8: width * 4 bytes/row) */
        int expected_size = width * height * 4;
        printf("Loaded: %dx%d, comps=%d, size=%d bytes\n", width, height, num_comps, expected_size);

        state.tex = sg_make_image(&(sg_image_desc){
            .type = SG_IMAGETYPE_2D,
            .width = width, .height = height,
            .pixel_format = SG_PIXELFORMAT_RGBA8,
            .usage = { .immutable = true },
            .data = { .mip_levels[0] = SG_RANGE(pixels) },  // Matches texcube-sapp.c exactly
            .label = "grass-tex"
        });
        stbi_image_free(pixels);
    }

    /* Validate image (texcube-sapp.c assumes success; we check) */
    sg_resource_state img_state = sg_query_image_state(state.tex);
    printf("Image state: %d (VALID=%d)\n", img_state, SG_RESOURCESTATE_VALID);
    if (img_state != SG_RESOURCESTATE_VALID) {
        fprintf(stderr, "sg_make_image failed!\n");
        return;  // Abort to prevent crash
    }

    /* Sampler (matches texcube-sapp.c: linear for cube, but nearest for pixel art) */
    state.smp = sg_make_sampler(&(sg_sampler_desc){
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .wrap_u = SG_WRAP_REPEAT, .wrap_v = SG_WRAP_REPEAT,
        .label = "grass-smp"
    });

    /* View (exact match to texcube-sapp.c) */
    state.tex_view = sg_make_view(&(sg_view_desc){
        .texture = { .image = state.tex },  // Full mip/slice
        .label = "grass-view"
    });

    sg_resource_state view_state = sg_query_view_state(state.tex_view);
    printf("View state: %d (VALID=%d)\n", view_state, SG_RESOURCESTATE_VALID);
    if (view_state != SG_RESOURCESTATE_VALID) {
        fprintf(stderr, "sg_make_view failed!\n");
        return;
    }

    /* Bind (matches texcube-sapp.c: slot 0) */
    state.bind.views[SLOT_tex] = state.tex_view;
    state.bind.samplers[SLOT_tex] = state.smp;

    /* Pipeline (vertex layout matches interleaved data) */
    state.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,
        .index_type = SG_INDEXTYPE_UINT16,
        .layout = {
            .attrs = {
                [0] = { .format = SG_VERTEXFORMAT_FLOAT3, .offset = 0 },
                [1] = { .format = SG_VERTEXFORMAT_FLOAT2, .offset = 3 * sizeof(float) }
            },
            .buffers[0] = { .stride = 5 * sizeof(float) }
        },
        .label = "quad-pip"
    });

    /* Pass action (clear to green for contrast) */
    state.pass_action = (sg_pass_action){
        .colors[0] = { .store_action = SG_LOADACTION_CLEAR, .clear_value = { 0.1f, 0.3f, 0.1f, 1.0f } }
    };
}

void frame(void) {
    sg_begin_pass(&(sg_pass){ .action = state.pass_action, .swapchain = sglue_swapchain() });
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);
    sg_draw(0, 6, 1);
    sg_end_pass();
    sg_commit();
}

void cleanup(void) { sg_shutdown(); }

void event(const sapp_event* ev) {
    if (ev->type == SAPP_EVENTTYPE_KEY_DOWN && ev->key_code == SAPP_KEYCODE_ESCAPE) {
        sapp_request_quit();
    }
}

sapp_desc sokol_main(int argc, char* argv[]) {
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .width = 800, .height = 600, .high_dpi = true,
        .window_title = "Textured Quad (grass PNG)",
        .logger.func = slog_func,
        .win32_console_attach = true,
    };
}