//------------------------------------------------------------------------------
//  loadpng_sapp02.c
//  Async PNG load + WASD + Shift/Space movement (camera-relative)
//  Fixed for vecmath.h explicit functions and proper key handling
//------------------------------------------------------------------------------

#define SOKOL_IMPL
#define SOKOL_GLCORE
#define VECMATH_GENERICS
#include "vecmath/vecmath.h"
#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_fetch.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "stb_image.h"
#include "dbgui/dbgui.h"
#include "util/fileutil.h"
#include "loadpng_sapp.glsl.h"

/* -------------------------------------------------------------
   Logging helper
   ------------------------------------------------------------- */
static void slogf(const char* tag, uint32_t level, const char* fmt, uint32_t line, const char* file, ...) {
    char buf[1024];
    va_list ap;
    va_start(ap, file);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    slog_func(tag, level, 0, buf, line, file, NULL);
}

#define LOG_INFO(tag, fmt, ...)  slogf(tag, 3, fmt, __LINE__, __FILE__, ##__VA_ARGS__)
#define LOG_WARN(tag, fmt, ...)  slogf(tag, 2, fmt, __LINE__, __FILE__, ##__VA_ARGS__)
#define LOG_ERROR(tag, fmt, ...) slogf(tag, 1, fmt, __LINE__, __FILE__, ##__VA_ARGS__)

/* -------------------------------------------------------------
   Global state
   ------------------------------------------------------------- */
static struct {
    float rx, ry;
    vec3_t cube_pos;
    float move_speed;
    float vertical_speed;
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;
    uint8_t file_buffer[256 * 1024];
} state;

/* -------------------------------------------------------------
   Key state (file scope)
   ------------------------------------------------------------- */
static uint32_t key_state = 0;  // bits: 0=W,1=S,2=A,3=D,4=Space,5=Shift
#define KEY(k) ((key_state >> (k)) & 1)

/* -------------------------------------------------------------
   Vertex definition
   ------------------------------------------------------------- */
typedef struct {
    float x, y, z;
    int16_t u, v;
} vertex_t;

/* -------------------------------------------------------------
   Forward declarations
   ------------------------------------------------------------- */
static vs_params_t compute_vsparams(float rx, float ry, vec3_t cube_pos);
static void fetch_callback(const sfetch_response_t*);

/* -------------------------------------------------------------
   Input handling
   ------------------------------------------------------------- */
static void handle_input(const sapp_event* ev) {
    if (ev->type != SAPP_EVENTTYPE_KEY_DOWN && ev->type != SAPP_EVENTTYPE_KEY_UP)
        return;

    bool down = (ev->type == SAPP_EVENTTYPE_KEY_DOWN);
    int bit = -1;

    switch (ev->key_code) {
        case SAPP_KEYCODE_W:      bit = 0; break;
        case SAPP_KEYCODE_S:      bit = 1; break;
        case SAPP_KEYCODE_A:      bit = 2; break;
        case SAPP_KEYCODE_D:      bit = 3; break;
        case SAPP_KEYCODE_SPACE:  bit = 4; break;
        case SAPP_KEYCODE_LEFT_SHIFT:
        case SAPP_KEYCODE_RIGHT_SHIFT: bit = 5; break;
        default: return;
    }

    if (bit >= 0) {
        if (down) key_state |=  (1u << bit);
        else      key_state &= ~(1u << bit);
    }
}

/* -------------------------------------------------------------
   Update cube position based on input
   ------------------------------------------------------------- */
static void update_cube(float dt) {
    const vec3_t cam_eye    = { 0.0f, 1.5f, 4.0f };
    const vec3_t cam_center = { 0.0f, 0.0f, 0.0f };
    const vec3_t cam_up     = { 0.0f, 1.0f, 0.0f };

    vec3_t forward = vec3_normalize(vec3_sub(cam_center, cam_eye));
    vec3_t right   = vec3_normalize(vec3_cross(forward, cam_up));

    const float speed  = state.move_speed * dt;
    const float vspeed = state.vertical_speed * dt;

    vec3_t delta = {0,0,0};

    if (KEY(0)) delta = vec3_add(delta, vec3_mulf(forward,  speed));  // W
    if (KEY(1)) delta = vec3_add(delta, vec3_mulf(forward, -speed));  // S
    if (KEY(2)) delta = vec3_add(delta, vec3_mulf(right,   -speed));  // A
    if (KEY(3)) delta = vec3_add(delta, vec3_mulf(right,    speed));  // D
    if (KEY(4)) delta.y +=  vspeed;                                    // Space
    if (KEY(5)) delta.y -=  vspeed;                                    // Shift

    state.cube_pos = vec3_add(state.cube_pos, delta);
}

/* -------------------------------------------------------------
   init()
   ------------------------------------------------------------- */
static void init(void) {
    LOG_INFO("load png cube", "init...");

    sg_setup(&(sg_desc){
        .environment = sglue_environment(),
        .logger.func = slog_func,
    });
    __dbgui_setup(sapp_sample_count());

    sfetch_setup(&(sfetch_desc_t){
        .max_requests = 1,
        .num_channels = 1,
        .num_lanes = 1,
        .logger.func = slog_func,
    });

    state.pass_action = (sg_pass_action) {
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = { 0.125f, 0.25f, 0.35f, 1.0f } }
    };

    state.bind.views[VIEW_tex] = sg_alloc_view();
    state.bind.samplers[SMP_smp] = sg_make_sampler(&(sg_sampler_desc){
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .label = "png-sampler",
    });

    /* Cube vertices */
    const vertex_t vertices[] = {
        { -1.0f, -1.0f, -1.0f,      0,     0 },
        {  1.0f, -1.0f, -1.0f,  32767,     0 },
        {  1.0f,  1.0f, -1.0f,  32767, 32767 },
        { -1.0f,  1.0f, -1.0f,      0, 32767 },

        { -1.0f, -1.0f,  1.0f,      0,     0 },
        {  1.0f, -1.0f,  1.0f,  32767,     0 },
        {  1.0f,  1.0f,  1.0f,  32767, 32767 },
        { -1.0f,  1.0f,  1.0f,      0, 32767 },

        { -1.0f, -1.0f, -1.0f,      0,     0 },
        { -1.0f,  1.0f, -1.0f,  32767,     0 },
        { -1.0f,  1.0f,  1.0f,  32767, 32767 },
        { -1.0f, -1.0f,  1.0f,      0, 32767 },

        {  1.0f, -1.0f, -1.0f,      0,     0 },
        {  1.0f,  1.0f, -1.0f,  32767,     0 },
        {  1.0f,  1.0f,  1.0f,  32767, 32767 },
        {  1.0f, -1.0f,  1.0f,      0, 32767 },

        { -1.0f, -1.0f, -1.0f,      0,     0 },
        { -1.0f, -1.0f,  1.0f,  32767,     0 },
        {  1.0f, -1.0f,  1.0f,  32767, 32767 },
        {  1.0f, -1.0f, -1.0f,      0, 32767 },

        { -1.0f,  1.0f, -1.0f,      0,     0 },
        { -1.0f,  1.0f,  1.0f,  32767,     0 },
        {  1.0f,  1.0f,  1.0f,  32767, 32767 },
        {  1.0f,  1.0f, -1.0f,      0, 32767 },
    };
    state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
        .data = SG_RANGE(vertices),
        .label = "cube-vertices"
    });

    const uint16_t indices[] = {
        0, 1, 2,  0, 2, 3,
        6, 5, 4,  7, 6, 4,
        8, 9,10,  8,10,11,
       14,13,12, 15,14,12,
       16,17,18, 16,18,19,
       22,21,20, 23,22,20
    };
    state.bind.index_buffer = sg_make_buffer(&(sg_buffer_desc){
        .usage.index_buffer = true, // update sokol_gfx.h
        .data = SG_RANGE(indices),
        .label = "cube-indices"
    });

    state.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = sg_make_shader(loadpng_shader_desc(sg_query_backend())),
        .layout = {
            .attrs = {
                [ATTR_loadpng_pos]       = { .format = SG_VERTEXFORMAT_FLOAT3 },
                [ATTR_loadpng_texcoord0] = { .format = SG_VERTEXFORMAT_SHORT2N }
            }
        },
        .index_type = SG_INDEXTYPE_UINT16,
        .cull_mode = SG_CULLMODE_BACK,
        .depth = {
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true
        },
        .label = "cube-pipeline"
    });

    /* Initialize movement */
    state.cube_pos = (vec3_t){0,0,0};
    state.move_speed = 3.0f;
    state.vertical_speed = 2.0f;

    /* Start loading PNG */
    char path_buf[512];
    sfetch_send(&(sfetch_request_t){
        .path = fileutil_get_path("grass16x16.png", path_buf, sizeof(path_buf)),
        .callback = fetch_callback,
        .buffer = SFETCH_RANGE(state.file_buffer)
    });
}

/* -------------------------------------------------------------
   frame()
   ------------------------------------------------------------- */
static void frame(void) {
    sfetch_dowork();

    const float dt = (float)sapp_frame_duration();
    const float t  = dt * 60.0f;

    //state.rx += 1.0f * t;
    //state.ry += 2.0f * t;

    update_cube(dt);

    const vs_params_t vs_params = compute_vsparams(state.rx, state.ry, state.cube_pos);

    sg_begin_pass(&(sg_pass){ .action = state.pass_action, .swapchain = sglue_swapchain() });
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);
    sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));
    sg_draw(0, 36, 1);
    __dbgui_draw();
    sg_end_pass();
    sg_commit();
}

/* -------------------------------------------------------------
   cleanup()
   ------------------------------------------------------------- */
static void cleanup(void) {
    __dbgui_shutdown();
    sfetch_shutdown();
    sg_shutdown();
}

/* -------------------------------------------------------------
   fetch_callback() – decode PNG and create texture
   ------------------------------------------------------------- */
static void fetch_callback(const sfetch_response_t* response) {
    if (response->fetched) {
        int w, h, n;
        const int req = 4;
        stbi_uc* pixels = stbi_load_from_memory(
            response->data.ptr, (int)response->data.size,
            &w, &h, &n, req);

        if (pixels) {
            sg_image img = sg_make_image(&(sg_image_desc){
                .width = w,
                .height = h,
                .pixel_format = SG_PIXELFORMAT_RGBA8,
                .data.mip_levels[0] = {
                    .ptr = pixels,
                    .size = (size_t)(w * h * 4)
                },
                .label = "png-image"
            });
            stbi_image_free(pixels);

            sg_init_view(state.bind.views[VIEW_tex], &(sg_view_desc){
                .texture = { .image = img },
                .label = "png-texture-view"
            });
        }
    } else if (response->failed) {
        state.pass_action.colors[0].clear_value = (sg_color){1.0f, 0.0f, 0.0f, 1.0f};
    }
}

/* -------------------------------------------------------------
   compute_vsparams() – includes cube translation
   ------------------------------------------------------------- */
static vs_params_t compute_vsparams(float rx, float ry, vec3_t cube_pos) {
    const float w = sapp_widthf();
    const float h = sapp_heightf();

    mat44_t proj = mat44_perspective_fov_rh(vm_radians(60.0f), w/h, 0.01f, 10.0f);
    mat44_t view = mat44_look_at_rh(
        (vec3_t){0.0f, 1.5f, 4.0f},
        (vec3_t){0.0f, 0.0f, 0.0f},
        (vec3_t){0.0f, 1.0f, 0.0f}
    );
    mat44_t view_proj = vm_mul(view, proj);

    mat44_t rxm = mat44_rotation_x(vm_radians(rx));
    mat44_t rym = mat44_rotation_y(vm_radians(ry));
    mat44_t rot = vm_mul(rym, rxm);

    mat44_t trans = mat44_translation(cube_pos.x, cube_pos.y, cube_pos.z);
    mat44_t model = vm_mul(trans, rot);

    return (vs_params_t){ .mvp = vm_mul(model, view_proj) };
}

/* -------------------------------------------------------------
   Event callback chain
   ------------------------------------------------------------- */
static void event_cb(const sapp_event* ev) {
    handle_input(ev);
    __dbgui_event(ev);
}

/* -------------------------------------------------------------
   sokol_main()
   ------------------------------------------------------------- */
sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    return (sapp_desc){
        .init_cb      = init,
        .frame_cb     = frame,
        .cleanup_cb   = cleanup,
        .event_cb     = event_cb,
        .width        = 800,
        .height       = 600,
        .sample_count = 4,
        .window_title = "Async PNG + WASD + Shift/Space",
        .icon.sokol_default = true,
        .logger.func  = slog_func,
        .win32_console_utf8 = true,
        .win32_console_attach = true,
    };
}